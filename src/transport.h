/*
 *   Copyright (c) 2014 Kulykov Oleh <nonamedemail@gmail.com>
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


#ifndef __FAYECPP_TRANSPORT_H__
#define __FAYECPP_TRANSPORT_H__

#include "../fayecpp.h"
#include "classmethodwrapper.h"
#include "REMutex.h"

namespace FayeCpp {
	
#define ADVICE_RECONNECT_NONE 0
#define ADVICE_RECONNECT_RETRY 1
#define ADVICE_RECONNECT_HANDSHAKE 2
	
	class Transport
	{
	public:
		typedef struct _adviceStructure
		{
			RETimeInterval interval;
			RETimeInterval timeout;
			int reconnect;
		} Advice;
	private:
		REString _url;
		REString _host;
		REString _path;
		ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * _processMethod;
		Advice _advice;
		RETimeInterval _lastSendTime;
		int _port;
		bool _isUseSSL;
		bool _isConnected;
		
		Transport();
		
		Client * client();
		Delegate * delegate();
		
	protected:
		RETimeInterval adviceInterval() const { return _advice.interval; }
		RETimeInterval adviceTimeout() const { return _advice.timeout; }
		int adviceReconnect() const { return _advice.reconnect; }
		
		
		void onConnected();
		void onDisconnected();
		void onTextReceived(const char * text);
		void onDataReceived(const unsigned char * data, const size_t dataSize);
		void onError(const REString & error);
		
		void onSended();
		
	public:
		RETimeInterval lastSendTime() const { return _lastSendTime; }
		void receivedAdvice(const VariantMap & advice);
		bool isConnected() const;
		void setUrl(const char * url);
		
		const REString & url() const;
		const REString & host() const;
		
		/**
		 @brief Should start with '/'
		 */
		const REString & path() const;
		int port() const;
		bool isUseSSL() const;
		
		virtual const REString name() const = 0;

		virtual void sendData(const unsigned char * data, const size_t dataSize) = 0;
		virtual void sendText(const char * text, const size_t textSize) = 0;

		virtual void connectToServer() = 0;
		virtual void disconnectFromServer() = 0;
		
		Transport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod);
		
		virtual ~Transport();

        static Transport * createNewTransport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod);
        static REStringList availableConnectionTypes();
	};
	
}

#endif /* __FAYECPP_TRANSPORT_H__ */
