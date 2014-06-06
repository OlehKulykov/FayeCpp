
#include "websocketqt.h"

#ifdef HAVE_SUITABLE_QT_VERSION

#include "../client.h"
#include "../message.h"

namespace FayeCpp {

void WebSocketQt::connected()
{
#ifdef DEBUG_QT
    qDebug() << "SocketQt:" << "connected";
#endif
    this->onConnected();
}

void WebSocketQt::disconnected()
{
#ifdef DEBUG_QT
    qDebug() << "SocketQt:" << "disconnected";
#endif
    this->onDisconnected();
}

//    void stateChanged(QAbstractSocket::SocketState state);
void WebSocketQt::textMessageReceived(const QString & message)
{
#ifdef DEBUG_QT
    qDebug() << "SocketQt:" << "received text:" << message;
#endif
    this->onTextReceived(message.toStdString());
}

void WebSocketQt::binaryMessageReceived(const QByteArray & message)
{
#ifdef DEBUG_QT
    qDebug() << "SocketQt:" << "received bin:" << (const char *)message.data();
#endif
    this->onDataReceived(std::vector<unsigned char>(message.data(), message.data() + message.size()));
}

void WebSocketQt::error(QAbstractSocket::SocketError error)
{
#ifdef DEBUG_QT
    qDebug() << "SocketQt:" << "ERROR:" << error;
#endif
    this->onError("TODO: unprocessed qt socket error");
    (void)error;
}

void WebSocketQt::sendData(const std::vector<unsigned char> & data)
{
#ifdef DEBUG_QT
    qDebug() << "SocketQt:" << "send bin:" << (const char*)data.data();
#endif
    _socket->sendBinaryMessage(QByteArray((char*)data.data(), data.size()));
}

void WebSocketQt::sendText(const std::string & text)
{
#ifdef DEBUG_QT
    qDebug() << "SocketQt:" << "send text:" << text.c_str();
#endif
    _socket->sendTextMessage(QString(text.c_str()));
}

void WebSocketQt::connectToServer()
{
#ifdef DEBUG_QT
    qDebug() << "SocketQT:" << "start connect url:" << this->url().c_str();
#endif
    _socket->open(QUrl(this->url().c_str()));
}

void WebSocketQt::disconnectFromServer()
{
    _socket->close();
}

WebSocketQt::WebSocketQt(ClassMethodWrapper<Client, void(Client::*)(Message*), Message> * processMethod) : QObject(), Transport(processMethod),
    _socket(new QWebSocket(QString(""), QWebSocketProtocol::Version13, this))
{
    Q_CHECK_PTR(_socket);

    connect(_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    //    connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    connect(_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(textMessageReceived(QString)));
    connect(_socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(binaryMessageReceived(QByteArray)));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
}

WebSocketQt::~WebSocketQt()
{
    disconnect(_socket, SIGNAL(connected()), this, SLOT(connected()));
    disconnect(_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    //    disconnect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
    disconnect(_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(textMessageReceived(QString)));
    disconnect(_socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(binaryMessageReceived(QByteArray)));
    disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));

    _socket->close();
    delete _socket;
}

}

#endif /* HAVE_SUITABLE_QT_VERSION */

