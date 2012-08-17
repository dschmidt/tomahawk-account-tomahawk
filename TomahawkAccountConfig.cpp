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

#include "TomahawkAccountConfig.h"
#include "utils/TomahawkUtils.h"
#include "utils/Closure.h"
#include "utils/Logger.h"

#include "ui_TomahawkAccountConfig.h"

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

#include <parser.h>
#include <serializer.h>

using namespace Tomahawk;
using namespace Accounts;

#define AUTH_SERVER "https://auth.jefferai.org"

TomahawkAccountConfig::TomahawkAccountConfig( QWidget* parent, Qt::WindowFlags f )
    : QWidget( parent, f )
    , m_ui( new Ui::TomahawkAccountConfig )
{
    m_ui->setupUi( this );

    connect( m_ui->registerbutton, SIGNAL( clicked( bool ) ), this, SLOT( registerClicked() ) );
    connect( m_ui->loginButton, SIGNAL( clicked( bool ) ), this, SLOT( login() ) );
}

TomahawkAccountConfig::~TomahawkAccountConfig()
{

}


void
TomahawkAccountConfig::registerClicked()
{
    if ( m_ui->usernameEdit->text().isEmpty() )
    {
        return;
    }


    // Register username
    const QString username = m_ui->usernameEdit->text();

    QVariantMap registerCmd;
    registerCmd[ "command" ] = "register";
    registerCmd[ "email" ] = "lfranchi@kde.org";
    registerCmd[ "password" ] = "mypw";
    registerCmd[ "username" ] = username;

    QNetworkReply* reply = buildRequest( "signup", registerCmd );
    NewClosure( reply, SIGNAL( finished() ), this, SLOT( onRegisterFinished( QNetworkReply* ) ), reply );
}


void
TomahawkAccountConfig::login()
{

}


void
TomahawkAccountConfig::onRegisterFinished( QNetworkReply* reply )
{
    Q_ASSERT( reply );

    if ( reply->error() == QNetworkReply::NoError )
    {
        const QByteArray data = reply->readAll();
        tDebug() << "Successfully ran register command:" << data;
    }
    else
    {
        const QByteArray data = reply->readAll();
        tLog() << "Error in register command:" << reply->error() << reply->errorString() << "body:" << data;
    }

    reply->deleteLater();
}


QNetworkReply*
TomahawkAccountConfig::buildRequest( const QString& command, const QVariantMap& params ) const
{
    QJson::Serializer s;
    const QByteArray msgJson = s.serialize( params );

    QNetworkRequest req( QUrl( QString( "%1/%2" ).arg( AUTH_SERVER ).arg( command ) ) );
    req.setHeader( QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8" );
    QNetworkReply* reply = TomahawkUtils::nam()->post( req, msgJson );

    return reply;
}
