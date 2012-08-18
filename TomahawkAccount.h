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

#ifndef TOMAHAWK_ACCOUNT_H
#define TOMAHAWK_ACCOUNT_H


#include "accounts/Account.h"
#include "../AccountDllMacro.h"

class SipPlugin;
class QNetworkReply;


namespace Tomahawk
{
namespace Accounts
{

class TomahawkAccountConfig;

class ACCOUNTDLLEXPORT TomahawkAccountFactory : public AccountFactory
{
    Q_OBJECT
    Q_INTERFACES( Tomahawk::Accounts::AccountFactory )
public:
    TomahawkAccountFactory();
    virtual ~TomahawkAccountFactory();

    virtual QString factoryId() const { return "tomahawkaccount"; }
    virtual QString prettyName() const { return "Tomahawk Online"; }
    virtual QString description() const { return tr( "Connect to your Tomahawk Online account" ); }
    virtual bool isUnique() const { return true; }
    AccountTypes types() const { return AccountTypes( SipType | InfoType | StatusPushType ); };
#ifndef ENABLE_HEADLESS
    virtual QPixmap icon() const;
#endif


    virtual Account* createAccount ( const QString& pluginId = QString() );
};

class ACCOUNTDLLEXPORT TomahawkAccount : public Account
{
    Q_OBJECT
public:
    TomahawkAccount( const QString &accountId );
    virtual ~TomahawkAccount();

    QPixmap icon() const;

    void authenticate();
    void deauthenticate();
    bool isAuthenticated() const;
    ConnectionState connectionState() const;

    virtual Tomahawk::InfoSystem::InfoPluginPtr infoPlugin() { return Tomahawk::InfoSystem::InfoPluginPtr(); }
    SipPlugin* sipPlugin();

    QWidget* configurationWidget();
    QWidget* aclWidget() { return 0; }

    bool loggedIn() const;
    QString username() const;

signals:
    void completedLogin();
    void completedLogout();

    void registerFinished( bool successful, const QString& msg );

private slots:
    void onRegisterFinished( QNetworkReply*, const QString& username, const QString& password );
    void onPasswordLoginFinished( QNetworkReply*, const QString& username );
    void onAuthtokenLoginFinished( QNetworkReply*, const QString& username, const QByteArray& authToken );

    void onLoggedIn( bool loggedIn );

private:
    QByteArray authToken() const;

    void doRegister( const QString& username, const QString& password, const QString& email );
    void loginWithPassword( const QString& username, const QString& password );
    void loginWithAuthToken( const QString& username, const QByteArray& authToken );
    void logout();

    QNetworkReply* buildRequest( const QString& command, const QVariantMap& params ) const;
    QVariantMap parseReply( QNetworkReply* reply, bool& ok ) const;

    QWeakPointer<TomahawkAccountConfig> m_configWidget;

//     QVariantMap m_creds;
    bool m_loggedIn;
    Account::ConnectionState m_state;

    friend class TomahawkAccountConfig;
};

}
}


#endif

class QNetworkReply;
