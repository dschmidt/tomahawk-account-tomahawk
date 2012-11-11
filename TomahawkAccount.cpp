/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2012 Leo Franchi <lfranchi@kde.org>
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

#include "TomahawkAccount.h"

#include "TomahawkAccountConfig.h"
#include "utils/Closure.h"
#include "utils/Logger.h"
#include "sip/TomahawkSip.h"
#include "utils/TomahawkUtils.h"

#include <QtPlugin>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

#include <parser.h>
#include <serializer.h>

using namespace Tomahawk;
using namespace Accounts;

static QPixmap* s_icon = 0;
TomahawkAccount* TomahawkAccount::s_instance  = 0;

#define AUTH_SERVER "http://auth.toma.hk"

TomahawkAccountFactory::TomahawkAccountFactory()
{
#ifndef ENABLE_HEADLESS
    if ( s_icon == 0 )
        s_icon = new QPixmap( ":/tomahawk-icon-64x64.png" );
#endif
}


TomahawkAccountFactory::~TomahawkAccountFactory()
{

}


QPixmap
TomahawkAccountFactory::icon() const
{
    return *s_icon;
}


Account*
TomahawkAccountFactory::createAccount( const QString& pluginId )
{
    return new TomahawkAccount( pluginId.isEmpty() ? generateId( factoryId() ) : pluginId );
}


// Tomahawk account

TomahawkAccount::TomahawkAccount( const QString& accountId )
    : Account( accountId )
{
    s_instance = this;
}


TomahawkAccount::~TomahawkAccount()
{

}


TomahawkAccount*
TomahawkAccount::instance()
{
    return s_instance;
}


QWidget*
TomahawkAccount::configurationWidget()
{
    if ( m_configWidget.isNull() )
        m_configWidget = QWeakPointer<TomahawkAccountConfig>( new TomahawkAccountConfig( this ) );

    return m_configWidget.data();
}


void
TomahawkAccount::authenticate()
{
    if ( connectionState() == Connected )
        return;

    if ( !authToken().isEmpty() )
    {
        qDebug() << "Have saved credentials with auth token:" << authToken();
        if ( sipPlugin() )
            sipPlugin()->connectPlugin();
    }
    else if ( !username().isEmpty() )
    {
        // Need to re-prompt for password, since we don't save it!
    }
}


void
TomahawkAccount::deauthenticate()
{
    if ( !m_tomahawkSipPlugin.isNull() )
        sipPlugin()->disconnectPlugin();
    emit deauthenticated();
}


void
TomahawkAccount::setConnectionState( Account::ConnectionState connectionState )
{
    m_state = connectionState;

    emit connectionStateChanged( connectionState );
}


Account::ConnectionState
TomahawkAccount::connectionState() const
{
    return m_state;
}


SipPlugin*
TomahawkAccount::sipPlugin()
{
    if ( m_tomahawkSipPlugin.isNull() )
    {
        tLog() << Q_FUNC_INFO;
        m_tomahawkSipPlugin = QWeakPointer< TomahawkSipPlugin >( new TomahawkSipPlugin( this ) );
        connect( m_tomahawkSipPlugin.data(), SIGNAL( authUrlDiscovered( Tomahawk::Accounts::TomahawkAccount::Service, QString ) ),
                 this, SLOT( authUrlDiscovered( Tomahawk::Accounts::TomahawkAccount::Service, QString ) ) );

        return m_tomahawkSipPlugin.data();
    }
    return m_tomahawkSipPlugin.data();
}


QPixmap
TomahawkAccount::icon() const
{
    return *s_icon;
}


bool
TomahawkAccount::isAuthenticated() const
{
    return credentials().contains( "authtoken" );
}


QString
TomahawkAccount::username() const
{
    return credentials().value( "username" ).toString();
}


QByteArray
TomahawkAccount::authToken() const
{
    return credentials().value( "authtoken" ).toByteArray();
}


void
TomahawkAccount::doRegister( const QString& username, const QString& password, const QString& email )
{
    if ( username.isEmpty() || password.isEmpty() || email.isEmpty() )
    {
        return;
    }

    QVariantMap registerCmd;
    registerCmd[ "command" ] = "register";
    registerCmd[ "email" ] = email;
    registerCmd[ "password" ] = password;
    registerCmd[ "username" ] = username;

    QNetworkReply* reply = buildRequest( "signup", registerCmd );
    connect( reply, SIGNAL( finished() ), this, SLOT( onRegisterFinished() ) );
}


void
TomahawkAccount::loginWithPassword( const QString& username, const QString& password )
{
    if ( username.isEmpty() || password.isEmpty() )
    {
        tLog() << "No tomahawk account username or pw, not logging in";
        return;
    }

    QVariantMap params;
    params[ "password" ] = password;
    params[ "username" ] = username;

    QNetworkReply* reply = buildRequest( "login", params );
    NewClosure( reply, SIGNAL( finished() ), this, SLOT( onPasswordLoginFinished( QNetworkReply*, const QString& ) ), reply, username );
}


void
TomahawkAccount::fetchAccessTokens()
{
    if ( authToken().isEmpty() )
    {
        tLog() << "No authToken, not logging in";
        return;
    }

    QVariantMap params;
    params[ "authtoken" ] = authToken();

    tLog() << "Fetching access tokens";
    QNetworkReply* reply = buildRequest( "tokens", params );
    connect( reply, SIGNAL( finished() ), this, SLOT( onFetchAccessTokensFinished() ) );
}


void
TomahawkAccount::onRegisterFinished()
{
    QNetworkReply* reply = qobject_cast< QNetworkReply* >( sender() );
    Q_ASSERT( reply );
    bool ok;
    const QVariantMap resp = parseReply( reply, ok );
    if ( !ok )
    {
        emit registerFinished( false, resp.value( "errormsg" ).toString() );
        return;
    }

    emit registerFinished( true, QString() );
}


void
TomahawkAccount::onPasswordLoginFinished( QNetworkReply* reply, const QString& username )
{
    Q_ASSERT( reply );
    bool ok;
    const QVariantMap resp = parseReply( reply, ok );
    if ( !ok )
        return;

    const QByteArray authenticationToken = resp.value( "message" ).toMap().value( "authtoken" ).toMap().value("token").toByteArray();

    QVariantHash creds = credentials();
    creds[ "username" ] = username;
    creds[ "authtoken" ] = authenticationToken;
    setCredentials( creds );
    syncConfig();
    
    if ( !authenticationToken.isEmpty() )
    {
        // We're succesful! Now log in with our authtoken for access
        fetchAccessTokens();
    }
}


void
TomahawkAccount::onFetchAccessTokensFinished()
{
    QNetworkReply* reply = qobject_cast< QNetworkReply* >( sender() );
    Q_ASSERT( reply );
    bool ok;
    const QVariantMap resp = parseReply( reply, ok );
    if ( !ok )
    {
        if ( resp["code"].toInt() == 140 )
        {
            tLog() << "Expired credentials, need to reauthenticate with password";
            QVariantHash creds = credentials();
            creds.remove( "authtoken" );
            setCredentials( creds );
            syncConfig();
        }
        else
            tLog() << "Unable to fetch access tokens";
        return;
    }

    QVariantHash creds = credentials();
    creds[ "accesstokens" ] = resp[ "message" ].toMap()[ "accesstokens" ];
    setCredentials( creds );
    syncConfig();

    emit accessTokensFetched();
}


QString
TomahawkAccount::authUrlForService( const Service &service ) const
{
    return m_extraAuthUrls.value( service, QString() );
}


void
TomahawkAccount::authUrlDiscovered( Service service, const QString &authUrl )
{
    m_extraAuthUrls[ service ] = authUrl;
}


QNetworkReply*
TomahawkAccount::buildRequest( const QString& command, const QVariantMap& params ) const
{
    QJson::Serializer s;
    const QByteArray msgJson = s.serialize( params );

    QNetworkRequest req( QUrl( QString( "%1/%2" ).arg( AUTH_SERVER ).arg( command ) ) );
    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8" );
    QNetworkReply* reply = TomahawkUtils::nam()->post( req, msgJson );

    return reply;
}


QVariantMap
TomahawkAccount::parseReply( QNetworkReply* reply, bool& okRet ) const
{
    QVariantMap resp;

    reply->deleteLater();

    if ( reply->error() != QNetworkReply::NoError )
    {
        tLog() << "Network error in command:" << reply->error() << reply->errorString();
        okRet = false;
        return resp;
    }

    QJson::Parser p;
    bool ok;
    resp = p.parse( reply, &ok ).toMap();

    if ( !ok || resp.value( "error", false ).toBool() )
    {
        tLog() << "Error from tomahawk server response, or in parsing from json:" << resp.value( "errormsg" ) << resp;
        okRet = false;
        return resp;
    }

    tLog() << "Got reply" << resp.keys();
    tLog() << "Got reply" << resp.values();
    okRet = true;
    return resp;
}

Q_EXPORT_PLUGIN2( Tomahawk::Accounts::AccountFactory, Tomahawk::Accounts::TomahawkAccountFactory )
