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

#ifndef TOMAHAWK_SIP_H
#define TOMAHAWK_SIP_H

#include "accounts/AccountDllMacro.h"
#include "sip/SipPlugin.h"
#include "../TomahawkAccount.h"

#include <QtCrypto>

class WebSocketWrapper;

class ACCOUNTDLLEXPORT TomahawkSipPlugin : public SipPlugin
{
    Q_OBJECT

    enum SipState {
        AcquiringVersion,
        Registering,
        Connected,
        Closed
    };

public:
    TomahawkSipPlugin( Tomahawk::Accounts::Account *account );

    virtual ~TomahawkSipPlugin();

    virtual bool isValid() const;

    virtual void sendSipInfo( const Tomahawk::peerinfo_ptr& receiver, const SipInfo& info );

public slots:
    virtual void connectPlugin();
    void disconnectPlugin();
    void checkSettings() {}
    void configurationChanged() {}
    void addContact( const QString &, const QString& ) {}
    void sendMsg( const QString&, const SipInfo& ) {}

signals:
    void authUrlDiscovered( Tomahawk::Accounts::TomahawkAccount::Service service, const QString& authUrl );

private slots:
    void makeWsConnection();
    void onWsOpened();
    void onWsFailed( const QString &msg );
    void onWsClosed( const QString &msg );
    void onWsMessage( const QString &msg );

private:
    bool sendBytes( QVariantMap jsonMap );
    bool checkKeys( QStringList keys, QVariantMap map );
    void newPeer( QVariantMap valMap );
    void peerAuthorization( QVariantMap valMap );
    Tomahawk::Accounts::TomahawkAccount* tomahawkAccount() const;

    QWeakPointer< WebSocketWrapper > m_ws;
    QString m_token;
    SipState m_sipState;
    int m_version;
    QCA::PublicKey* m_publicKey;
};

#endif
