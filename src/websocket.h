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


#ifndef __FAYECPP_FAYE_CLIENT_H__
#define __FAYECPP_FAYE_CLIENT_H__


#include "../fayecpp.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if !defined(HAVE_SUITABLE_QT_VERSION) && defined(HAVE_LIBWEBSOCKETS_H)

#include "transport.h"

#include <libwebsockets.h>

namespace FayeCpp {

	class WebSocket : public Transport
	{
	public:
#if defined(HAVE_PTHREAD_H)
		static bool initRecursiveMutex(pthread_mutex_t * mutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
		static bool initRecursiveMutex(LPCRITICAL_SECTION mutex);
#endif
	private:
		class ThreadsJoiner
		{
#if defined(HAVE_PTHREAD_H)
		private:
			static pthread_mutex_t _mutex;
			static pthread_t * _thread;
		public:
			static void add(pthread_t * t);
#elif defined(__RE_USING_WINDOWS_THREADS__)
		private:
			static CRITICAL_SECTION _mutex;
			static HANDLE _thread;
		public:
			static void add(HANDLE t);
#endif
		private:	
			static bool _isInitialized;
			static bool clean();
		public:
			static bool init();
		};
		
		class WriteBuffer : public REBuffer
		{
		public:
			int tag;
			WriteBuffer(const void * buff, const REUInt32 buffSize) : REBuffer(buff, buffSize), tag(0) { }
			virtual ~WriteBuffer() { }
		};
		
		REList<WriteBuffer *> _writeBuffers;
		
		struct lws_context_creation_info _info;
		
#if defined(HAVE_PTHREAD_H)	
		pthread_mutex_t _mutex;
		pthread_t * _workThread;
#elif defined(__RE_USING_WINDOWS_THREADS__)
        HANDLE _workThread;
        CRITICAL_SECTION _mutex;
#endif	
		
		struct libwebsocket_context * _context;
		struct libwebsocket * _connection;
		
		REBuffer * _receivedTextBuffer;
		REBuffer * _receivedBinaryBuffer;
		
		int _isWorking;
		
#if defined(HAVE_PTHREAD_H)	
		static void * workThreadFunc(void * somePointer);
#elif defined(__RE_USING_WINDOWS_THREADS__)
        static DWORD WINAPI workThreadFunc(LPVOID lpParameter);
#endif	
		
		bool createWorkThread();
		
		#define MAX_ECHO_PAYLOAD 4096
		typedef struct echoSessionData
		{
			unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + MAX_ECHO_PAYLOAD + LWS_SEND_BUFFER_POST_PADDING];
			unsigned int len;
			unsigned int index;
		} EchoSessionData;
				
		static struct libwebsocket_protocols protocols[];
		static int callbackEcho(struct libwebsocket_context * context,
								struct libwebsocket * wsi,
								enum libwebsocket_callback_reasons reason,
								void * user,
								void * input,
								size_t len);
		
		void onCallbackConnectionDestroyed();
		void onCallbackEstablished();
		int onCallbackWritable(struct libwebsocket_context * context,
							   struct libwebsocket * connection,
							   EchoSessionData * pss);
		void onCallbackReceive(struct libwebsocket * wsi, void * input, size_t len);
		
		void addWriteBufferData(const unsigned char * data, const REUInt32 dataSize, const enum libwebsocket_write_protocol type);
		void cleanup();
		void workMethod();
		
		static const char * copyUTF8(const REString & from);
		static struct libwebsocket_context * createWebSocketContext(WebSocket * webSocket);
		
	public:
		virtual const REString name() const;
		
		virtual void sendData(const unsigned char * data, const REUInt32 dataSize);		
		virtual void sendText(const char * text, const REUInt32 textSize);
		
		virtual void connectToServer();
		virtual void disconnectFromServer();
		
		WebSocket(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod);
		virtual ~WebSocket();
		
		static REString transportName();
	};
	
}

#endif /* !HAVE_SUITABLE_QT_VERSION && HAVE_LIBWEBSOCKETS_H */

#endif /* __FAYECPP_FAYE_CLIENT_H__ */
