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


#include "websocket.h"

#if !defined(HAVE_SUITABLE_QT_VERSION) && defined(HAVE_LIBWEBSOCKETS_H)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if !defined(__FUNCTION__)
#if defined(__func__)
#define __FUNCTION__ __func__
#else
#define NO__FUNCTION__
#endif
#endif

#ifndef ERROR_LINE_INFO_STRING
#ifdef NO__FUNCTION__
#define ERROR_LINE_INFO_STRING REString::createWithFormat("File: %s, line: %i", __FILE__, (int)__LINE__)
#else
#define ERROR_LINE_INFO_STRING REString::createWithFormat("File: %s, function: %s, line: %i", __FILE__, __FUNCTION__, (int)__LINE__)
#endif
#endif


namespace FayeCpp {

#if defined(HAVE_PTHREAD_H)			
	pthread_mutex_t WebSocket::ThreadsJoiner::_mutex;
	pthread_t * WebSocket::ThreadsJoiner::_thread = NULL;
#elif defined(__RE_USING_WINDOWS_THREADS__)
	CRITICAL_SECTION WebSocket::ThreadsJoiner::_mutex;
	HANDLE WebSocket::ThreadsJoiner::_thread = NULL;
#endif
	
	bool WebSocket::ThreadsJoiner::clean()
	{
		if (_thread) 
		{
#if defined (HAVE_PTHREAD_H)
			void * r = NULL;
			pthread_join(*_thread, &r);
			if (r == NULL) { free(_thread); _thread = NULL; }
#elif defined(__RE_USING_WINDOWS_THREADS__)
			DWORD dwExitCode = 0;
			do {
				if (GetExitCodeThread(_thread, &dwExitCode) == 0) break; // fail
			} while (dwExitCode == STILL_ACTIVE);
			if (dwExitCode == STILL_ACTIVE) TerminateThread(_thread, 0);
			if (CloseHandle(_thread)) _thread = NULL;
#endif
		}
		return (_thread == NULL);
	}
	
#if defined (HAVE_PTHREAD_H)	
	void WebSocket::ThreadsJoiner::add(pthread_t * t) {
#elif defined(__RE_USING_WINDOWS_THREADS__)		
	void WebSocket::ThreadsJoiner::add(HANDLE t) {
#endif
		LOCK_MUTEX(&_mutex)
		RE_ASSERT(ThreadsJoiner::clean()); /// If assert - thread not finished correctly.
		_thread = t;
		UNLOCK_MUTEX(&_mutex)
	}
	bool WebSocket::ThreadsJoiner::_isInitialized = false;
	bool WebSocket::ThreadsJoiner::init()
	{
		if (!_isInitialized) _isInitialized = WebSocket::initRecursiveMutex(&_mutex);
		return _isInitialized;
	}
	
#if defined(HAVE_PTHREAD_H)
	bool WebSocket::initRecursiveMutex(pthread_mutex_t * mutex)
	{
		pthread_mutexattr_t attr;
		if (pthread_mutexattr_init(&attr) == 0)
		{
			bool isInit = false;
			if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0)
				isInit = (pthread_mutex_init(mutex, &attr) == 0);
			pthread_mutexattr_destroy(&attr);
			return isInit;
		}
		return false;
	}
#elif defined(__RE_USING_WINDOWS_THREADS__)
	bool WebSocket::initRecursiveMutex(LPCRITICAL_SECTION mutex)
	{
		InitializeCriticalSection(mutex);
		return true;
	}
#endif
	
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
		if (!socket->_isWorking) return 0;
		
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
		FAYECPP_DEBUG_LOG("CALLBACK CONNECTION DESTROYED")

		LOCK_MUTEX(&_mutex)
		_isWorking = 0;
		UNLOCK_MUTEX(&_mutex)
		
		this->onDisconnected();
	}
	
	void WebSocket::onCallbackEstablished()
	{
		FAYECPP_DEBUG_LOG("CALLBACK CONNECTION ESTABLISHED")

		this->onConnected();
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
		
		const int writed = libwebsocket_write(connection, &pss->buf[LWS_SEND_BUFFER_PRE_PADDING], pss->len, type);		
		if (writed < 0)
		{
			FAYECPP_DEBUG_LOGA("ERROR %d writing to socket, hanging up", writed)

			libwebsocket_callback_on_writable(context, connection);
			return -1;
		}
        else if (pss->len > writed)
		{
			FAYECPP_DEBUG_LOG("Partial write")

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
			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			REString format = Error::localizedStringForErrorCode(Error::SendingBufferTooLarge);
			info[kErrorLocalizedDescriptionKey] = REString::createWithFormat(format.UTF8String(), (unsigned int)dataSize, (int)MAX_ECHO_PAYLOAD);

			this->onError(Error(kErrorDomainTransport, Error::SendingBufferTooLarge, info));
			return;
		}
		
		bool isError = false;
		LOCK_MUTEX(&_mutex)
		
		WriteBuffer * buffer = new WriteBuffer(data, dataSize);
		if (buffer && buffer->size() == dataSize)
		{
			buffer->tag = (int)type;

			FAYECPP_DEBUG_LOGA("WILL WRITE %i bytes: %s", (int)buffer->size(), (char *)buffer->buffer())

			_writeBuffers.add(buffer);
		}
		else isError = true;
		
		if (!isError && this->isConnected())
		{
			libwebsocket_callback_on_writable(_context, _connection);
		}
		
		UNLOCK_MUTEX(&_mutex)
		
		if (isError)
		{
			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = Error::localizedStringForErrorCode(Error::InternalApplicationError);
			this->onError(Error(kErrorDomainTransport, Error::InternalApplicationError, info));
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
	
#if defined(HAVE_PTHREAD_H)	
	void * WebSocket::workThreadFunc(void * somePointer)
	{
#if defined(HAVE_FUNCTION_PTHREAD_SETNAME_NP) && defined(__APPLE__)	
		pthread_setname_np("FayeCpp WebSocket workThread");
#endif	
		WebSocket * socket = static_cast<WebSocket *>(somePointer);
		socket->workMethod();
		pthread_t * t = socket->_workThread;
		socket->_workThread = NULL;
		socket->onTransportWillSelfDestruct();
		delete socket;
		ThreadsJoiner::add(t);
		return NULL;
	}
#elif defined(__RE_USING_WINDOWS_THREADS__)
	DWORD WebSocket::workThreadFunc(LPVOID lpParameter)
	{
		WebSocket * socket = static_cast<WebSocket *>(lpParameter);
		socket->workMethod();
		HANDLE t = socket->_workThread;
		socket->_workThread = NULL;
		socket->onTransportWillSelfDestruct();
		delete socket;
		ThreadsJoiner::add(t);
		return 0;
	}
#endif
	
	bool WebSocket::createWorkThread()
	{
#if defined(HAVE_PTHREAD_H)	
		_workThread = (pthread_t *)malloc(sizeof(pthread_t));
		if (!_workThread) return false;
		pthread_attr_t attr;
		if (pthread_attr_init(&attr) == 0) 
		{
			bool res = false;
			if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) == 0)
			{
				if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0) 
				{
					memset(_workThread, 0, sizeof(pthread_t));
					res = (pthread_create(_workThread, &attr, WebSocket::workThreadFunc, static_cast<void *>(this)) == 0);
#if !defined(__APPLE__)
#if defined(HAVE_FUNCTION_PTHREAD_SETNAME_NP)
					if (res) pthread_setname_np(*_workThread, "FayeCpp WebSocket workThread");
#elif defined(HAVE_FUNCTION_PTHREAD_SET_NAME_NP)
					if (res) pthread_set_name_np(*_workThread, "FayeCpp WebSocket workThread");
#endif				
#endif	
				}
			}
			pthread_attr_destroy(&attr);
			return res;
		}
#elif defined(__RE_USING_WINDOWS_THREADS__)
        HANDLE hThread = CreateThread(NULL, 0, WebSocket::workThreadFunc, static_cast<LPVOID>(this), 0, NULL);
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
			RE_ASSERT(this->createWorkThread());
		}
	}
	
	void WebSocket::cleanup()
	{
		LOCK_MUTEX(&_mutex)
		
		memset(&_info, 0, sizeof(struct lws_context_creation_info));
		
		if (_context) libwebsocket_context_destroy(_context);
		_context = NULL;
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
		
		UNLOCK_MUTEX(&_mutex)
	}
	
	void WebSocket::disconnectFromServer()
	{
		LOCK_MUTEX(&_mutex)
		_isWorking = 0;
		UNLOCK_MUTEX(&_mutex)
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
	
	struct libwebsocket_context * WebSocket::createWebSocketContext(WebSocket * webSocket)
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
		info.options = webSocket->isUsingIPV6() ? 0 : LWS_SERVER_OPTION_DISABLE_IPV6;
		
		info.user = static_cast<WebSocket *>(webSocket);
		
		SSLDataSource * dataSource = webSocket->sslDataSource();
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
	
	void WebSocket::workMethod()
	{
		LOCK_MUTEX(&_mutex)
		
		_isWorking = 0;
		
		_context = WebSocket::createWebSocketContext(this);
		
		if (!_context)
		{
			UNLOCK_MUTEX(&_mutex)

			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = Error::localizedStringForErrorCode(Error::InternalApplicationError);
			this->onError(Error(kErrorDomainTransport, Error::InternalApplicationError, info));
			return;
		}

		FAYECPP_DEBUG_LOGA("Start connecting to host[%s] port[%i] path[%s]", this->client()->host().UTF8String(), this->client()->port(), this->client()->path().UTF8String())
		
		_connection = libwebsocket_client_connect(_context,
												  this->client()->host().UTF8String(),
												  this->client()->port(),
												  this->client()->isUseSSL() ? 2 : 0,
												  this->client()->path().UTF8String(),
												  this->client()->host().UTF8String(),
												  "origin",
												  NULL,
												  -1);
		if (!_connection)
		{
			if (_context) libwebsocket_context_destroy(_context);
			_context = NULL;
			
			UNLOCK_MUTEX(&_mutex)

			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			REString format = Error::localizedStringForErrorCode(Error::FailedConnectToHost);
			info[kErrorLocalizedDescriptionKey] = REString::createWithFormat(format.UTF8String(), this->client()->host().UTF8String(), this->client()->port());
			info[kErrorURLKey] = this->client()->url();

			this->onError(Error(kErrorDomainTransport, Error::FailedConnectToHost, info));
			return;
		}
		
		libwebsocket_callback_on_writable(_context, _connection);
		
		_isWorking = 1;
		
		UNLOCK_MUTEX(&_mutex)	/// for initialization
		
		int n = 0;
		while (n >= 0 && _isWorking && _context)
		{
			LOCK_MUTEX(&_mutex)
			
			n = _context ? libwebsocket_service(_context, 75) : -1;

			UNLOCK_MUTEX(&_mutex)
			
#if defined(HAVE_UNISTD_H)			
            usleep(75);   /// 1s = 1'000'000 microsec.
#elif defined(__RE_USING_WINDOWS_THREADS__)
            Sleep(1);     /// 1s = 1'000 millisec.
#endif	
		}
		
		this->cleanup();
	}
	
	WebSocket::WebSocket(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) : Transport(processMethod),
#if defined(HAVE_PTHREAD_H) || defined(__RE_USING_WINDOWS_THREADS__)
		_workThread(NULL),
#endif
		_context(NULL),
		_connection(NULL),
		_receivedTextBuffer(NULL),
		_receivedBinaryBuffer(NULL),
		_isWorking(0)
	{
		RE_ASSERT(ThreadsJoiner::init());
		
		memset(&_info, 0, sizeof(struct lws_context_creation_info));
		
		RE_ASSERT(WebSocket::initRecursiveMutex(&_mutex));
	}
	
	WebSocket::~WebSocket()
	{
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

