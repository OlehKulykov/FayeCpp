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
#include "REThread.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#ifdef HAVE_SUITABLE_QT_VERSION
#include "websocketqt.h"
#else
#include "websocket.h"
#endif

#include <assert.h>

namespace FayeCpp {
		
	void Transport::updateLastSendTime()
	{
		_lastSendTime = RETime::time();
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
	
	Client * Transport::client()
	{
		return _processMethod->classPointer();
	}
	
	Delegate * Transport::delegate()
	{
		return this->client()->delegate();
	}
	
	void Transport::onConnected()
	{
		_isConnected = true;
		Responce message;
        message.setType(Responce::ResponceTransportConnected);
		_processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onDisconnected()
	{
		_isConnected = false;
		Responce message;
        message.setType(Responce::ResponceTransportDisconnected);
		_processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onTextReceived(const char * text)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("TRANSPORT RECEIVED: %s", text);
#endif
		Responce message;
        message.setMessageText(text).setType(Responce::ResponceMessage);
		_processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onDataReceived(const unsigned char * data, const size_t dataSize)
	{
		Responce message;
        message.setMessageData(data, dataSize).setType(Responce::ResponceMessage);
		_processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onError(const REString & error)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("TRANSPORT ERROR: %s", error.UTF8String());
#endif
		Responce message;
        message.setType(Responce::ResponceTransportError).setErrorString(error.UTF8String());
		_processMethod->invokeWithPointer(&message);
	}
	
	const REString & Transport::url() const
	{
		return _url;
	}
	
	bool Transport::isConnected() const
	{
		return _isConnected;
	}
	
	void Transport::setUrl(const char * url)
	{
		_url = url;
		REMutableString urlString(url);
		if (urlString.isContaines("ws://"))
		{
			urlString.replace("ws://");
			_isUseSSL = false;
		}
		
		if (urlString.isContaines("wss://"))
		{
			urlString.replace("wss://");
			_isUseSSL = true;
		}
		
		if (urlString.isContaines("http://"))
		{
			urlString.replace("http://");
#if defined(HAVE_SUITABLE_QT_VERSION)
			REMutableString u(url);
			u.replace("http://", "ws://");
			_url = u.UTF8String();
#endif
			_isUseSSL = false;
		}
		
		if (urlString.isContaines("https://"))
		{
			urlString.replace("https://");
#if defined(HAVE_SUITABLE_QT_VERSION)
			REMutableString u(url);
			u.replace("https://", "wss://");
			_url = u.UTF8String();
#endif
			_isUseSSL = true;
		}
		
		const char * sub = strstr(urlString.UTF8String(), ":");
		if (sub)
		{
			int port = -1;
			if (sscanf(++sub, "%i", &port) == 1)
			{
				_port = port;
			}
		}
		
		sub = strstr(urlString.UTF8String(), "/");
		if (sub) 
		{
			_path = sub;
		}
		else 
		{
			_path = "/";
		}
	
		sub = strstr(urlString.UTF8String(), ":");
		if (!sub) sub = strstr(urlString.UTF8String(), "/");
		if (sub) 
		{
			const REUInt32 len = (REUInt32)(sub - urlString.UTF8String());
			_host = REString(urlString.UTF8String(), len);
		}
		else
		{
			_host = urlString.UTF8String();
		}
	}
	
	const REString & Transport::host() const
	{
		return _host;
	}
	
	const REString & Transport::path() const
	{
		return _path;
	}
	
	int Transport::port() const
	{
		return _port;
	}
	
	bool Transport::isUseSSL() const
	{
		return _isUseSSL;
	}
	
	Transport::Transport() :
		_processMethod(NULL),
		_lastSendTime(RETime::time()),
		_port(-1),
		_isUseSSL(false),
		_isConnected(false)
	{
		_advice.reconnect = ADVICE_RECONNECT_NONE;
		_advice.timeout = _advice.interval = -1;
	}
	
	Transport::Transport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) :
		_processMethod(processMethod),
		_lastSendTime(RETime::time()),
		_port(-1),
		_isUseSSL(false),
		_isConnected(false)
	{
		assert(_processMethod);
		assert(this->client());
		
		_advice.reconnect = ADVICE_RECONNECT_NONE;
		_advice.timeout = _advice.interval = -1;
	}
	
	Transport::~Transport()
	{
        delete _processMethod;
	}

    Transport * Transport::createNewTransport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod)
    {
        if (processMethod)
        {
#ifdef HAVE_SUITABLE_QT_VERSION
		return new WebSocketQt(processMethod);
#elif defined(HAVE_LIBWEBSOCKETS_H)
        return new WebSocket(processMethod);
#endif
        }
        return NULL;
    }

    REStringList Transport::availableConnectionTypes()
    {
        REStringList types;

#ifdef HAVE_SUITABLE_QT_VERSION
		types.add(WebSocketQt::transportName());
#elif defined(HAVE_LIBWEBSOCKETS_H)
		types.add(WebSocket::transportName());
#endif
        return types;
    }
}
