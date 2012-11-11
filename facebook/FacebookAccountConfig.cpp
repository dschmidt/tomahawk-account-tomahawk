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

#include "FacebookAccountConfig.h"
#include "FacebookAccount.h"
#include "../TomahawkAccount.h"
#include "utils/TomahawkUtils.h"
#include "utils/Logger.h"
#include "FacebookAuthDialog.h"
#include "utils/Closure.h"

#include "ui_FacebookAccountConfig.h"

#include <QUrl>

using namespace Tomahawk;
using namespace Accounts;

FacebookAccountConfig::FacebookAccountConfig( FacebookAccount* account )
    : QWidget( 0 )
    , m_ui( new Ui::FacebookAccountConfig )
    , m_account( account )
{
    Q_ASSERT( m_account );

    m_ui->setupUi( this );
    toggleRegisterArea( false );

    connect( m_ui->connectButton, SIGNAL( clicked() ), this, SLOT( connectToFacebook() ) );
}

FacebookAccountConfig::~FacebookAccountConfig()
{

}

void
FacebookAccountConfig::showEvent( QShowEvent* event )
{
    m_tomahawkAccount = QWeakPointer<TomahawkAccount>( TomahawkAccount::instance() );
}

void
FacebookAccountConfig::connectToFacebook()
{
    if ( m_tomahawkAccount.isNull() )
    {
        tLog() << "No tomahawk account when trying to connect to facebook!?";
        return;
    }

    if ( !m_tomahawkAccount.data()->isAuthenticated() )
    {
        toggleRegisterArea( true );
    }
    else if ( !m_tomahawkAccount.data()->authUrlForService( TomahawkAccount::Facebook ).isEmpty() )
    {
        // Handle facebook register w/ stored auth from tomahawk account
        const QUrl authUrl = QUrl::fromUserInput( m_tomahawkAccount.data()->authUrlForService( TomahawkAccount::Facebook ) );
        tDebug() << "Auth with facebook with url:" << authUrl;
        FacebookAuthDialog* authdiag = new FacebookAuthDialog( authUrl, this );
        NewClosure( authdiag, SIGNAL( finished( int ) ), this, SLOT( authDialogFinished( FacebookAuthDialog* ) ), authdiag );
        authdiag->show();
    }
}


void
FacebookAccountConfig::authDialogFinished( FacebookAuthDialog* dialog )
{

}


// TODO make a reusable component from TomahawkAccountConfig
void
FacebookAccountConfig::toggleRegisterArea( bool show )
{
    m_ui->emailEdit->setVisible( show );
    m_ui->emailLabel->setVisible( show );
    m_ui->passwordEdit->setVisible( show );
    m_ui->passwordLabel->setVisible( show );
    m_ui->usernameEdit->setVisible( show );
    m_ui->usernameLabel->setVisible( show );
    m_ui->errorLabel->setVisible( show );
//    m_ui->registerSpacer->setVisible( show );
    m_ui->registerButton->setVisible( show );
}
