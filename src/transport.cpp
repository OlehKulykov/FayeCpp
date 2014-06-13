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

#if defined(HAVE_SUITABLE_QT_VERSION) && defined(FAYECPP_DEBUG_MESSAGES)
#include <QDebug>
#endif

namespace FayeCpp {
	
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
#ifdef HAVE_SUITABLE_QT_VERSION
		qDebug() << "TRANSPORT RECEIVED:" << text;
#else		
		fprintf(stderr, "TRANSPORT RECEIVED: %s\n", text);
#endif		
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
#ifdef HAVE_SUITABLE_QT_VERSION
		qDebug() << "TRANSPORT ERROR:" << error.UTF8String();
#else		
		fprintf(stderr, "TRANSPORT ERROR: %s\n", error.UTF8String());
#endif		
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
		_port(-1),
		_isUseSSL(false),
		_isConnected(false)
	{
		
	}
	
	Transport::Transport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) :
		_processMethod(processMethod),
		_port(-1),
		_isUseSSL(false),
		_isConnected(false)
	{
		assert(_processMethod);
		assert(this->client());
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
