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
#ifndef WEBSOCKET__H
#define WEBSOCKET__H

#include "DllMacro.h"

#include "hatchet_config.hpp"
#include <websocketpp/client.hpp>

#include <QPointer>
#include <QSslSocket>
#include <QTimer>
#include <QUrl>

#include <memory>

typedef typename websocketpp::client< websocketpp::config::hatchet_client > hatchet_client;

class DLLEXPORT WebSocket : public QObject
{
    Q_OBJECT
public:
    explicit WebSocket( const QString& url );
    virtual ~WebSocket();

signals:
    void connected();
    void disconnected();

public slots:
    void setUrl( const QString& url );
    void connectWs();
    void disconnectWs();

//    void send(const QString& msg);
//    void stop();

//signals:
//    void message(const QString& msg);

//    void opened();
//    void closed(const QString& reason);
//    void failed(const QString& reason);

private slots:
    void socketStateChanged( QAbstractSocket::SocketState state );
    void sslErrors( const QList< QSslError >& errors );
    void encrypted();
    void reconnectWs();
    void ioTimeout();

private:
    Q_DISABLE_COPY( WebSocket )

    QUrl m_url;
    std::ostringstream m_outputStream;
    std::unique_ptr< hatchet_client > m_client;
    hatchet_client::connection_ptr m_connection;
    QPointer< QSslSocket > m_socket;
    QPointer< QTimer > m_ioTimer;
    QAbstractSocket::SocketState m_lastSocketState;
};

#endif
