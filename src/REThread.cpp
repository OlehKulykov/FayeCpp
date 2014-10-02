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


#include "REThread.h"
#include "REMutex.h"

#ifdef __RE_DEBUG_MODE__
#include "../../include/recore/RELog.h"
#endif


#if defined(HAVE_RECORE_SDK_CONFIG_H)
#include "recore_sdk_config.h"
#endif

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
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

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

#if defined(HAVE_STRING_H)
#include <string.h>
#endif

#if defined(HAVE_ERRNO_H)
#include <errno.h>
#endif

#if defined(HAVE_SYS_ERRNO_H)
#include <sys/errno.h>
#endif

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined(HAVE_DISPATCH_DISPATCH_H)
#include <dispatch/dispatch.h>
#endif

#if defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif

#if !defined(HAVE_FUNCTION_PTHREAD_CANCEL) && defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif

namespace FayeCpp {

/// -----------------------------------------------------------------------

class REThreadInternal 
{
private:
	REThread * _parent;

#if defined(HAVE_PTHREAD_H)	
	pthread_t _thread;
	pthread_attr_t _attr;
#elif defined(__RE_USING_WINDOWS_THREADS__)
	HANDLE _thread;
#endif

public:	
	bool start();
	bool cancel();
	
	REThreadInternal(REThread * parent);
	~REThreadInternal();
	
#if defined(HAVE_PTHREAD_H)	
	static void * threadFunction(void * p);
	static pthread_t mainPThread;
	
#elif defined(__RE_USING_WINDOWS_THREADS__)
	static DWORD mainThreadID;
	static DWORD WINAPI threadProc(LPVOID lpParameter);
#endif	
};

#if defined(HAVE_PTHREAD_H)
pthread_t REThreadInternal::mainPThread = (pthread_t)0;
void * REThreadInternal::threadFunction(void * p)
{
	REThreadInternal * ti = static_cast<REThreadInternal *>(p);
	REThread * t = ti->_parent;
	
	if (t) 
	{
		t->threadBody();
	}
	
	return p;
}
#elif defined(__RE_USING_WINDOWS_THREADS__)
DWORD REThreadInternal::mainThreadID = 0;
DWORD REThreadInternal::threadProc(LPVOID lpParameter)
{
	REThreadInternal * ti = static_cast<REThreadInternal *>(lpParameter);
	REThread * t = ti->_parent;
	
	if (t) 
	{
		t->threadBody();
	}
	
	return 0;
}
#endif

bool REThreadInternal::cancel()
{
#if defined(HAVE_PTHREAD_H)	
	int s = 0;
	
#if defined(HAVE_FUNCTION_PTHREAD_CANCEL)	
	s = pthread_cancel(_thread);
	if (s != 0) 
	{
		return false;
	}
#else
	s = pthread_kill(_thread, SIGUSR2);
	if (s != 0) 
	{
		RELog::log("REThread: no function 'pthread_cancel' && error 'pthread_kill' thread %lu, error = %i", 
				   (unsigned long)_thread, 
				   s);
	} 
#endif	
	
	if (!_parent->isJoinable()) 
	{
		return true;
	}
	
	void * r = NULL;
	s = pthread_join(_thread, &r);
	if (s != 0)
	{

#if defined(HAVE_ERRNO_H) || defined(HAVE_SYS_ERRNO_H) 		
		switch (s) 
		{
			case EDEADLK:
				printf("\nREThread: A deadlock was detected (e.g., two threads tried to join with each other); or thread specifies the calling thread.");
			break;
			
			case EINVAL:
				printf("\nREThread: Thread is not a joinable thread. Another thread is already waiting to join with this thread.");
			break;
			
			case ESRCH:
				printf("\nREThread: No thread with the ID thread could be found.");
			break;
				
			default:
			break;
		}
#endif	/* HAVE_ERRNO_H || HAVE_SYS_ERRNO_H */	
		return false;
	}

#if defined(HAVE_FUNCTION_PTHREAD_CANCEL)	
	const bool res = (r == PTHREAD_CANCELED);
#else
	const bool res = true;
#endif	
	return res;
	
#elif defined (__RE_USING_WINDOWS_THREADS__)
	if (_thread)
	{
		bool isAlive = false;
		DWORD dwExitCode = 0;
		if (GetExitCodeThread(_thread, &dwExitCode))
		{
			// if return code is STILL_ACTIVE,
			// then thread is live.
			isAlive = (dwExitCode == STILL_ACTIVE);
		}
		if (isAlive)
		{
			TerminateThread(_thread, 0);
		}
		CloseHandle(_thread);
		_thread = (HANDLE)0;
		return true;
	}
#endif
	
	return true;
}

bool REThreadInternal::start()
{
#if defined(HAVE_PTHREAD_H)	
	return (pthread_create(&_thread, &_attr, REThreadInternal::threadFunction, (void *)this) == 0);
#elif defined(__RE_USING_WINDOWS_THREADS__)
	HANDLE hThread = CreateThread(NULL, 0, REThreadInternal::threadProc, (LPVOID)this, 0, NULL);
	if (hThread)
	{
		_thread = hThread;
		return true;
	}
#endif	
	
	return false;
}

#if !defined(HAVE_FUNCTION_PTHREAD_CANCEL) && defined(HAVE_PTHREAD_H)
static void thread_exit_handler(int sig)
{
	pthread_exit(0);
}
#endif
	
REThreadInternal::REThreadInternal(REThread * parent) : 
	_parent(parent)
{
#if defined(HAVE_PTHREAD_H)	
	
#if !defined(HAVE_FUNCTION_PTHREAD_CANCEL)
	static bool needSetup = true;
	if (needSetup)
	{
		needSetup = false;
		struct sigaction actions;
		memset(&actions, 0, sizeof(actions));
		sigemptyset(&actions.sa_mask);
		actions.sa_flags = 0;
		actions.sa_handler = thread_exit_handler;
		sigaction(SIGUSR2, &actions, NULL);
	}
#endif
	
	memset(&_thread, 0, sizeof(pthread_t));
	memset(&_attr, 0, sizeof(pthread_attr_t));
	
	pthread_attr_init(&_attr);
	
#if defined(HAVE_FUNCTION_PTHREAD_ATTR_SETSCOPE)
	pthread_attr_setscope(&_attr, PTHREAD_SCOPE_SYSTEM);
#endif
	
	if (_parent->isJoinable()) 
	{
		pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_JOINABLE);
	}
	
	if (_parent->isDetached()) 
	{
		pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
	}
#elif defined(__RE_USING_WINDOWS_THREADS__)
	_thread = (HANDLE)0;
#endif
}

REThreadInternal::~REThreadInternal()
{
#if defined(HAVE_PTHREAD_H)	
	pthread_attr_destroy(&_attr);
#endif	
}

/// -----------------------------------------------------------------------

/// Start thread's work
bool REThread::start()
{
	if (_t)
	{
		return false;
	}
	
	_t = new REThreadInternal(this);
	if (_t) 
	{
		const bool r = _t->start();
		if (!r) 
		{
			delete _t;
			_t = NULL;
		}
		return r;
	}
	
	return false;
}

/// Cancels thread
bool REThread::cancel()
{
	if (_t) 
	{
		const bool r = _t->cancel();
		if (r) 
		{
			delete _t;
			_t = NULL;
		}
		return r;
	}
	
	return true;
}

bool REThread::isJoinable() const
{
	return _isJoinable;
}

bool REThread::isDetached() const
{
	return _isDetached;
}

void REThread::setJoinable(bool isJoin)
{
#if defined(HAVE_PTHREAD_H)		
	if (!_t) 
	{
		//TODO: check if possible
		_isJoinable = isJoin;
		if (_isJoinable) 
		{
			_isDetached = false;
		}
	}
#endif	
}

void REThread::setDetached(bool isDetach)
{
#if defined(HAVE_PTHREAD_H)		
	if (!_t) 
	{
		//TODO: check if possible
		_isDetached = isDetach;
		if (_isDetached) 
		{
			_isJoinable = false;
		}
	}
#endif	
}

REThread::REThread() : 
	_t(NULL),
	_isJoinable(false),
	_isDetached(false)
{
//	REThreadsPool::pool();
	
#if defined(HAVE_PTHREAD_H)	
	this->setJoinable(true);
#endif
	
	
#if defined(HAVE_PTHREAD_H)
	if (REThreadInternal::mainPThread == (pthread_t)0)
	{
		REThreadInternal::mainPThread = pthread_self();
	}
#elif defined(__RE_USING_WINDOWS_THREADS__)
	if (REThreadInternal::mainThreadID == 0)
	{
		REThreadInternal::mainThreadID = GetCurrentThreadId();
	}
#endif
}

REThread::~REThread()
{
	this->cancel();
}

uintptr_t REThread::mainThreadIdentifier()
{
	uintptr_t thID = 0;
#if defined(HAVE_PTHREAD_H)
	if (REThreadInternal::mainPThread == (pthread_t)0)
	{
		REThreadInternal::mainPThread = pthread_self();
	}
	thID = ((uintptr_t)REThreadInternal::mainPThread);
#elif defined(__RE_USING_WINDOWS_THREADS__)
	if (REThreadInternal::mainThreadID == 0)
	{
		REThreadInternal::mainThreadID = GetCurrentThreadId();
	}
	thID = ((uintptr_t)REThreadInternal::mainThreadID);
#endif
	return thID;
}

uintptr_t REThread::currentThreadIdentifier()
{
	uintptr_t thID = 0;
#if defined(HAVE_PTHREAD_H)
	thID = ((uintptr_t)pthread_self());
#elif defined(__RE_USING_WINDOWS_THREADS__)
	thID = ((uintptr_t)GetCurrentThreadId());
#endif
	return thID;
}

#include <math.h>

uint64_t __convertSecondsToMicroseconds(const double seconds)
{
	double fullSeconds = 0.0;
	const double fractSecond = modf(seconds, &fullSeconds);
	const uint64_t fullIntMicrosec = (uint64_t)fullSeconds * 1000000;
	const uint64_t fractIntMicrosec = (uint64_t)fractSecond * 1000000;
	return (fullIntMicrosec + fractIntMicrosec);
}

void REThread::uSleep(const uint32_t microseconds)
{
#if defined(__RE_OS_WINDOWS__)
	LARGE_INTEGER time1, time2, sysFreq;
	
	QueryPerformanceCounter(&time1);
	QueryPerformanceFrequency(&sysFreq);
	do
	{
		QueryPerformanceCounter(&time2);
	}
	
	while((time2.QuadPart - time1.QuadPart) < microseconds);
#endif
	
#if defined(HAVE_FUNCTION_USLEEP)
	usleep(microseconds);
#endif
}

void REThread::uSleepInSeconds(const double seconds)
{
	REThread::uSleep((uint32_t)__convertSecondsToMicroseconds(seconds));
}

	bool REThread::isMainThread()
	{
#if defined(HAVE_PTHREAD_H)
		pthread_t curThread = pthread_self();
		if (REThreadInternal::mainPThread)
		{
			return (pthread_equal(curThread, REThreadInternal::mainPThread) != 0);
		}
		else
		{
			REThreadInternal::mainPThread = curThread;
		}
#elif defined(__RE_USING_WINDOWS_THREADS__) 
		const DWORD curTHID = GetCurrentThreadId();
		if (REThreadInternal::mainThreadID)
		{
			return (REThreadInternal::mainThreadID == curTHID);
		}
		else
		{
			REThreadInternal::mainThreadID = curTHID;
		}
#endif	
		return true;
	}
	
}

