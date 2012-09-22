/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2012, Jeff Mitchell <jeff@tomahawk-player.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TomahawkSip.h"
#include <accounts/tomahawk/TomahawkAccount.h>

#include <network/Servent.h>
#include <utils/WebSocketWrapper.h>
#include <database/Database.h>
#include <database/DatabaseImpl.h>
#include "network/ControlConnection.h"

TomahawkSipPlugin::TomahawkSipPlugin( Tomahawk::Accounts::Account *account )
    : SipPlugin( account )
    , m_sipState( Closed )
    , m_state( Tomahawk::Accounts::Account::Disconnected )
{
    tLog() << Q_FUNC_INFO;

    connect( m_account, SIGNAL( accessTokensFetched() ), this, SLOT( makeWsConnection() ) );
}


TomahawkSipPlugin::~TomahawkSipPlugin()
{
    if ( !m_ws.isNull() )
    {
        m_ws.data()->stop();
        m_ws.clear();
    }

    m_sipState = Closed;
    m_state = Tomahawk::Accounts::Account::Disconnected;

    foreach ( QString dbid, m_knownPeers.keys() )
        delete m_knownPeers[ dbid ];
}


bool
TomahawkSipPlugin::isValid() const
{
    return m_account->enabled() && m_account->isAuthenticated();
}


void
TomahawkSipPlugin::connectPlugin()
{
    if ( !m_account->isAuthenticated() )
    {
        //FIXME: Prompt user for password?
        return;
    }

    m_state = Tomahawk::Accounts::Account::Connecting;
    emit stateChanged( m_state );
    qobject_cast< Tomahawk::Accounts::TomahawkAccount* >( account() )->fetchAccessTokens();
}


void
TomahawkSipPlugin::disconnectPlugin()
{
    if ( !m_ws.isNull() )
    {
        m_ws.data()->stop();
        m_ws.clear();
    }

    m_sipState = Closed;
    m_state = Tomahawk::Accounts::Account::Disconnected;
    emit stateChanged( m_state );
}


void
TomahawkSipPlugin::makeWsConnection()
{
    //Other things can request access tokens, so if we're already connected there's no need to pay attention
    if ( !m_ws.isNull() )
        return;
    
    QVariantMap tokensCreds = m_account->credentials()[ "accesstokens" ].toMap();
    //FIXME: Don't blindly pick the first one that matches?
    QVariantMap connectVals;
    foreach ( QString token, tokensCreds.keys() )
    {
        QVariantList tokenList = tokensCreds[ token ].toList();
        foreach ( QVariant tokenListVar, tokenList )
        {
            QVariantMap tokenListVal = tokenListVar.toMap();
            if ( tokenListVal.contains( "type" ) && tokenListVal[ "type" ].toString() == "sync" )
            {
                connectVals = tokenListVal;
                m_token = token;
                break;
            }
        }
        if ( !connectVals.isEmpty() )
            break;
    }

    QString url;
    if ( !connectVals.isEmpty() )
        url = connectVals[ "host" ].toString() + ':' + connectVals[ "port" ].toString();

    if ( url.isEmpty() )
    {
        tLog() << Q_FUNC_INFO << "Unable to find a proper connection endpoint; bailing";
        disconnectPlugin();
        return;
    }
    
    m_ws = QWeakPointer< WebSocketWrapper >( new WebSocketWrapper( url ) );
    connect( m_ws.data(), SIGNAL( opened() ), this, SLOT( onWsOpened() ) );
    connect( m_ws.data(), SIGNAL( failed( QString ) ), this, SLOT( onWsFailed( QString ) ) );
    connect( m_ws.data(), SIGNAL( closed( QString ) ), this, SLOT( onWsClosed( QString ) ) );
    connect( m_ws.data(), SIGNAL( message( QString ) ), this, SLOT( onWsMessage( QString ) ) );
    m_ws.data()->start();

    m_state = Tomahawk::Accounts::Account::Connected;
    emit stateChanged( m_state );
}

bool
TomahawkSipPlugin::sendBytes( QVariantMap jsonMap )
{
    tLog() << Q_FUNC_INFO;
    if ( m_sipState == Closed )
    {
        tLog() << Q_FUNC_INFO << "was told to send bytes on a closed connection, not gonna do it";
        return false;
    }
    
    QJson::Serializer serializer;
    QByteArray bytes = serializer.serialize( jsonMap );
    if ( bytes.isEmpty() )
    {
        tLog() << Q_FUNC_INFO << "could not serialize register structure to JSON";
        return false;
    }

    m_ws.data()->send( bytes );
    return true;
}

void
TomahawkSipPlugin::onWsOpened()
{
    tLog() << Q_FUNC_INFO << "WebSocket opened";

    if ( m_token.isEmpty() || !m_account->credentials().contains( "username" ) )
    {
        tLog() << Q_FUNC_INFO << "access token or username is empty, aborting";
        return;
    }
    
    m_sipState = Registering;
    
    QVariantMap registerMap;
    registerMap[ "command" ] = "register";
    registerMap[ "host" ] = Servent::instance()->externalAddress();
    registerMap[ "port" ] = Servent::instance()->externalPort();
    registerMap[ "dbid" ] = Database::instance()->impl()->dbid();
    registerMap[ "accesstoken" ] = m_token;
    registerMap[ "username" ] = m_account->credentials()[ "username" ].toString();

    if ( !sendBytes( registerMap ) )
    {
        tLog() << Q_FUNC_INFO << "Failed sending message";
        m_sipState = Closed;
        return;
    }
}


void
TomahawkSipPlugin::onWsFailed( const QString &msg )
{
    tLog() << Q_FUNC_INFO << "WebSocket failed with message: " << msg;
    disconnectPlugin();
}


void
TomahawkSipPlugin::onWsClosed( const QString &msg )
{
    tLog() << Q_FUNC_INFO << "WebSocket closed with message: " << msg;
    disconnectPlugin();
}


void
TomahawkSipPlugin::onWsMessage( const QString &msg )
{
    tLog() << Q_FUNC_INFO << "WebSocket message: " << msg;

    QJson::Parser parser;
    bool ok;
    QVariant jsonVariant = parser.parse( msg.toUtf8(), &ok );
    if ( !jsonVariant.isValid() )
    {
        tLog() << Q_FUNC_INFO << "Failed to parse message back from server";
        return;
    }

    QVariantMap retMap = jsonVariant.toMap();

    if ( m_sipState == Registering )
    {
        tLog() << Q_FUNC_INFO << "In registering state, checking status of registration";
        if ( retMap.contains( "status" ) &&
                retMap[ "status" ].toString() == "success" )
        {
            tLog() << Q_FUNC_INFO << "Registered successfully";
            m_sipState = Connected;
            m_state = Tomahawk::Accounts::Account::Connected;
            emit stateChanged( m_state );
            return;
        }
        else
        {
            tLog() << Q_FUNC_INFO << "Failed to register successfully";
            m_ws.data()->stop();
        }
    }
    else if ( m_sipState != Connected )
    {
        // ...erm?
        tLog() << Q_FUNC_INFO << "Got a message from a non connected socket?";
        return;
    }
    else if ( !retMap.contains( "command" ) ||
                !retMap[ "command" ].canConvert< QString >() )
    {
        tLog() << Q_FUNC_INFO << "Unable to convert and/or interepret command from server";
        return;
    }

    QString command = retMap[ "command" ].toString();

    if ( command == "new-peer" )
        newPeer( retMap );
    else if ( command == "peer-authorization" )
        peerAuthorization( retMap );
}


bool
TomahawkSipPlugin::checkKeys( QStringList keys, QVariantMap map )
{
    foreach ( QString key, keys )
    {
        if ( !map.contains( key ) )
        {
            tLog() << Q_FUNC_INFO << "Did not find the value" << key << "in the new-peer structure";
            return false;
        }
    }
    return true;
}


void
TomahawkSipPlugin::newPeer( QVariantMap valMap )
{
    tLog() << Q_FUNC_INFO;
    QStringList keys( QStringList() << "command" << "username" << "host" << "port" << "dbid" );
    if ( !checkKeys( keys, valMap ) )
        return;

    if( !Servent::instance()->visibleExternally() )
    {
        tLog() << Q_FUNC_INFO << "Not visible externally, so not creating an offer";
        return;
    }

    PeerInfo* info = new PeerInfo( valMap[ "username" ].toString(), valMap[ "host" ].toString(),
                        valMap[ "port" ].toUInt(), valMap[ "dbid" ].toString() );

    m_knownPeers[ valMap[ "dbid" ].toString() ] = info;
    
    QString key = uuid();
    ControlConnection* conn = new ControlConnection( Servent::instance(), QString() );

    const QString& nodeid = valMap[ "dbid" ].toString();
    conn->setName( valMap[ "username" ].toString() );
    conn->setId( nodeid );

    Servent::instance()->registerOffer( key, conn );

    QVariantMap sendMap;
    sendMap[ "command" ] = "authorize-peer";
    sendMap[ "dbid" ] = Database::instance()->impl()->dbid();
    sendMap[ "offer" ] = key;

    if ( !sendBytes( sendMap ) )
    {
        tLog() << Q_FUNC_INFO << "Failed sending message";
        return;
    }
}


void
TomahawkSipPlugin::peerAuthorization( QVariantMap valMap )
{
    tLog() << Q_FUNC_INFO;
    QStringList keys( QStringList() << "command" << "dbid" << "offer" );
    if ( !checkKeys( keys, valMap ) )
        return;

    if ( !m_knownPeers.contains( valMap[ "dbid" ].toString() ) )
    {
        tLog() << Q_FUNC_INFO << "Received a peer-authorization for a peer we don't know about";
        return;
    }

    PeerInfo* info = m_knownPeers[ valMap[ "dbid" ].toString() ];
    Servent::instance()->connectToPeer( info->host, info->port, valMap[ "offer" ].toString(), info->username, info->dbid );
}