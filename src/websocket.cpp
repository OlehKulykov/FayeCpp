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

#ifndef SAFE_DELETE
#define SAFE_DELETE(o) if(o){delete o;o=NULL;}
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
	
	void WebSocket::onCallbackReceive(struct libwebsocket * wsi, void * input, size_t len)
	{
		const size_t bytesLeft = libwebsockets_remaining_packet_payload(wsi);
		if (bytesLeft > 0)
		{
			// large fragment
			if (lws_frame_is_binary(wsi)) 
			{
				// binary fragment received
				if (_receivedBinaryBuffer) _receivedBinaryBuffer->append(input, (REUInt32)len);
				else _receivedBinaryBuffer = new REBuffer(input, (REUInt32)len);
			}
			else
			{
				// text fragment received
				if (_receivedTextBuffer) _receivedTextBuffer->append(input, (REUInt32)len);
				else _receivedTextBuffer = new REBuffer(input, (REUInt32)len);
			}
		}
		
		if (libwebsocket_is_final_fragment(wsi)) 
		{
			if (lws_frame_is_binary(wsi)) 
			{
				// final binary
				if (_receivedBinaryBuffer)
				{
					this->onDataReceived((const unsigned char *)_receivedBinaryBuffer->buffer(), _receivedBinaryBuffer->size());
					SAFE_DELETE(_receivedBinaryBuffer)
				}
				else
				{
					this->onDataReceived((const unsigned char *)input, (REUInt32)len);
				}
			}
			else
			{
				// final text
				if (_receivedTextBuffer)
				{
					this->onTextReceived((const char *)_receivedTextBuffer->buffer());
					SAFE_DELETE(_receivedTextBuffer)
				}
				else
				{
					this->onTextReceived((const char *)input);
				}
			}
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
		switch (reason)
		{
			case LWS_CALLBACK_CLIENT_ESTABLISHED:
				socket->onCallbackEstablished();
				break;
				
			case LWS_CALLBACK_CLIENT_RECEIVE:
				if (input && len) socket->onCallbackReceive(wsi, input, len);
				break;
				
			case LWS_CALLBACK_CLIENT_WRITEABLE:
				return socket->onCallbackWritable(context, wsi, static_cast<EchoSessionData *>(user));
				break;
				
			default:
				break;
		}
		return 0;
	}
	
	void WebSocket::onCallbackEstablished()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("CALLBACK CONNECTION ESTABLISHED");
#endif
		this->onConnected();
	}
	
	void WebSocket::onCallbackConnectionError()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("CALLBACK CONNECTION ERROR");
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
		memcpy(&pss->buf[LWS_SEND_BUFFER_PRE_PADDING], buffer->buffer(), buffer->size());
		pss->len = buffer->size();
		
		delete buffer;
		
		this->updateLastSendTime();
		const int writed = libwebsocket_write(connection, &pss->buf[LWS_SEND_BUFFER_PRE_PADDING], pss->len, type);		
		if (writed < 0)
		{
#ifdef FAYECPP_DEBUG_MESSAGES
			RELog::log("ERROR %d writing to socket, hanging up", writed);
#endif
			libwebsocket_callback_on_writable(context, connection);
			return -1;
		}
		if (writed < pss->len)
		{
#ifdef FAYECPP_DEBUG_MESSAGES
			RELog::log("Partial write");
#endif
			libwebsocket_callback_on_writable(context, connection);
			return -1;
		}
		
		if (!_writeBuffers.isEmpty())
		{
			libwebsocket_callback_on_writable(context, connection);
		}
		
		return 0;
	}
	
	void WebSocket::addWriteBufferData(const unsigned char * data, const REUInt32 dataSize, const enum libwebsocket_write_protocol type)
	{
		if (dataSize > MAX_ECHO_PAYLOAD) 
		{
			this->onError(REString::createWithFormat("Error sending %u bytes. Can't send more than %i bytes.", (unsigned int)dataSize, (int)MAX_ECHO_PAYLOAD));
			return;
		}
		
		bool isError = false;
		
		this->writeLock();
		
		WriteBuffer * buffer = new WriteBuffer(data, dataSize);
		if (buffer && buffer->size() == dataSize)
		{
			buffer->tag = (int)type;
#ifdef FAYECPP_DEBUG_MESSAGES
			RELog::log("WILL WRITE %i bytes: %s", (int)buffer->size(), (char *)buffer->buffer());
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
	
	void WebSocket::sendData(const unsigned char * data, const REUInt32 dataSize)
	{
		if (data && dataSize > 0)
		{
			this->addWriteBufferData(data, dataSize, LWS_WRITE_BINARY);
		}
	}
	
	void WebSocket::sendText(const char * text, const REUInt32 textSize)
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
		
		SAFE_DELETE(_receivedTextBuffer)
		SAFE_DELETE(_receivedBinaryBuffer)
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
		info.options = this->isUsingIPV6() ? 0 : LWS_SERVER_OPTION_DISABLE_IPV6;
		
		info.user = static_cast<WebSocket *>(this);
		
		_context = libwebsocket_create_context(&info);
		
		if (!_context)
		{
			this->onError("Socket initialization failed");
			return;
		}
		
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Start connecting to host[%s] port[%i] path[%s]", this->host().UTF8String(), this->port(), this->path().UTF8String());
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
				this->onError(REString("Failed to connect"));
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
	
	WebSocket::WebSocket(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) : REThread(), Transport(processMethod),
		_context(NULL),
		_connection(NULL),
		_writeMutex(new REMutex()),
		_receivedTextBuffer(NULL),
		_receivedBinaryBuffer(NULL)
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
		
		SAFE_DELETE(_writeMutex)
		
		this->cleanup();
	}
	
	REString WebSocket::transportName() 
	{
		return REString("websocket");
	}
}

#endif /* #ifndef HAVE_SUITABLE_QT_VERSION */

