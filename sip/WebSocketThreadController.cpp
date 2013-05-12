/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2012,      Leo Franchi <lfranchi@kde.org>
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
#include "WebSocketThreadController.h"
#include "WebSocket.h"

#include <QDebug>

WebSocketThreadController::WebSocketThreadController( QObject* parent )
    : QThread( parent )
{
}


WebSocketThreadController::~WebSocketThreadController()
{
    if ( m_webSocket )
    {
        delete m_webSocket;
        m_webSocket = 0;
    }
}


void
WebSocketThreadController::setUrl( const QString &url )
{
    m_url = url;
    if ( m_webSocket )
    {
        m_webSocket->setUrl( url );
    }
}


void
WebSocketThreadController::run()
{
    m_webSocket = QPointer< WebSocket >( new WebSocket( m_url ) );
    if ( m_webSocket )
    {
        connect( m_webSocket, SIGNAL( connected() ), parent(), SLOT( webSocketConnected() ) );
        connect( m_webSocket, SIGNAL( disconnected() ), parent(), SLOT( webSocketDisconnected() ) );
        exec();
        delete m_webSocket;
        m_webSocket = 0;
    }
}
