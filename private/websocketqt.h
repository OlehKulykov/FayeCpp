/*
 *   Copyright 2014 Kulykov Oleh
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */


#ifndef __FAYECPP_WEBSOCKETQT_H__
#define __FAYECPP_WEBSOCKETQT_H__

#include "../fayecpp.h"

#ifdef HAVE_SUITABLE_QT_VERSION

#include "transport.h"

#include <QObject>
#include <QString>
#include <QUrl>
#include <QtWebSockets/QtWebSockets>
#include <QWebSocket>

namespace FayeCpp {

class WebSocketQt : public QObject, public Transport
{
    Q_OBJECT
private:
    QWebSocket * _socket;

private slots:
    void connected();
    void disconnected();
//    void stateChanged(QAbstractSocket::SocketState state);
    void textMessageReceived(const QString & message);
    void binaryMessageReceived(const QByteArray & message);
    void error(QAbstractSocket::SocketError error);

public:
    virtual const std::string name() const;

    virtual void sendData(const std::vector<unsigned char> & data);
    virtual void sendText(const std::string & text);
    virtual void connectToServer();
    virtual void disconnectFromServer();
    WebSocketQt(ClassMethodWrapper<Client, void(Client::*)(Message*), Message> * processMethod);
    virtual ~WebSocketQt();

    static std::string transportName();
};

}


#endif /* HAVE_SUITABLE_QT_VERSION */

#endif /* __FAYECPP_WEBSOCKETQT_H__ */
