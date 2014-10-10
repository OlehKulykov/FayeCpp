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

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

namespace FayeCpp {
	
#if defined(USE_TRANSPORT_MESSENGER)	
	
#if defined(HAVE_PTHREAD_H)		
	void * Transport::Messenger::workThreadFunc(void * somePointer)
	{
#if defined(HAVE_FUNCTION_PTHREAD_SETNAME_NP) && defined(__APPLE__)
		pthread_setname_np("FayeCpp Transport Messenger workThread");
#endif		
		Transport::Messenger * messenger = static_cast<Transport::Messenger *>(somePointer);
		
		while (messenger->_isWorking)
		{
			pthread_mutex_lock(&messenger->_mutex);
			
			if ( !messenger->sendSingleResponce() )
			{
#ifdef FAYECPP_DEBUG_MESSAGES	
				RELog::log("MESSENGER SELF PAUSED");
#endif				
				messenger->_isSuspended = true;
				do
				{
					pthread_cond_wait(&messenger->_conditionVariable, &messenger->_mutex);
				} while (messenger->_isSuspended);
			}
			
			pthread_mutex_unlock(&messenger->_mutex);
		}
		
		return NULL;
	}
	
	bool Transport::Messenger::initConditionVariable(pthread_cond_t * conditionVariable)
	{
		pthread_condattr_t attr;
		if (pthread_condattr_init(&attr) == 0)
		{
			bool res = false;
			if (pthread_condattr_setpshared(&attr , PTHREAD_PROCESS_PRIVATE) == 0)
			{
				res = (pthread_cond_init(conditionVariable, &attr) == 0);
			}
			pthread_condattr_destroy(&attr);
			return res;
		}
		return false;
	}
#elif defined(__RE_USING_WINDOWS_THREADS__)
	bool Transport::Messenger::initConditionVariable(PCONDITION_VARIABLE conditionVariable)
	{
		InitializeConditionVariable(conditionVariable);
		return true;
	}
	
	DWORD Transport::Messenger::workThreadFunc(LPVOID lpParameter)
	{
		Transport::Messenger * messenger = static_cast<Transport::Messenger *>(lpParameter);
		while (messenger->_isWorking)
		{
			TryEnterCriticalSection(&messenger->_mutex);
			
			if ( !messenger->sendSingleResponce() )
			{
#ifdef FAYECPP_DEBUG_MESSAGES
				RELog::log("MESSENGER SELF PAUSED");
#endif
				messenger->_isSuspended = true;
				do
				{
					SleepConditionVariableCS(&messenger->_conditionVariable, &messenger->_mutex, INFINITE);
				} while (messenger->_isSuspended);
			}
			
			LeaveCriticalSection(&messenger->_mutex);
		}
		return 0;
	}
#endif	
	
	bool Transport::Messenger::sendSingleResponce()
	{
		Responce * responce = NULL;
		REList<Responce *>::Iterator i = _responces.iterator();
		while (!responce && i.next())
		{
			responce = i.value();
			_responces.removeNode(i.node());
		}
		time(&_lastWorkTime);
		if (responce)
		{
#ifdef FAYECPP_DEBUG_MESSAGES			
			RELog::log("MESSENGER SEND SINGLE RESPONCE ...");
#endif
			_processMethod->invokeWithPointer(responce);
			delete responce;
			
#ifdef FAYECPP_DEBUG_MESSAGES			
			RELog::log("MESSENGER SEND SINGLE RESPONCE OK");
#endif			
		}
		
		return (responce != NULL);
	}
	
	time_t Transport::Messenger::lastWorkTime() const
	{
		return _lastWorkTime;
	}
	
	void Transport::Messenger::addResponce(Responce * responce)
	{
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("MESSENGER ADD RESPONCE ...");
#endif		
		
#if defined(HAVE_PTHREAD_H)
		pthread_mutex_lock(&_mutex);
		if (responce) _responces.add(responce);
		_isSuspended = false;
		pthread_cond_signal(&_conditionVariable);
		pthread_mutex_unlock(&_mutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
		TryEnterCriticalSection(&_mutex);
		if (responce) _responces.add(responce);
		_isSuspended = false;
        WakeConditionVariable(&_conditionVariable);
		LeaveCriticalSection(&_mutex);
#endif	
		
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("MESSENGER ADD RESPONCE OK");
#endif		
	}
	
	bool Transport::Messenger::createWorkThread()
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
					res = (pthread_create(&_thread, &attr, Transport::Messenger::workThreadFunc, static_cast<void *>(this)) == 0);
#if !defined(__APPLE__)
#if defined(HAVE_FUNCTION_PTHREAD_SETNAME_NP)
					if (res) pthread_setname_np(_thread, "FayeCpp Transport Messenger workThread");
#elif defined(HAVE_FUNCTION_PTHREAD_SET_NAME_NP)
					if (res) pthread_set_name_np(_thread, "FayeCpp Transport Messenger workThread");
#endif				
#endif	
				}
			}
			pthread_attr_destroy(&attr);
			return res;
		}
#elif defined(__RE_USING_WINDOWS_THREADS__)
        HANDLE hThread = CreateThread(NULL, 0, Transport::Messenger::workThreadFunc, static_cast<LPVOID>(this), 0, NULL);
		if (hThread)
		{
			_thread = hThread;
			return true;
		}
#endif	
		return false;
	}
	
	Transport::Messenger::Messenger(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) :
#if defined(__RE_USING_WINDOWS_THREADS__)
		_thread(NULL),
#endif
		_processMethod(processMethod),
		_lastWorkTime(0),
		_isWorking(true),
		_isSuspended(false)
	{
#if defined(HAVE_ASSERT_H) 
		assert(_processMethod);
#endif		
		
		RE_ASSERT(Transport::Messenger::initConditionVariable(&_conditionVariable))
		RE_ASSERT(Transport::initRecursiveMutex(&_mutex))
		RE_ASSERT(this->createWorkThread())
	}
	
	void Transport::Messenger::stopWorking()
	{
#if defined(HAVE_PTHREAD_H)			
		pthread_mutex_lock(&_mutex);
        _isWorking = _isSuspended = false;
		pthread_cond_signal(&_conditionVariable);
		pthread_mutex_unlock(&_mutex);
		
		void * r = NULL;
		pthread_join(_thread, &r);
#elif (__RE_USING_WINDOWS_THREADS__)
		TryEnterCriticalSection(&_mutex);
        _isWorking = _isSuspended = false;
        WakeConditionVariable(&_conditionVariable);
		LeaveCriticalSection(&_mutex);

        DWORD dwExitCode = 0;
        do {
            if (GetExitCodeThread(_thread, &dwExitCode) == 0) break; // fail
        } while (dwExitCode == STILL_ACTIVE);
        if (dwExitCode == STILL_ACTIVE) TerminateThread(_thread, 0);
        CloseHandle(_thread);
        _thread = NULL;
#endif

#ifdef FAYECPP_DEBUG_MESSAGES
        RELog::log("MESSENGER STOPED.");
#endif
	}
	
	Transport::Messenger::~Messenger()
	{
#if defined(HAVE_PTHREAD_H)	
		pthread_mutex_lock(&_mutex);
		REList<Responce *>::Iterator i = _responces.iterator();
		while (i.next())
		{
			Responce * r = i.value();
			delete r;
		}
		_responces.clear();
		pthread_mutex_unlock(&_mutex);
		
		pthread_cond_destroy(&_conditionVariable);
		pthread_mutex_destroy(&_mutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
		TryEnterCriticalSection(&_mutex);
		REList<Responce *>::Iterator i = _responces.iterator();
		while (i.next())
		{
			Responce * r = i.value();
			delete r;
		}
		_responces.clear();
		LeaveCriticalSection(&_mutex);
        DeleteCriticalSection(&_mutex);
#endif 
		
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("Transport ~Messenger()");
#endif		
	}
#endif 
	
#if defined(HAVE_PTHREAD_H)	
	bool Transport::initRecursiveMutex(pthread_mutex_t * mutex)
	{
		pthread_mutexattr_t attr;
		if (pthread_mutexattr_init(&attr) == 0)
		{
			bool isInit = false;
			if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) == 0)
			{
				isInit = (pthread_mutex_init(mutex, &attr) == 0);
			}
			pthread_mutexattr_destroy(&attr);
			return isInit;
		}
		return false;
	}
	
#elif defined(__RE_USING_WINDOWS_THREADS__)
	bool Transport::initRecursiveMutex(LPCRITICAL_SECTION mutex)
	{
		InitializeCriticalSection(mutex);
		return true;
	}
#endif
	
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
#if defined(USE_TRANSPORT_MESSENGER) 
		_messenger->stopWorking();
		delete _messenger;
#endif		
		if (_processMethod) delete _processMethod;
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
		
#ifdef HAVE_SUITABLE_QT_VERSION
		types.add(WebSocketQt::transportName());
#elif defined(HAVE_LIBWEBSOCKETS_H)
		types.add(WebSocket::transportName());
#endif
		return types;
	}
	
	bool Transport::isSupportsIPV6()
	{
#ifdef HAVE_SUITABLE_QT_VERSION
		
#elif defined(HAVE_LIBWEBSOCKETS_H)
		
#if defined(LWS_USE_IPV6)	
		return true;
#endif		
		
#endif
		return false;
	}
	
	bool Transport::isSupportsSSLConnection()
	{
#ifdef HAVE_SUITABLE_QT_VERSION
		return true;
#elif defined(HAVE_LIBWEBSOCKETS_H)
		
#if defined(LWS_OPENSSL_SUPPORT)	
		return true;
#endif		
		
#endif
		return false;
	}
	
}
