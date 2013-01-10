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

#include "FacebookAccount.h"

#include "FacebookAccountConfig.h"
#include "FacebookSip.h"
#include "utils/Closure.h"
#include "utils/Logger.h"
#include "utils/TomahawkUtils.h"

#include <qjson/parser.h>
#include <qjson/serializer.h>

#include <QtPlugin>

using namespace Tomahawk;
using namespace Accounts;

static QPixmap* s_icon = 0;

FacebookAccountFactory::FacebookAccountFactory()
{
#ifndef ENABLE_HEADLESS
    if ( s_icon == 0 )
        s_icon = new QPixmap( ":/facebook-icon-64x64.png" );
#endif
}


FacebookAccountFactory::~FacebookAccountFactory()
{

}


QPixmap
FacebookAccountFactory::icon() const
{
    return *s_icon;
}


Account*
FacebookAccountFactory::createAccount( const QString& pluginId )
{
    return new FacebookAccount( pluginId.isEmpty() ? generateId( factoryId() ) : pluginId );
}


// Facebook account

FacebookAccount::FacebookAccount( const QString& accountId )
    : Account( accountId )
{
}


FacebookAccount::~FacebookAccount()
{

}


QWidget*
FacebookAccount::configurationWidget()
{
    if ( m_configWidget.isNull() )
        m_configWidget = QWeakPointer<FacebookAccountConfig>( new FacebookAccountConfig( this ) );

    return m_configWidget.data();
}


void
FacebookAccount::authenticate()
{
    if ( connectionState() == Connected )
        return;


}


void
FacebookAccount::deauthenticate()
{
    if ( !m_sipPlugin.isNull() )
        sipPlugin()->disconnectPlugin();
}


Account::ConnectionState
FacebookAccount::connectionState() const
{
    return Disconnected;
}


SipPlugin*
FacebookAccount::sipPlugin()
{
    if ( m_sipPlugin.isNull() )
    {
        m_sipPlugin = QWeakPointer< FacebookSipPlugin >( new FacebookSipPlugin( this ) );

        connect( m_sipPlugin.data(), SIGNAL( stateChanged( Tomahawk::Accounts::Account::ConnectionState ) ), this, SIGNAL( connectionStateChanged( Tomahawk::Accounts::Account::ConnectionState ) ) );
        return m_sipPlugin.data();
    }
    return m_sipPlugin.data();
}


QPixmap
FacebookAccount::icon() const
{
    return *s_icon;
}


bool
FacebookAccount::isAuthenticated() const
{
    return false;
}

Q_EXPORT_PLUGIN2( Tomahawk::Accounts::AccountFactory, Tomahawk::Accounts::FacebookAccountFactory )
