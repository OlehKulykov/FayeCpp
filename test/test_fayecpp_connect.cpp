/*
 *   Copyright (c) 2014 - 2015 Kulykov Oleh <info@resident.name>
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


#include <stdlib.h>
#include <assert.h>

#if defined(CMAKE_BUILD)
#undef CMAKE_BUILD
#endif

#if defined(__BUILDING_RECORE_DYNAMIC_LIBRARY__)
#undef __BUILDING_RECORE_DYNAMIC_LIBRARY__
#endif


#include <fayecpp.h>


#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif


#if defined(CMAKE_BUILD)
#undef CMAKE_BUILD
#endif


#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif


#if !defined(__RE_HAVE_THREADS__) && defined(__RE_OS_WINDOWS__)
#include <Windows.h>
#define __RE_THREADING_WINDOWS__ 1
#define __RE_HAVE_THREADS__ 1
#endif

#if !defined(__RE_HAVE_THREADS__) && defined(HAVE_PTHREAD_H)
#include <pthread.h>
#define __RE_THREADING_PTHREAD__ 1
#define __RE_HAVE_THREADS__ 1
#endif

#if defined(__RE_THREADING_PTHREAD__)
#define LOCK_MUTEX(mPtr) pthread_mutex_lock(mPtr);
#define UNLOCK_MUTEX(mPtr) pthread_mutex_unlock(mPtr);
#elif defined(__RE_THREADING_WINDOWS__)
#define LOCK_MUTEX(mPtr) TryEnterCriticalSection(mPtr);
#define UNLOCK_MUTEX(mPtr) LeaveCriticalSection(mPtr);
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(o) if(o){delete o;o=NULL;}
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(m) if(m){free((void *)m);m=NULL;}
#endif


using namespace FayeCpp;

class FayeDelegate;

static Client * _client = NULL;
static FayeDelegate * _delegate = NULL;
static int _result = EXIT_SUCCESS;


#define STEP_WAIT 0
#define STEP_CONNECT 1

static int _step = STEP_CONNECT;
static int _subscribedChannels = 0;

class FayeDelegate : public FayeCpp::Delegate
{
public:
	virtual void onFayeTransportConnected(FayeCpp::Client * client)
	{
		RELog::log("DELEGATE onFayeTransportConnected");
	}

	virtual void onFayeTransportDisconnected(FayeCpp::Client * client)
	{
		RELog::log("DELEGATE onFayeTransportDisconnected");
		_result++;
		SAFE_DELETE(_client)
	}

	virtual void onFayeClientConnected(FayeCpp::Client * client)
	{
		RELog::log("DELEGATE onFayeClientConnected");
		RELog::log("Start connecting OK");
	}

	virtual void onFayeClientDisconnected(FayeCpp::Client * client)
	{
		RELog::log("DELEGATE onFayeClientDisconnected");
		_result++;
		SAFE_DELETE(_client)
	}

	virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client,
												 const FayeCpp::REString & channel)
	{
		RELog::log("DELEGATE onFayeClientSubscribedToChannel: %s", channel.UTF8String());

		if ((++_subscribedChannels) == 2)
		{
			SAFE_DELETE(_client)
		}
	}

	virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client,
													 const FayeCpp::REString & channel)
	{
		RELog::log("DELEGATE onFayeClientUnsubscribedFromChannel");
	}

	virtual void onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client,
														const FayeCpp::REVariantMap & message,
														const FayeCpp::REString & channel)
	{
		RELog::log("DELEGATE onFayeClientReceivedMessageFromChannel");
	}

	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::REVariantMap & message)
	{
		RELog::log("DELEGATE onFayeClientWillSendMessage");
	}

	virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client,
												FayeCpp::REVariantMap & message)
	{
		RELog::log("DELEGATE onFayeClientWillReceiveMessage");
	}

	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{
		RELog::log("DELEGATE ERROR: %s", errorString.UTF8String());
		_result++;
		SAFE_DELETE(_client)
	}

	FayeDelegate() { }
	virtual ~FayeDelegate() { }
};

int main(int argc, char* argv[])
{
	RELog::log("Client info: %s", Client::info());
	RELog::log("Start test");

	_client = new Client();
	if (!_client) return (++_result);

	_delegate = new FayeDelegate();
	if (!_delegate) return (++_result);

	while (_client && _delegate)
	{
		switch (_step)
		{
			case STEP_CONNECT:
				RELog::log("Start connecting ...");
				_step = STEP_WAIT;
				_client->setExtValue("Some ext value");
				_client->setUsingIPV6(false);
				_client->setUrl("xxxxx://xxxxxxxxxxxxxx:80/faye");
				_client->setDelegate(_delegate);
				_client->connect();
				_client->subscribeToChannel("/xxxxxxxxxxx");
				_client->subscribeToChannel("/xxxxxx/xxxxxxxxx");
				break;

			default:
				break;
		}

#if defined(__RE_THREADING_WINDOWS__)
		Sleep(10);
#elif defined(__RE_THREADING_PTHREAD__) && defined(HAVE_FUNCTION_USLEEP)
		usleep(100);
#endif
	}

	SAFE_DELETE(_client)
	SAFE_DELETE(_delegate)

	return 0;
//	return _result;
}

