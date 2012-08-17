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

#include <QtPlugin>

using namespace Tomahawk;
using namespace Accounts;

static QPixmap* s_icon = 0;

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

}


TomahawkAccount::~TomahawkAccount()
{

}


QWidget*
TomahawkAccount::configurationWidget()
{
    if ( m_configWidget.isNull() )
        m_configWidget = QWeakPointer<TomahawkAccountConfig>( new TomahawkAccountConfig );

    return m_configWidget.data();
}


void
TomahawkAccount::authenticate()
{

}


void
TomahawkAccount::deauthenticate()
{

}


Account::ConnectionState
TomahawkAccount::connectionState() const
{
    return Account::Disconnected;
}


SipPlugin*
TomahawkAccount::sipPlugin()
{
    return 0;
}


QPixmap
TomahawkAccount::icon() const
{
    return *s_icon;
}


bool
TomahawkAccount::isAuthenticated() const
{
    return false;
}

Q_EXPORT_PLUGIN2( Tomahawk::Accounts::AccountFactory, Tomahawk::Accounts::TomahawkAccountFactory )
