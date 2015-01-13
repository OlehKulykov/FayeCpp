/*
 *   Copyright (c) 2014 - 2015 Kulykov Oleh <nonamedemail@gmail.com>
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


#ifndef __FAYECPP_WEBSOCKETQT_H__
#define __FAYECPP_WEBSOCKETQT_H__

#include "../fayecpp.h"

#if defined(HAVE_SUITABLE_QT_VERSION)

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

		static QString toString(const REString & s);
		void setupSocketWithSSLDataSource(SSLDataSource * dataSource);

	private slots:
		void connected();
		void disconnected();
		void textMessageReceived(const QString & message);
		void binaryMessageReceived(const QByteArray & message);
		void error(QAbstractSocket::SocketError error);

	public:
		virtual const REString name() const;

		virtual void sendData(const unsigned char * data, const REUInt32 dataSize);
		virtual void sendText(const char * text, const REUInt32 textSize);

		virtual void connectToServer();
		virtual void disconnectFromServer();
		WebSocketQt(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod);
		virtual ~WebSocketQt();

		static REString transportName();
	};

}


#endif /* HAVE_SUITABLE_QT_VERSION */

#endif /* __FAYECPP_WEBSOCKETQT_H__ */
