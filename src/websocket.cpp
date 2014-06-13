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


#include "websocket.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if !defined(HAVE_SUITABLE_QT_VERSION) && defined(HAVE_LIBWEBSOCKETS_H)

#include "REThread.h"
#include "REMutex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#endif

namespace FayeCpp {
	
	struct libwebsocket_protocols WebSocket::protocols[] = {
		{
			"default",
			WebSocket::callbackEcho,
			sizeof(EchoSessionData)
		},
		{
			NULL, NULL, 0
		}
	};
	
	void WebSocket::writeLock()
	{
		if (_writeMutex) _writeMutex->lock();
	}
	
	void WebSocket::writeUnlock()
	{
		if (_writeMutex) _writeMutex->unlock();
	}
	
	void WebSocket::onCallbackReceive(void * input, size_t len)
	{
		char * str = (char *)input;
		if (strlen(str) == len)
		{
			this->onTextReceived(str);
		}
		else
		{
			this->onDataReceived((const unsigned char *)input, (REUInt32)len);
		}
	}
	
	int WebSocket::callbackEcho(struct libwebsocket_context * context,
								struct libwebsocket * wsi,
								enum libwebsocket_callback_reasons reason,
								void * user,
								void * input,
								size_t len)
	{
		WebSocket * socket = static_cast<WebSocket *>(libwebsocket_context_user(context));
		EchoSessionData * pss = static_cast<EchoSessionData *>(user);
		switch (reason)
		{
			case LWS_CALLBACK_CLIENT_ESTABLISHED:
				socket->onCallbackEstablished();
				break;
				
			case LWS_CALLBACK_CLIENT_RECEIVE:
				if (input && len) socket->onCallbackReceive(input, len);
				break;
				
			case LWS_CALLBACK_CLIENT_WRITEABLE:
				return socket->onCallbackWritable(context, wsi, pss);
				break;
				
			default:
				break;
		}
		return 0;
	}
	
	void WebSocket::onCallbackEstablished()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		fprintf(stderr, "CALLBACK CONNECTION ESTABLISHED\n");
#endif
		this->onConnected();
	}
	
	void WebSocket::onCallbackConnectionError()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		fprintf(stderr, "CALLBACK CONNECTION ERROR\n");
#endif
		//TODO: error string
		this->onError("");
	}
	
	int WebSocket::onCallbackWritable(struct libwebsocket_context * context,
									  struct libwebsocket * connection,
									  EchoSessionData * pss)
	{
		WriteBuffer * buffer = NULL;
		
		this->writeLock();
		REList<WriteBuffer *>::Iterator i = _writeBuffers.iterator();
		while (!buffer && i.next()) 
		{
			buffer = i.value();
			_writeBuffers.removeNode(i.node());
		}
		this->writeUnlock();
		
		if (!buffer) return 0;
		
		const enum libwebsocket_write_protocol type = (enum libwebsocket_write_protocol)buffer->tag;
		
//		pss->len = sprintf((char *)&pss->buf[LWS_SEND_BUFFER_PRE_PADDING],
//						   "%s",
//						   (const char*)buffer->buffer());
		memcpy(&pss->buf[LWS_SEND_BUFFER_PRE_PADDING], buffer->buffer(), buffer->size());
		pss->len = buffer->size();
		
		delete buffer;
		
		const int writed = libwebsocket_write(connection, &pss->buf[LWS_SEND_BUFFER_PRE_PADDING], pss->len, type);
		if (writed < 0)
		{
#ifdef FAYECPP_DEBUG_MESSAGES
			fprintf(stderr, "ERROR %d writing to socket, hanging up\n", writed);
#endif
			return -1;
		}
		if (writed < pss->len)
		{
#ifdef FAYECPP_DEBUG_MESSAGES
			fprintf(stderr, "Partial write\n");
#endif
			return -1;
		}
		
		if (!_writeBuffers.isEmpty())
		{
			libwebsocket_callback_on_writable(context, connection);
		}
		
		return 0;
	}
	
	void WebSocket::addWriteBufferData(const unsigned char * data, const size_t dataSize, const enum libwebsocket_write_protocol type)
	{
		bool isError = false;
		
		this->writeLock();
		
		WriteBuffer * buffer = new WriteBuffer(data, (REUInt32)dataSize);
		if (buffer && buffer->size() == dataSize)
		{
			buffer->tag = (int)type;
#ifdef FAYECPP_DEBUG_MESSAGES
			fprintf(stdout, "WILL WRITE %i bytes: %s\n", (int)buffer->size(), (char *)buffer->buffer());
#endif
			_writeBuffers.add(buffer);
		}
		else
		{
			isError = true;
		}
		
		this->writeUnlock();
		
		if (isError)
		{
			this->onError("Can't send buffer data");
		}
		else if (this->isConnected())
		{
			libwebsocket_callback_on_writable(_context, _connection);
		}
	}
	
	void WebSocket::sendData(const unsigned char * data, const size_t dataSize)
	{
		if (data && dataSize > 0)
		{
			this->addWriteBufferData(data, dataSize, LWS_WRITE_BINARY);
		}
	}
	
	void WebSocket::sendText(const char * text, const size_t textSize)
	{
		if (text && textSize > 0)
		{
			// NO NULL terminated char
			this->addWriteBufferData((const unsigned char *)text, textSize, LWS_WRITE_TEXT);
		}
	}
	
	const REString WebSocket::name() const 
	{
		return WebSocket::transportName(); 
	}
	
	void WebSocket::connectToServer()
	{
		if (!this->isConnected())
		{
			this->start();
		}
	}
	
	void WebSocket::cleanup()
	{
		memset(&_info, 0, sizeof(struct lws_context_creation_info));
		
		if (_context)
		{
			libwebsocket_context_destroy(_context);
			_context = NULL;
		}
		_connection = NULL;
		
		REList<WriteBuffer *>::Iterator i = _writeBuffers.iterator();
		while (i.next()) 
		{
			WriteBuffer * b = i.value();
			delete b;
		}
		_writeBuffers.clear();
	}
	
	void WebSocket::disconnectFromServer()
	{
		this->cancel();
		this->cleanup();
	}
	
	void WebSocket::threadBody()
	{
		struct lws_context_creation_info info;
		memset(&info, 0, sizeof(struct lws_context_creation_info));
		
		info.port = CONTEXT_PORT_NO_LISTEN;
		info.iface = NULL;
		info.protocols = WebSocket::protocols;
#ifndef LWS_NO_EXTENSIONS
		info.extensions = libwebsocket_get_internal_extensions();
#endif
		
		info.gid = -1;
		info.uid = -1;
		info.options = 0;
		info.user = static_cast<WebSocket *>(this);
		
		_context = libwebsocket_create_context(&info);
		
		if (!_context)
		{
			this->onError("Socket initialization failed");
			return;
		}
		
#ifdef FAYECPP_DEBUG_MESSAGES
		fprintf(stdout, "Start connecting to host[%s] port[%i] path[%s]\n", this->host().UTF8String(), this->port(), this->path().UTF8String());
#endif
		
		_connection = libwebsocket_client_connect(_context,
												  this->host().UTF8String(),
												  this->port(),
												  this->isUseSSL() ? 2 : 0,
												  this->path().UTF8String(),
												  this->host().UTF8String(),
												  "origin",
												  NULL,
												  -1);
		if (!_connection)
		{
			if (_context) libwebsocket_context_destroy(_context);
			_context = NULL;
			
			char error[1024] = { 0 };
#if defined(HAVE_FUNCTION_SPRINTF_S)	
			const int writed = sprintf_s(error, 1024, "Failed to connect to %s:%i", this->host().UTF8String(), this->port());
#else			
			const int writed = sprintf(error, "Failed to connect to %s:%i", this->host().UTF8String(), this->port());
#endif			
			if (writed > 0) 
			{
				this->onError(REString(error, (REUInt32)writed));
			}
			else
			{
				this->onError(REString("Failed to connect "));
			}
			return;
		}
		
		libwebsocket_callback_on_writable(_context, _connection);
		
		int n = 0;
		while (n >= 0 && _context /* && !force_exit */ )
		{
			n = _context ? libwebsocket_service(_context, 10) : -1;
			REThread::uSleep(10);
		}
		
		if (_context) libwebsocket_context_destroy(_context);
		_context = NULL;
	}
	
	WebSocket::WebSocket(ClassMethodWrapper<Client, void(Client::*)(Message*), Message> * processMethod) : REThread(), Transport(processMethod),
		_context(NULL),
		_connection(NULL),
		_writeMutex(new REMutex())
	{
		memset(&_info, 0, sizeof(struct lws_context_creation_info));

#if defined(HAVE_ASSERT_H)		
		assert(_writeMutex);
#endif
		
        _writeMutex->init(REMutex::REMutexTypeRecursive);
		
		REThread::isMainThread();
	}
	
	WebSocket::~WebSocket()
	{
		this->cancel();
		
		if (_writeMutex)
		{
			delete _writeMutex;
			_writeMutex = NULL;
		}
		
		this->cleanup();
	}
	
	REString WebSocket::transportName() 
	{
		return REString("websocket");
	}
}

#endif /* #ifndef HAVE_SUITABLE_QT_VERSION */

