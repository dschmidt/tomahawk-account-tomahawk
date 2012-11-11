/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2012, Leo Franchi <lfranchi@kde.org>
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

#ifndef FACEBOOK_SIP_H
#define FACEBOOK_SIP_H

#include "accounts/AccountDllMacro.h"
#include "sip/SipPlugin.h"
#include "accounts/Account.h"

class ACCOUNTDLLEXPORT FacebookSipPlugin : public SipPlugin
{
    Q_OBJECT

public:
    FacebookSipPlugin( Tomahawk::Accounts::Account *account );

    virtual ~FacebookSipPlugin();

    virtual bool isValid() const;
    virtual Tomahawk::Accounts::Account::ConnectionState connectionState() const { return Tomahawk::Accounts::Account::Disconnected; }

signals:
    void stateChanged( Tomahawk::Accounts::Account::ConnectionState );

public slots:
    virtual void connectPlugin();
    void disconnectPlugin();
    void checkSettings() {}
    void configurationChanged() {}
    void addContact( const QString &, const QString& ) {}
    void sendMsg( const QString&, const SipInfo& ) {}
};

#endif
