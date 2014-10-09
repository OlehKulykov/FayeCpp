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

#if !defined(HAVE_SUITABLE_QT_VERSION) && defined(HAVE_LIBWEBSOCKETS_H)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SAFE_DELETE
#define SAFE_DELETE(o) if(o){delete o;o=NULL;}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(m) if(m){free((void *)m);m=NULL;}
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
				
			case LWS_CALLBACK_WSI_DESTROY:
				socket->onCallbackConnectionDestroyed();
				break;
								
			default:
				break;
		}
		return 0;
	}
	
	void WebSocket::onCallbackConnectionDestroyed()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("CALLBACK CONNECTION DESTROYED");
#endif	
		
		_isWorking = 0;
		
		this->cleanup();
		
		this->onDisconnected();
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
		REList<WriteBuffer *>::Iterator i = _writeBuffers.iterator();
		while (!buffer && i.next()) 
		{
			buffer = i.value();
			_writeBuffers.removeNode(i.node());
		}
		
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
		this->lockMutex();
		
		WriteBuffer * buffer = new WriteBuffer(data, dataSize);
		if (buffer && buffer->size() == dataSize)
		{
			buffer->tag = (int)type;
#ifdef FAYECPP_DEBUG_MESSAGES
			RELog::log("WILL WRITE %i bytes: %s", (int)buffer->size(), (char *)buffer->buffer());
#endif
			_writeBuffers.add(buffer);
		}
		else isError = true;
		
		if (!isError && this->isConnected())
		{
			libwebsocket_callback_on_writable(_context, _connection);
		}
		
		this->unLockMutex();
		
		if (isError) this->onError("Can't send buffer data");
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
	
#if defined(HAVE_PTHREAD_H)		
	void * WebSocket::workThreadFunc(void * somePointer)
	{
		WebSocket * socket = static_cast<WebSocket *>(somePointer);
		socket->workMethod();
		return somePointer;
	}
#elif defined(__RE_USING_WINDOWS_THREADS__)
     DWORD WebSocket::workThreadFunc(LPVOID lpParameter)
     {
         WebSocket * socket = static_cast<WebSocket *>(lpParameter);
         socket->workMethod();
         return 0;
     }
#endif
	
	bool WebSocket::createWorkThread()
	{
#if defined(HAVE_PTHREAD_H)	
		pthread_attr_t attr;
		if (pthread_attr_init(&attr) == 0) 
		{
			bool res = false;
			if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) == 0)
			{
				if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0) 
				{
					res = (pthread_create(&_workThread, &attr, WebSocket::workThreadFunc, static_cast<void *>(this)) == 0);
				}
			}
			pthread_attr_destroy(&attr);
			return res;
		}
#elif defined(__RE_USING_WINDOWS_THREADS__)
        HANDLE hThread = CreateThread(NULL,
                                      0,
                                      WebSocket::workThreadFunc,
                                      static_cast<LPVOID>(this),
                                      0,
                                      NULL);
        if (hThread)
        {
            _workThread = hThread;
            return true;
        }
#endif	
        return false;
    }

    void WebSocket::connectToServer()
    {
        if (!this->isConnected())
        {
            RE_ASSERT(this->createWorkThread())
        }
    }

    void WebSocket::cleanup()
    {
        this->lockMutex();

        memset(&_info, 0, sizeof(struct lws_context_creation_info));

        struct libwebsocket_context * context = _context;
		_context = NULL;
		_connection = NULL;
		
		if (context) libwebsocket_context_destroy(context);
		
		REList<WriteBuffer *>::Iterator i = _writeBuffers.iterator();
		while (i.next()) 
		{
			WriteBuffer * b = i.value();
			delete b;
		}
		_writeBuffers.clear();
		
		SAFE_DELETE(_receivedTextBuffer)
		SAFE_DELETE(_receivedBinaryBuffer)
		
		this->unLockMutex();
	}
	
	void WebSocket::disconnectFromServer()
	{
		this->lockMutex();
		_isWorking = 0;
		this->unLockMutex();
		
		// wait thread
		while (_connection || _context) 
		{
			Transport::USleep(4);
		}
		
#if defined(HAVE_PTHREAD_H)	
		void * r = NULL;
		pthread_join(_workThread, &r);
#elif defined(__RE_USING_WINDOWS_THREADS__)
        bool isAlive = false;
        DWORD dwExitCode = 0;
        if (GetExitCodeThread(_workThread, &dwExitCode)) isAlive = (dwExitCode == STILL_ACTIVE);
        if (isAlive) TerminateThread(_workThread, 0);
        CloseHandle(_workThread);
        _workThread = NULL;
#endif	
		
		this->cleanup();
	}
	
	const char * WebSocket::copyUTF8(const REString & from)
	{
		const size_t len = (size_t)from.length();
		const char * utf8 = from.UTF8String();
		if (len > 0 && utf8) 
		{
			char * newMem = (char *)malloc(len + 1);
			if (newMem) 
			{
				memcpy(newMem, utf8, len);
				newMem[len] = (char)0;
				return newMem;
			}
		}
		return NULL;
	}
	
	struct libwebsocket_context * WebSocket::createWebSocketContext()
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
		
		SSLDataSource * dataSource = this->sslDataSource();
		if (dataSource) 
		{
			info.ssl_cert_filepath = WebSocket::copyUTF8(dataSource->clientLocalCertificateFilePath());
			info.ssl_private_key_filepath = WebSocket::copyUTF8(dataSource->clientPrivateKeyFilePath());
			info.ssl_private_key_password = WebSocket::copyUTF8(dataSource->clientPrivateKeyPassPhrase());
			info.ssl_ca_filepath = WebSocket::copyUTF8(dataSource->clientCACertificateFilePath());
			
			info.options = (info.options == 0) ? LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT : info.options | LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
			
			struct libwebsocket_context * context = libwebsocket_create_context(&info);
		
			SAFE_FREE(info.ssl_cert_filepath)
			SAFE_FREE(info.ssl_private_key_filepath)
			SAFE_FREE(info.ssl_private_key_password)
			SAFE_FREE(info.ssl_ca_filepath)
			
			return context;
		}
		
		return libwebsocket_create_context(&info);
	}
	
	void WebSocket::lockMutex()
	{
#if defined(HAVE_PTHREAD_H)	
		pthread_mutex_lock(&_mutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
        TryEnterCriticalSection(&_mutex);
#endif	
	}
	
	void WebSocket::unLockMutex()
	{
#if defined(HAVE_PTHREAD_H)	
		pthread_mutex_unlock(&_mutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
         LeaveCriticalSection(&_mutex);
#endif	
	}
	
	void WebSocket::workMethod()
	{
		this->lockMutex();
		
		_isWorking = 0;
		
		_context = this->createWebSocketContext();
		
		if (!_context)
		{
			this->unLockMutex();
			
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
			
			this->unLockMutex();
			
			this->onError(REString::createWithFormat("Failed to connect to %s:%i", this->host().UTF8String(), this->port()));
			return;
		}
		
		libwebsocket_callback_on_writable(_context, _connection);
		
		_isWorking = 1;
		
		this->unLockMutex();	/// for initialization
		
		int n = 0;
		while (n >= 0 && _isWorking && _context)
		{
#if defined(HAVE_PTHREAD_H)	
			pthread_mutex_lock(&_mutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
            TryEnterCriticalSection(&_mutex);
#endif	
			
			n = _context ? libwebsocket_service(_context, 50) : -1;
	
#if defined(HAVE_PTHREAD_H)	
			pthread_mutex_unlock(&_mutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
             LeaveCriticalSection(&_mutex);
#endif

#if defined(HAVE_UNISTD_H)			
			usleep(50);
#elif defined(__RE_USING_WINDOWS_THREADS__)
            Transport::USleep(50);
#endif	
		}
		
		this->cleanup();
	}
	
	WebSocket::WebSocket(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) : /*REThread(),*/ Transport(processMethod),
#if defined(__RE_USING_WINDOWS_THREADS__)
        _workThread(NULL),
#endif
        _context(NULL),
		_connection(NULL),
		_receivedTextBuffer(NULL),
		_receivedBinaryBuffer(NULL),
		_isWorking(0)
	{
		memset(&_info, 0, sizeof(struct lws_context_creation_info));
		
        RE_ASSERT(Transport::initRecursiveMutex(&_mutex))
	}
	
	WebSocket::~WebSocket()
	{
		_isWorking = 0;
		
		/// wait for thread joined
		while (_connection || _context) 
		{
			Transport::USleep(4);
		}
				
		this->cleanup();
		
#if defined(HAVE_PTHREAD_H)	
		pthread_mutex_destroy(&_mutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
         DeleteCriticalSection(&_mutex);
#endif	
	}
	
	REString WebSocket::transportName() 
	{
		return REString("websocket");
	}
}

#endif /* #ifndef HAVE_SUITABLE_QT_VERSION */

