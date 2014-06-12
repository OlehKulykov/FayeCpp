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
#include "../fayecpp.h"
#include "REThread.h"

#ifdef HAVE_SUITABLE_QT_VERSION
#include "private/websocketqt.h"
#else
#include "private/websocket.h"
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
		Message message;
        message.setType(Message::MessageTypeTransportConnected).setSuccessfully(true);
		_processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onDisconnected()
	{
		_isConnected = false;
		Message message;
        message.setType(Message::MessageTypeTransportDisconnected).setSuccessfully(true);
		_processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onTextReceived(const std::string & text)
	{
		Message message(text);
        message.setType(Message::MessageTypeServerResponce);
		_processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onDataReceived(const std::vector<unsigned char> & data)
	{
		Message message(data);
        message.setType(Message::MessageTypeServerResponce);
		_processMethod->invokeWithPointer(&message);
	}
	
	void Transport::onError(const std::string & error)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
#ifdef HAVE_SUITABLE_QT_VERSION
		qDebug() << "TRANSPORT ERROR:" << error.c_str();
#else		
		fprintf(stderr, "TRANSPORT ERROR: %s\n", error.c_str());
#endif		
#endif
		Message message;
        message.setType(Message::MessageTypeTransportError).setSuccessfully(true).setErrorString(error);
		_processMethod->invokeWithPointer(&message);
	}
	
	const std::string & Transport::url() const
	{
		return _url;
	}
	
	bool Transport::isConnected() const
	{
		return _isConnected;
	}
	
	void Transport::setUrl(const std::string & url)
	{
		_url = url;
		std::string urlString = url;
		if (urlString.find("ws://") == 0)
		{
			urlString.erase(0, 5);
			_isUseSSL = false;
		}
		
		if (urlString.find("wss://") == 0)
		{
			urlString.erase(0, 6);
			_isUseSSL = true;
		}
		
		if (urlString.find("http://") == 0)
		{
			urlString.erase(0, 7);
#if defined(HAVE_SUITABLE_QT_VERSION)
			_url.replace(0, 7, "ws://");
#endif
			_isUseSSL = false;
		}
		
		if (urlString.find("https://") == 0)
		{
			urlString.erase(0, 8);
#if defined(HAVE_SUITABLE_QT_VERSION)
			_url.replace(0, 8, "wss://");
#endif
			_isUseSSL = true;
		}
		
		size_t pos = urlString.find(":");
		if (pos > 0)
		{
			_port = atoi(urlString.substr(pos + 1, urlString.size()).c_str());
		}
		
		pos = urlString.find("/", pos);
		_path = "/";
		if(pos > 0)
		{
			_path += urlString.substr(pos + 1, urlString.size());
		}
		
		pos = urlString.find(":");
		if(pos > 0)
		{
			urlString.erase(pos, urlString.size());
		}
		_host = urlString;
	}
	
	const std::string & Transport::host() const
	{
		return _host;
	}
	
	const std::string & Transport::path() const
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
	
	Transport::Transport(ClassMethodWrapper<Client, void(Client::*)(Message*), Message> * processMethod) :
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

    Transport * Transport::createNewTransport(ClassMethodWrapper<Client, void(Client::*)(Message*), Message> * processMethod)
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

    std::list<std::string> Transport::availableConnectionTypes()
    {
        std::list<std::string> types;

#ifdef HAVE_SUITABLE_QT_VERSION
        types.push_back(WebSocketQt::transportName());
#elif defined(HAVE_LIBWEBSOCKETS_H)
        types.push_back(WebSocket::transportName());
#endif
        return types;
    }
}
