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

#include "FacebookAuthDialog.h"

#include "utils/Logger.h"

#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QHBoxLayout>
#include <QPushButton>

FacebookAuthDialog::FacebookAuthDialog( const QUrl &authUrl, QWidget *parent )
  : QDialog( parent, Qt::Sheet )
  , m_webView( new QWebView( this ) )
  , m_authUrl( authUrl )
  , m_redirectedToHatchet( false )
{
    setLayout( new QVBoxLayout );

    layout()->addWidget( m_webView );
    connect( m_webView, SIGNAL( urlChanged( QUrl ) ), this, SLOT( urlChanged( QUrl ) ) );
    connect( m_webView, SIGNAL( loadFinished( bool ) ), this, SLOT( loadFinished( bool ) ) );

    m_webView->load( authUrl );
}

FacebookAuthDialog::~FacebookAuthDialog()
{}


void
FacebookAuthDialog::urlChanged( const QUrl& url )
{
    tLog() << "Facebook dialog url: " << url << url.host();
    if ( url.host() == "hatchet.toma.hk" )
        m_redirectedToHatchet = true;
}


void
FacebookAuthDialog::loadFinished( bool ok )
{
    tLog() << "Load finished! body:" << m_webView->page()->mainFrame()->toPlainText() << m_redirectedToHatchet;
    if ( m_redirectedToHatchet )
        accept();
}
