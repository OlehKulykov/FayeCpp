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

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined(HAVE_DISPATCH_DISPATCH_H)
#include <dispatch/dispatch.h>
#endif

#if !defined(HAVE_FUNCTION_PTHREAD_CANCEL) && defined(HAVE_SIGNAL_H)
#include <signal.h>
#endif

namespace FayeCpp {
	
	typedef void(*REThreadInvokeThreadBodyFunction)(REThread *);
	
	class REThreadInternal
	{
	private:
		bool _isTaskFinished;
	public:
		bool isTaskFinished() const { return _isTaskFinished; }
#if defined(HAVE_PTHREAD_H)
		pthread_t _reThreadThread;
#elif defined(__RE_USING_WINDOWS_THREADS__)
		HANDLE _reThreadThread;
#endif
		REThread * parent;
		REThreadInvokeThreadBodyFunction invokeFunction;
		
		void threadBody()
		{
			REThread * p = parent;
			REThreadInvokeThreadBodyFunction f = invokeFunction;
			if (p && f)
			{
				f(p);
			}
			_isTaskFinished = true;
		}
		
		REThreadInternal(REThread * p) :
		_isTaskFinished(false),
#if defined(HAVE_PTHREAD_H)
		_reThreadThread((pthread_t)0),
#elif defined(__RE_USING_WINDOWS_THREADS__)
		_reThreadThread((HANDLE)0),
#endif
		parent(p),
		invokeFunction(NULL)
		{

	
#if !defined(HAVE_FUNCTION_PTHREAD_CANCEL) && defined(HAVE_PTHREAD_H)
			static bool needSetup = true;
			if (needSetup)
			{
				needSetup = false;
				struct sigaction actions;
				memset(&actions, 0, sizeof(actions));
				sigemptyset(&actions.sa_mask);
				actions.sa_flags = 0;
				actions.sa_handler = thread_exit_handler;
				sigaction(SIGUSR2,&actions,NULL);
			}
#endif


			if (!_internalsMutex.isInitialized())
			{
                _internalsMutex.init(REMutex::REMutexTypeRecursive);
			}
		}
		
		~REThreadInternal()
		{
			this->cancel();
		}
		
		bool start()
		{
#if defined(HAVE_PTHREAD_H)
			if (_reThreadThread) { return false; }
			
			size_t stackSize = 0;
			
#if defined(HAVE_FUNCTION_GETRLIMIT)
			struct rlimit limit = { 0 };
			const int rc = getrlimit(RLIMIT_STACK, &limit);
			if (rc == 0)
			{
				if (limit.rlim_cur != 0) { stackSize = (size_t)limit.rlim_cur; }
			}
#endif
			
			pthread_attr_t threadAttr = { 0 };
			pthread_attr_init(&threadAttr);
			
#if defined( HAVE_FUNCTION_PTHREAD_ATTR_SETSCOPE)
			pthread_attr_setscope(&threadAttr, PTHREAD_SCOPE_SYSTEM);
#endif
			
			pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);
			if (stackSize > 0)
			{
#if defined(HAVE_FUNCTION_PTHREAD_ATTR_SETSTACKSIZE)
				pthread_attr_setstacksize(&threadAttr, stackSize);
#endif
			}
			
			bool isCreated = false;
			pthread_t newThread = (pthread_t)0;
			if (pthread_create(&newThread, &threadAttr, REThreadInternal::threadFunction, REPtrCast<void, REThreadInternal>(this)) == 0)
			{
				isCreated = true;
				REThreadInternal::isMultiThreaded = true;
				_reThreadThread = newThread;
			}
			
			pthread_attr_destroy(&threadAttr);
			return isCreated;
			
#elif defined(__RE_USING_WINDOWS_THREADS__)
			HANDLE hThread = CreateThread(NULL, 0, REThreadInternal::threadProc, REPtrCast<LPVOID, REThreadInternal>(this), 0, NULL);
			if (hThread)
			{
				REThreadInternal::isMultiThreaded = true;
				_reThreadThread = hThread;
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
		
		bool cancel()
		{
#if defined(HAVE_PTHREAD_H)
			if (_reThreadThread)
			{
				bool res = false;
				pthread_t th = _reThreadThread;
				
				pthread_kill(th, SIGUSR2);
				
#if defined(HAVE_FUNCTION_PTHREAD_CANCEL)
				if (pthread_cancel(th) == 0)
#else
					pthread_kill(th, SIGUSR2);
#endif
				{
					void * r = NULL;
					if (pthread_join(th, &r) == 0)
					{
#if defined(HAVE_FUNCTION_PTHREAD_CANCEL)
						res = (r == PTHREAD_CANCELED);
#else
						res = true;
#endif
					}
				}
				_reThreadThread = (pthread_t)0;
				return res;
			}
#elif defined (__RE_USING_WINDOWS_THREADS__)
			if (_reThreadThread)
			{
				bool isAlive = false;
				DWORD dwExitCode = 0;
				if (GetExitCodeThread(_reThreadThread, &dwExitCode))
				{
					// if return code is STILL_ACTIVE,
					// then thread is live.
					isAlive = (dwExitCode == STILL_ACTIVE);
				}
				if (isAlive)
				{
					TerminateThread(_reThreadThread, 0);
				}
				CloseHandle(_reThreadThread);
				_reThreadThread = (HANDLE)0;
				return true;
			}
#endif
			return true;
		}
		
		float priority() const
		{
#if defined(HAVE_PTHREAD_H)
			if (_reThreadThread)
			{
				const int maxPrior = sched_get_priority_max(SCHED_RR);
				if (maxPrior > 0)
				{
					struct sched_param p = { 0 };
					int policy = SCHED_RR;
					pthread_t thisThread = _reThreadThread;
					if ( pthread_getschedparam(thisThread, &policy, &p) == 0 )
					{
						return ((float)p.sched_priority / (float)maxPrior);
					}
				}
			}
#elif defined(__RE_USING_WINDOWS_THREADS__)
			if (_reThreadThread)
			{
				const int prior = GetThreadPriority(_reThreadThread);
				const int minPriority = MIN(THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL);
				const int range = MAX(THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL) - minPriority;
				return ( (float)(prior - minPriority) / (float)range );
			}
#endif
			return 0.0f;
		}
		
		bool setPriority(const float newPriority)
		{
			if ((newPriority <= 0.0f) || (newPriority > 1.0f)) return false;
#if defined(HAVE_PTHREAD_H)
			if (_reThreadThread)
			{
				const int maxPrior = sched_get_priority_max(SCHED_RR);
				if (maxPrior > 0)
				{
					struct sched_param p = { 0 };
					p.sched_priority = (int)(newPriority * maxPrior);
					pthread_t thisThread = _reThreadThread;
					return ( pthread_setschedparam(thisThread, SCHED_RR, &p) == 0 );
				}
			}
#elif defined(__RE_USING_WINDOWS_THREADS__)
			if (_reThreadThread)
			{
				const int minPriority = MIN(THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL);
				const int range = MAX(THREAD_PRIORITY_IDLE, THREAD_PRIORITY_TIME_CRITICAL) - minPriority;
				const int prior = (int)(minPriority + (newPriority * (float)range));
				if (SetThreadPriority(_reThreadThread, prior))
				{
					return true;
				}
			}
#endif
			return false;
		}
		
		
#if defined(HAVE_PTHREAD_H)
		static pthread_t mainPThread;
		static void * threadFunction(void * th);
#elif defined(__RE_USING_WINDOWS_THREADS__)
		static DWORD mainThreadID;
		static DWORD WINAPI threadProc(LPVOID lpParameter);
#endif
		static bool isMultiThreaded;
		static void onDone(REThreadInternal * t);
		
	private:
		static REList<REThreadInternal *> _internalsList;
		static REMutex _internalsMutex;
	};
	
	REList<REThreadInternal *> REThreadInternal::_internalsList;
	REMutex REThreadInternal::_internalsMutex;
	
	void REThreadInternal::onDone(REThreadInternal * t)
	{
		_internalsMutex.lock();
		REList<REThreadInternal *>::Iterator iter = _internalsList.iterator();
		while (iter.next())
		{
			REThreadInternal * ti = iter.value();
			if (ti->isTaskFinished())
			{
				if (_internalsList.removeNode(iter.node()))
				{
					ti->cancel();
					delete ti;
				}
			}
		}
		_internalsList.add(t);
		_internalsMutex.unlock();
	}
	
#if defined(HAVE_PTHREAD_H)
	pthread_t REThreadInternal::mainPThread = (pthread_t)0;
	void * REThreadInternal::threadFunction(void * th)
	{
		if (th)
		{
			REThreadInternal * reThread = REPtrCast<REThreadInternal, void>(th);
			reThread->threadBody();
		}
		return th;
	}
#elif defined(__RE_USING_WINDOWS_THREADS__)
	DWORD REThreadInternal::mainThreadID = 0;
	DWORD REThreadInternal::threadProc(LPVOID lpParameter)
	{
		if (lpParameter)
		{
			REThreadInternal * reThread = REPtrCast<REThreadInternal, void>(lpParameter);
			reThread->threadBody();
		}
		return 0;
	}
#endif
	bool REThreadInternal::isMultiThreaded = false;
	
	
	bool REThread::start()
	{
		if (_t)
		{
			return true;
		}
		else
		{
			_t = new REThreadInternal(this);
			if (_t)
			{
				_t->invokeFunction = &REThread::invokeThreadBody;
				return _t->start();
			}
		}
		return false;
	}
	
	bool REThread::cancel()
	{
		if (_t)
		{
			const bool r = _t->cancel();
			delete _t;
			_t = NULL;
			return r;
		}
		return true;
	}
	
	bool REThread::isTaskFinished() const
	{
		return _isTaskFinished;
	}
	
	float REThread::priority() const
	{
		return (_t) ? _t->priority() : 0.0f;
	}
	
	bool REThread::setPriority(const float newPriority)
	{
		return (_t) ? _t->setPriority(newPriority) : false;
	}
	
	void REThread::invokeThreadBody(REThread * thread)
	{
		if (thread)
		{
			REThreadInternal * ti = thread->_t;
			thread->_t = NULL;
			
			ti->parent = NULL;
			ti->invokeFunction = NULL;
			
			thread->threadBody();
			thread->_isTaskFinished = true;
			REThreadInternal::onDone(ti);
		}
	}
	
	REThread::REThread() :
		_t(NULL),
		_isTaskFinished(false)
	{
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
	
	bool REThread::isMultiThreaded()
	{
		return REThreadInternal::isMultiThreaded;
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
	
#include <math.h>
	
	const uint64_t __convertSecondsToMicroseconds(const double seconds)
	{
		double fullSeconds = 0.0;
		const double fractSecond = modf(seconds, &fullSeconds);
		const uint64_t fullIntMicrosec = (uint64_t)fullSeconds * 1000000;
		const uint64_t fractIntMicrosec = (uint64_t)fractSecond * 1000000;
		return (fullIntMicrosec + fractIntMicrosec);
	}
	
	/// Sleps current thread for time in micro seconds.
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
