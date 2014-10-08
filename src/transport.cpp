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

namespace FayeCpp {
	
	void * Transport::Messanger::workThreadFunc(void * somePointer)
	{
		Transport::Messanger * messanger = static_cast<Transport::Messanger *>(somePointer);
		
		while (messanger->_isWorking) 
		{
			pthread_mutex_lock(&messanger->_mutex);
			
			if ( !messanger->sendSingleResponce() )
			{
#ifdef FAYECPP_DEBUG_MESSAGES	
				RELog::log("MESSANGER SELF PAUSE");
#endif				
				messanger->_isSuspended = true;
				do
				{
					pthread_cond_wait(&messanger->_conditionVariable, &messanger->_mutex);
				} while (messanger->_isSuspended);
			}
			
			pthread_mutex_unlock(&messanger->_mutex);
		}
		
		return somePointer;
	}
	
	bool Transport::Messanger::sendSingleResponce()
	{
		Responce * responce = NULL;
		REList<Responce *>::Iterator i = _responces.iterator();
		while (!responce && i.next()) 
		{
			responce = i.value();
			_responces.removeNode(i.node());
		}
		
		if (responce) 
		{
#ifdef FAYECPP_DEBUG_MESSAGES			
			RELog::log("MESSANGER SEND SINGLE RESPONCE ...");
#endif
			
			_processMethod->invokeWithPointer(responce);
			delete responce;
			
#ifdef FAYECPP_DEBUG_MESSAGES			
			RELog::log("MESSANGER SEND SINGLE RESPONCE OK");
#endif			
		}
		
		return (responce != NULL);
	}
	
	void Transport::Messanger::addResponce(Responce * responce)
	{
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("MESSANGER ADD RESPONCE ...");
#endif		
		
		pthread_mutex_lock(&_mutex);
		_responces.add(responce);
		_isSuspended = false;
		pthread_cond_signal(&_conditionVariable);
		pthread_mutex_unlock(&_mutex);
	
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("MESSANGER ADD RESPONCE OK");
#endif		
	}
	
	bool Transport::Messanger::createWorkThread()
	{
		pthread_attr_t attr;
		if (pthread_attr_init(&attr) == 0) 
		{
			bool res = false;
			if (pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM) == 0)
			{
				if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0) 
				{
					res = (pthread_create(&_thread, &attr, Transport::Messanger::workThreadFunc, static_cast<void *>(this)) == 0);
				}
			}
			pthread_attr_destroy(&attr);
			return res;
		}
		return false;
	}
	
	bool Transport::Messanger::initConditionVariable()
	{
		pthread_condattr_t attr;
		pthread_condattr_init(&attr);
		pthread_cond_init(&_conditionVariable, &attr);
		pthread_condattr_destroy(&attr);
		return true;
	}
	
	Transport::Messanger::Messanger(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) : 
		_processMethod(processMethod),
		_isWorking(true),
		_isSuspended(false)
	{
#if defined(HAVE_ASSERT_H) 
		assert(_processMethod);
#endif		
		
		RE_ASSERT(this->initConditionVariable())
		RE_ASSERT(Transport::initRecursiveMutex(&_mutex))
		RE_ASSERT(this->createWorkThread())
	}
	
	void Transport::Messanger::stopWorking()
	{
		pthread_mutex_lock(&_mutex);
		_isWorking = false;
		_isSuspended = false;
		pthread_cond_signal(&_conditionVariable);
		pthread_mutex_unlock(&_mutex);
		
		void * r = NULL;
		pthread_join(_thread, &r);
	}
	
	Transport::Messanger::~Messanger()
	{
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
		
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("Transport ~Messanger()");
#endif		
	}
	
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
	
	bool Transport::isUsingIPV6() const 
	{
		Client * client = this->client(); 
		return client ? client->isUsingIPV6() : false; 
	}
	
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
	
	Client * Transport::client() const
	{
		return _processMethod->classPointer();
	}
	
	Delegate * Transport::delegate() const
	{
		//No transport without client.
		return this->client()->delegate();
	}
	
	SSLDataSource * Transport::sslDataSource() const
	{
		//No transport without client.
		return this->client()->sslDataSource();
	}
	
	void Transport::onConnected()
	{
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("TRANSPORT CONNECTED");
#endif		
		_isConnected = true;
		
		Responce * message = new Responce();
		if (message)
		{
			message->setType(Responce::ResponceTransportConnected);			
			_messanger->addResponce(message);
		}
	}
	
	void Transport::onDisconnected()
	{
#ifdef FAYECPP_DEBUG_MESSAGES		
		RELog::log("TRANSPORT DISCONNECTED");
#endif		
		_isConnected = false;
		
		Responce * message = new Responce();
		if (message)
		{
			message->setType(Responce::ResponceTransportDisconnected);
			_messanger->addResponce(message);
		}
	}
	
	void Transport::onTextReceived(const char * text)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("TRANSPORT RECEIVED: %s", text);
#endif
		Responce * message = new Responce();
		if (message)
		{
			message->setMessageText(text).setType(Responce::ResponceMessage);
			_messanger->addResponce(message);
		}
	}
	
	void Transport::onDataReceived(const unsigned char * data, const size_t dataSize)
	{
		Responce * message = new Responce();
		if (message)
		{
			message->setMessageData(data, dataSize).setType(Responce::ResponceMessage);			
			_messanger->addResponce(message);
		}
	}
	
	void Transport::onError(const REString & error)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("TRANSPORT ERROR: %s", error.UTF8String());
#endif
		Responce * message = new Responce();
		if (message)
		{
			message->setType(Responce::ResponceTransportError).setErrorString(error);
			_messanger->addResponce(message);
		}
	}
	
	void Transport::onError(const char * error)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("TRANSPORT ERROR: %s", error);
#endif
		Responce * message = new Responce();
		if (message)
		{
			message->setType(Responce::ResponceTransportError).setErrorString(error);
			_messanger->addResponce(message);
		}
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
	
	Transport::Transport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod) :
		_processMethod(processMethod),
		_messanger(new Transport::Messanger(processMethod)),
		_lastSendTime(RETime::time()),
		_port(-1),
		_isUseSSL(false),
		_isConnected(false)
	{
#if defined(HAVE_ASSERT_H) 
		assert(_processMethod);
		assert(this->client());
		assert(_messanger);
#endif		
		
		_advice.reconnect = ADVICE_RECONNECT_NONE;
		_advice.timeout = _advice.interval = -1;
	}
	
	Transport::~Transport()
	{
		_messanger->stopWorking();
		delete _messanger;
        delete _processMethod;
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
