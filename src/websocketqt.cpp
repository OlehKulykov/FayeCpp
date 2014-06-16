
#include "websocketqt.h"

#if defined(HAVE_SUITABLE_QT_VERSION)

#include "../fayecpp.h"

namespace FayeCpp {

	void WebSocketQt::connected()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "connected";
#endif
		this->onConnected();
	}

	void WebSocketQt::disconnected()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "disconnected";
#endif
		this->onDisconnected();
	}

	void WebSocketQt::textMessageReceived(const QString & message)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "received text:" << message;
#endif
		QByteArray bytes(message.toUtf8());
		if (bytes.size() > 0)
		{
			this->onTextReceived((const char *)bytes.data());
		}
	}

	void WebSocketQt::binaryMessageReceived(const QByteArray & message)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "received bin:" << (const char *)message.data();
#endif
		if (message.size() > 0)
		{
			this->onDataReceived((const unsigned char *)message.data(), (REUInt32)message.size());
		}
	}

	void WebSocketQt::error(QAbstractSocket::SocketError error)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "ERROR:" << error;
#endif
		const QString err(_socket->errorString());
		if (err.isEmpty())
		{
			this->onError(REString("TODO: unprocessed qt socket error"));
		}
		else
		{
			this->onError(REString(err.toUtf8().data()));
		}
		(void)error;
	}

	void WebSocketQt::sendData(const unsigned char * data, const size_t dataSize)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "send bin:" << (const char*)data;
#endif
		_socket->sendBinaryMessage(QByteArray((char*)data, (int)dataSize));
	}

	void WebSocketQt::sendText(const char * text, const size_t textSize)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "send text:" << text;
#endif
		_socket->sendTextMessage(QString(text));
		(void)textSize;
	}

	void WebSocketQt::connectToServer()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQT:" << "start connect url:" << this->url().UTF8String();
#endif
		_socket->open(QUrl(this->url().UTF8String()));
	}

	void WebSocketQt::disconnectFromServer()
	{
		_socket->close();
	}

	const REString WebSocketQt::name() const
	{
		return WebSocketQt::transportName();
	}
	
	WebSocketQt::WebSocketQt(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) : QObject(), Transport(processMethod),
		_socket(new QWebSocket(QString(""), QWebSocketProtocol::Version13, this))
	{
		Q_CHECK_PTR(_socket);

		connect(_socket, SIGNAL(connected()), this, SLOT(connected()));
		connect(_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
		connect(_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(textMessageReceived(QString)));
		connect(_socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(binaryMessageReceived(QByteArray)));
		connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
	}

	WebSocketQt::~WebSocketQt()
	{
		disconnect(_socket, SIGNAL(connected()), this, SLOT(connected()));
		disconnect(_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
		disconnect(_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(textMessageReceived(QString)));
		disconnect(_socket, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(binaryMessageReceived(QByteArray)));
		disconnect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));

		_socket->close();
		delete _socket;
	}

	REString WebSocketQt::transportName()
	{
		return REString("websocket");
	}
	
}

#endif /* HAVE_SUITABLE_QT_VERSION */

