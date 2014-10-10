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
	
	void Transport::receivedAdvice(const VariantMap & advice)
	{
		Advice a;
		a.reconnect = ADVICE_RECONNECT_NONE;
		a.timeout = a.interval = -1;
		
		VariantMap::Iterator i = advice.iterator();
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
		
		Variant * thisTransportAdvice = advice.findTypedValue(this->name(), Variant::TypeMap);
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
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("TRANSPORT CONNECTED");
#endif		
		_isConnected = true;
		
		Responce message; message.setType(Responce::ResponceTransportConnected);
		if (_processMethod) _processMethod->invokeWithPointer(&message);	
	}
	
	void Transport::onDisconnected()
	{
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("TRANSPORT DISCONNECTED");
#endif		
		_isConnected = false;
		
		Responce message; message.setType(Responce::ResponceTransportDisconnected);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onTextReceived(const char * text)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("TRANSPORT RECEIVED: %s", text);
#endif
		
		Responce message; message.setMessageText(text).setType(Responce::ResponceMessage);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onDataReceived(const unsigned char * data, const size_t dataSize)
	{
		Responce message; message.setMessageData(data, dataSize).setType(Responce::ResponceMessage);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onError(const REString & error)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("TRANSPORT ERROR: %s", error.UTF8String());
#endif
		
		Responce message; message.setType(Responce::ResponceTransportError).setErrorString(error);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onError(const char * error)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("TRANSPORT ERROR: %s", error);
#endif
		
		Responce message; message.setType(Responce::ResponceTransportError).setErrorString(error);
		if (_processMethod) _processMethod->invokeWithPointer(&message);
	}
	
	bool Transport::isConnected() const
	{
		return _isConnected;
	}
	
	Transport::Transport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) :
		_processMethod(processMethod),
		_isConnected(false)
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
			///
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
