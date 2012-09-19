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

TomahawkSipPlugin::TomahawkSipPlugin( Tomahawk::Accounts::Account *account )
    : SipPlugin( account )
    , m_state( Tomahawk::Accounts::Account::Disconnected )
{
    tLog() << Q_FUNC_INFO;

    connect( m_account, SIGNAL( completedLogin() ), this, SLOT( makeWsConnection() ) );
}


TomahawkSipPlugin::~TomahawkSipPlugin()
{

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


void
TomahawkSipPlugin::onWsOpened()
{
    tLog() << Q_FUNC_INFO << "WebSocket opened";
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
}