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


#include "transport.h"

#ifdef HAVE_SUITABLE_QT_VERSION
#include "websocketqt.h"
#else
#include "websocket.h"
#endif

namespace FayeCpp {
	
	bool Transport::isUsingIPV6() const
	{
		Client * client = this->client();
		return client ? client->isUsingIPV6() : false;
	}
	
	RETimeInterval Transport::adviceInterval() const 
	{
		return _advice.interval; 
	}
	
	RETimeInterval Transport::adviceTimeout() const 
	{
		return _advice.timeout; 
	}
	
	int Transport::adviceReconnect() const 
	{
		return _advice.reconnect; 
	}
	
	void Transport::receivedAdvice(const REVariantMap & advice)
	{
		Advice a;
		a.reconnect = ADVICE_RECONNECT_NONE;
		a.timeout = a.interval = -1;
		
		REVariantMap::Iterator i = advice.iterator();
		while (i.next())
		{
			if (i.key().isEqual("reconnect") && i.value().isString())
			{
				if (i.value().toString().isEqual("retry"))
					a.reconnect = ADVICE_RECONNECT_RETRY;
				else if (i.value().toString().isEqual("handshake"))
					a.reconnect = ADVICE_RECONNECT_HANDSHAKE;
			}
			else if (i.key().isEqual("interval") && i.value().isNumber())
				a.interval = i.value().toInt64() / 1000;
			else if (i.key().isEqual("timeout") && i.value().isNumber())
				a.timeout = i.value().toInt64() / 1000;
		}
		
		_advice = a;
		
		REVariant * thisTransportAdvice = advice.findTypedValue(this->name(), REVariant::TypeMap);
		if (thisTransportAdvice) this->receivedAdvice(thisTransportAdvice->toMap());
	}
	
	Client * Transport::client() const
	{
		return _processMethod ? _processMethod->classPointer() : NULL;
	}
	
	Delegate * Transport::delegate() const
	{
		Client * client = this->client();
		return client ? client->delegate() : NULL;
	}
	
	SSLDataSource * Transport::sslDataSource() const
	{
		Client * client = this->client();
		return client ? client->sslDataSource() : NULL;
	}
	
	void Transport::onConnected()
	{
		FAYECPP_DEBUG_LOG("TRANSPORT CONNECTED")

		_isConnected = true;
		
		Responce message(Responce::ResponceTransportConnected);
		if (_processMethod) _processMethod->invokeWithPointer(&message);	
	}
	
	void Transport::onDisconnected()
	{
		FAYECPP_DEBUG_LOG("TRANSPORT DISCONNECTED")

		_isConnected = false;
		
		Responce message(Responce::ResponceTransportDisconnected);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onTextReceived(const char * text)
	{
		FAYECPP_DEBUG_LOGA("TRANSPORT RECEIVED: %s", text)
		
		Responce message(Responce::ResponceMessage); message.setMessageText(text);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onDataReceived(const unsigned char * data, const size_t dataSize)
	{
		Responce message(Responce::ResponceMessage); message.setMessageData(data, dataSize);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onError(const Error & error)
	{
		FAYECPP_DEBUG_LOG("TRANSPORT ERROR")
		
		Responce message(Responce::ResponceTransportError); message.setError(error);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}

	void Transport::onTransportWillSelfDestruct()
	{
		FAYECPP_DEBUG_LOG("TRANSPORT WILL SELF DESTRUCT")

		_isSelfDestructing = true;

		Responce message(Responce::ResponceTransportWillSelfDestruct);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}

	bool Transport::isConnected() const
	{
		return _isConnected;
	}

	bool Transport::isSelfDestructing() const
	{
		return _isSelfDestructing;
	}

	Transport::Transport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) :
		_processMethod(processMethod),
		_isConnected(false),
		_isSelfDestructing(false)
	{
#if defined(HAVE_ASSERT_H) 
		assert(_processMethod);
		assert(this->client());
#endif		
		_advice.reconnect = ADVICE_RECONNECT_NONE;
		_advice.timeout = _advice.interval = -1;
	}
	
	Transport::~Transport()
	{
		SAFE_DELETE(_processMethod)
	}
	
	Transport * Transport::createNewTransport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod)
	{
		if (processMethod)
		{
#if defined(HAVE_SUITABLE_QT_VERSION)
			return new WebSocketQt(processMethod);
#elif defined(HAVE_LIBWEBSOCKETS_H)
			return new WebSocket(processMethod);
#endif
		}
		return NULL;
	}
	
	void Transport::deleteTransport(Transport * transport)
	{
		if (transport) 
		{
			ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod = transport->_processMethod;
			transport->_processMethod = NULL;
			if (processMethod) delete processMethod;
			
			transport->disconnectFromServer();
			
#if defined(HAVE_SUITABLE_QT_VERSION)
			delete transport;
#elif defined(HAVE_LIBWEBSOCKETS_H)
			/// will self destruct
#endif	
		}
	}
	
	REStringList Transport::availableConnectionTypes()
	{
		REStringList types;
		
#if defined(HAVE_SUITABLE_QT_VERSION)
		types.add(WebSocketQt::transportName());
#elif defined(HAVE_LIBWEBSOCKETS_H)
		types.add(WebSocket::transportName());
#endif
		return types;
	}
	
	bool Transport::isSupportsIPV6()
	{
		bool isSupported = false;
#if defined(HAVE_SUITABLE_QT_VERSION)
		
#elif defined(HAVE_LIBWEBSOCKETS_H)
		
#if defined(LWS_USE_IPV6)	
		isSupported = true;
#endif		
		
#endif
		return isSupported;
	}
	
	bool Transport::isSupportsSSLConnection()
	{
		bool isSupported = false;
#if defined(HAVE_SUITABLE_QT_VERSION)
		isSupported = true;
#elif defined(HAVE_LIBWEBSOCKETS_H)
		
#if defined(LWS_OPENSSL_SUPPORT)	
		isSupported = true;
#endif		
		
#endif
		return isSupported;
	}
	
}

