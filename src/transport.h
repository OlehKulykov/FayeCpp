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


#ifndef __FAYECPP_TRANSPORT_H__
#define __FAYECPP_TRANSPORT_H__

#include "../fayecpp.h"
#include "classmethodwrapper.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#define RE_ASSERT(r) assert(r)
#else
#define RE_ASSERT(r) r
#endif

#if defined(HAVE_PTHREAD_H)
#include <pthread.h>
#elif defined(__RE_OS_WINDOWS__)
/* Use Windows threading */
#ifndef __RE_USING_WINDOWS_THREADS__
#define __RE_USING_WINDOWS_THREADS__ 1
#endif
#include <Windows.h>
#endif /* __RE_OS_WINDOWS__ */

#if defined(HAVE_PTHREAD_H)
#define LOCK_MUTEX(mPtr) pthread_mutex_lock(mPtr);
#define UNLOCK_MUTEX(mPtr) pthread_mutex_unlock(mPtr);
#elif defined(__RE_USING_WINDOWS_THREADS__)
#define LOCK_MUTEX(mPtr) TryEnterCriticalSection(mPtr);
#define UNLOCK_MUTEX(mPtr) LeaveCriticalSection(mPtr);
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(o) if(o){delete o;o=NULL;}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(m) if(m){free((void *)m);m=NULL;}
#endif

#if defined(FAYECPP_DEBUG_MESSAGES)
#define FAYECPP_DEBUG_LOG(s) RELog::log(s);
#define FAYECPP_DEBUG_LOGA(s, ...) RELog::log(s, __VA_ARGS__);
#else
#define FAYECPP_DEBUG_LOG(s) 
#define FAYECPP_DEBUG_LOGA(s, ...)
#endif

namespace FayeCpp {
	
#define ADVICE_RECONNECT_NONE 0
#define ADVICE_RECONNECT_RETRY 1
#define ADVICE_RECONNECT_HANDSHAKE 2

	class Transport
	{
	public:
		typedef struct _adviceStructure
		{
			RETimeInterval interval;
			RETimeInterval timeout;
			int reconnect;
		} Advice;
		
	private:
		ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * _processMethod;
		Advice _advice;
		
		bool _isConnected;
		bool _isSelfDestructing;

		Delegate * delegate() const;
		
	protected:
		Client * client() const;
		SSLDataSource * sslDataSource() const;
		bool isUsingIPV6() const;
		RETimeInterval adviceInterval() const;
		RETimeInterval adviceTimeout() const;
		int adviceReconnect() const;
		
		void onConnected();
		void onDisconnected();
		void onTextReceived(const char * text);
		void onDataReceived(const unsigned char * data, const size_t dataSize);
		void onError(const Error & error);
		void onTransportWillSelfDestruct();

	public:
		bool isSelfDestructing() const;
		void receivedAdvice(const REVariantMap & advice);
		bool isConnected() const;
		
		virtual const REString name() const = 0;

		virtual void sendData(const unsigned char * data, const REUInt32 dataSize) = 0;
		virtual void sendText(const char * text, const REUInt32 textSize) = 0;

		virtual void connectToServer() = 0;
		virtual void disconnectFromServer() = 0;
		
		Transport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod);
		virtual ~Transport();

        static Transport * createNewTransport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod);
		static void deleteTransport(Transport * transport);
        static REStringList availableConnectionTypes();
		static bool isSupportsIPV6();
		static bool isSupportsSSLConnection();
	};
	
}

#endif /* __FAYECPP_TRANSPORT_H__ */
