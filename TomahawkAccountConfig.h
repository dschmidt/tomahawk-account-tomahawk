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

#ifndef TOMAHAWK_ACCOUNT_CONFIG_H
#define TOMAHAWK_ACCOUNT_CONFIG_H

#include <QWidget>
#include <QVariantMap>

class QNetworkReply;

namespace Ui {
    class TomahawkAccountConfig;
};

namespace Tomahawk {
namespace Accounts {

class TomahawkAccount;

class TomahawkAccountConfig : public QWidget
{
    Q_OBJECT
public:
    explicit TomahawkAccountConfig( TomahawkAccount* account );
    virtual ~TomahawkAccountConfig();

private slots:
    void registerClicked();
    void loginOrRegister();

    void registerFinished( bool success, const QString& error );

    void fieldsChanged();

    void showLoggedIn();
    void showLoggedOut();

    void accountInfoUpdated();
private:
    Ui::TomahawkAccountConfig* m_ui;
    TomahawkAccount* m_account;
};

}
}

Q_DECLARE_METATYPE( QNetworkReply* )

#endif
