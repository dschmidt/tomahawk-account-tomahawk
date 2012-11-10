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

#ifndef FACEBOOK_ACCOUNT_CONFIG_H
#define FACEBOOK_ACCOUNT_CONFIG_H

#include <QWidget>
#include <QVariantMap>

class QNetworkReply;

namespace Ui {
    class FacebookAccountConfig;
};

namespace Tomahawk {
namespace Accounts {

class FacebookAccount;

class FacebookAccountConfig : public QWidget
{
    Q_OBJECT
public:
    explicit FacebookAccountConfig( FacebookAccount* account );
    virtual ~FacebookAccountConfig();

private slots:

private:
    Ui::FacebookAccountConfig* m_ui;
    FacebookAccount* m_account;
};

}
}

#endif
