/*
 *   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "FCWebSocketQt.h"

#if defined(HAVE_SUITABLE_QT_VERSION)

#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>

#include "../fayecpp.h"

#if !defined(__FUNCTION__)
#if defined(__func__)
#define __FUNCTION__ __func__
#else
#define NO__FUNCTION__
#endif
#endif

#ifndef ERROR_LINE_INFO_STRING
#ifdef NO__FUNCTION__
#define ERROR_LINE_INFO_STRING REString::createWithFormat("File: %s, line: %i", __FILE__, (int)__LINE__)
#else
#define ERROR_LINE_INFO_STRING REString::createWithFormat("File: %s, function: %s, line: %i", __FILE__, __FUNCTION__, (int)__LINE__)
#endif
#endif

namespace FayeCpp {

    void WebSocketQt::timerEvent(QTimerEvent * event)
    {
        if (event->timerId() == _tickTimer.timerId())
        {
            this->tick();
        }
        else
        {
            QObject::timerEvent(event);
        }
    }

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
		const QString err(_socket->errorString());
		if (err.isEmpty()) qDebug() << "SocketQt:" << "disconnected without error";
		else qDebug() << "SocketQt:" << "disconnected with error:" << err;
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
        REVariantMap info;
        info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
		const QString err(_socket->errorString());
		if (err.isEmpty())
		{
            info[kErrorLocalizedDescriptionKey] = "Unprocessed qt socket error";
		}
		else
		{
            info[kErrorLocalizedDescriptionKey] = err.toUtf8().data();
		}
        this->onError(Error(kErrorDomainTransport, Error::InternalApplicationError, info));
		(void)error;
	}

	void WebSocketQt::sendData(const unsigned char * data, const REUInt32 dataSize)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "send bin:" << (const char*)data;
#endif
		_socket->sendBinaryMessage(QByteArray((char*)data, (int)dataSize));
	}

	void WebSocketQt::sendText(const char * text, const REUInt32 textSize)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQt:" << "send text:" << text;
#endif
		_socket->sendTextMessage(QString(text));
		(void)textSize;
	}

	QString WebSocketQt::toString(const REString & s)
	{
		return s.isNotEmpty() ? QString(s.UTF8String()) : QString("");
	}

	void WebSocketQt::setupSocketWithSSLDataSource(SSLDataSource * dataSource)
	{
		QSslConfiguration config;

		QFile localFile(WebSocketQt::toString(dataSource->clientLocalCertificateFilePath()));
		if (localFile.open(QIODevice::ReadOnly))
		{
			QSslCertificate cert(localFile.readAll());
			localFile.close();
			if (cert.isNull())
			{
#ifdef FAYECPP_DEBUG_MESSAGES
				qDebug() << "SocketQT: LocalCertificate is NULL";
#endif
			}
			else
			{
				config.setLocalCertificate(cert);
			}
		}

		QFile keyFile(WebSocketQt::toString(dataSource->clientPrivateKeyFilePath()));
		if (keyFile.open(QIODevice::ReadOnly))
		{
			QByteArray pp;
			pp.append(WebSocketQt::toString(dataSource->clientPrivateKeyPassPhrase()));
			QSslKey key(keyFile.readAll(),
						QSsl::Rsa,
						QSsl::Pem,
						QSsl::PrivateKey,
						pp);
			pp.clear();
			keyFile.close();
			if (key.isNull())
			{
#ifdef FAYECPP_DEBUG_MESSAGES
				qDebug() << "SocketQT: PrivateKey is NULL";
#endif
			}
			else
			{
				config.setPrivateKey(key);
			}
		}

		QFile caFile(WebSocketQt::toString(dataSource->clientCACertificateFilePath()));
		if (caFile.open(QIODevice::ReadOnly))
		{
			QSslCertificate cert(caFile.readAll());
			caFile.close();
			if (cert.isNull())
			{
#ifdef FAYECPP_DEBUG_MESSAGES
				qDebug() << "SocketQT: CACertificate is NULL";
#endif
			}
			else
			{
				QList<QSslCertificate> caList(config.caCertificates());
				caList.append(cert);
				config.setCaCertificates(caList);
			}
		}

		_socket->setSslConfiguration(config);
	}

	void WebSocketQt::connectToServer()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		qDebug() << "SocketQT:" << "start connect url:" << this->client()->url().UTF8String();
#endif

		SSLDataSource * dataSource = this->sslDataSource();
		if (dataSource)
		{
			this->setupSocketWithSSLDataSource(dataSource);
		}

		_socket->open(QUrl(this->client()->url().UTF8String()));
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

        _tickTimer.start(200, this);
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

