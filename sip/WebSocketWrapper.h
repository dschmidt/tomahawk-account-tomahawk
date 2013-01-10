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
#ifndef WEBSOCKET_WRAPPER_H
#define WEBSOCKET_WRAPPER_H

#include "DllMacro.h"

#include <QThread>

/**
 * A Qt wrapper around websocket++'s boost::asio-based websockets.
 *
 * One wrapper per connection. Pass in the desired endpoint in the constructor.
 *
 *
 *
 */
class WebSocketWrapperPrivate;
class DLLEXPORT WebSocketWrapper : public QThread
{
    Q_OBJECT
public:
    explicit WebSocketWrapper(const QString& url, QObject* parent = 0);
    virtual ~WebSocketWrapper();

    void send(const QString& msg);
    void stop();

signals:
    void message(const QString& msg);

    void opened();
    void closed(const QString& reason);
    void failed(const QString& reason);

protected:
    void run();

private:
    Q_DISABLE_COPY(WebSocketWrapper)

    QScopedPointer<WebSocketWrapperPrivate> pimpl;
    friend class WebSocketWrapperPrivate;
};

#endif
