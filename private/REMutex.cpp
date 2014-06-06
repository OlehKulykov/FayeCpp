/*
 *   Copyright 2012 - 2013 Kulykov Oleh
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */


#include "REMutex.h"

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
#define __RE_USING_WINDOWS_THREADS__
#endif
#include <Windows.h>
#endif /* __RE_OS_WINDOWS__ */

namespace FayeCpp {
	
	class REMutexInternal
	{
	private:
#if defined(HAVE_PTHREAD_H)
		pthread_mutex_t _pthreadMutex;
#elif defined(__RE_USING_WINDOWS_THREADS__)
		HANDLE _mutexHANDLE;
#endif
		REUInt32 _successfulLocks;
	public:
		REBOOL init(const REMutexType type);
		REBOOL lock();
		REBOOL unlock();
		REBOOL isLocked() const;
		REMutexInternal();
		~REMutexInternal();
	};
	
	REBOOL REMutexInternal::init(const REMutexType type)
	{
#if defined(HAVE_PTHREAD_H)
		
		pthread_mutexattr_t attr = { 0 };
		if (pthread_mutexattr_init(&attr) == 0)
		{
			int setTypeResult = -1;
			switch (type)
			{
				case REMutexTypeNormal:
					setTypeResult = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
					break;
				case REMutexTypeRecursive:
					setTypeResult = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
					break;
				default:
					break;
			}
			REBOOL isInit = false;
			if (setTypeResult == 0)
			{
				if (pthread_mutex_init(&_pthreadMutex, &attr) == 0)
				{
					isInit = true;
				}
			}
			pthread_mutexattr_destroy(&attr);
			return isInit;
		}
		
#elif defined(__RE_USING_WINDOWS_THREADS__)
		
		HANDLE h = CreateMutex(NULL, FALSE, NULL);
		if (h)
		{
			_mutexHANDLE = h;
			return true;
		}
		
#endif
		
		return false;
	}
	
	REBOOL REMutexInternal::unlock()
	{
#if defined(HAVE_PTHREAD_H)
		if (pthread_mutex_unlock(&_pthreadMutex) == 0)
		{
			_successfulLocks--;
			return true;
		}
#elif defined(__RE_USING_WINDOWS_THREADS__)
		if (_mutexHANDLE)
		{
			if (ReleaseMutex(_mutexHANDLE))
			{
				_successfulLocks--;
				return true;
			}
		}
#endif
		return false;
	}
	
	REBOOL REMutexInternal::lock()
	{
#if defined(HAVE_PTHREAD_H)
		if (pthread_mutex_lock(&_pthreadMutex) == 0)
		{
			_successfulLocks++;
			return true;
		}
#elif defined(__RE_USING_WINDOWS_THREADS__)
		if (_mutexHANDLE)
		{
			const DWORD r = WaitForSingleObject(_mutexHANDLE, INFINITE);
			if (r != WAIT_FAILED)
			{
				_successfulLocks++;
				return true;
			}
		}
#endif
		return false;
	}
	
	REBOOL REMutexInternal::isLocked() const
	{
		return (_successfulLocks > 0);
	}
	
	REMutexInternal::REMutexInternal() :
#if defined(HAVE_PTHREAD_H)
	_successfulLocks(0)
#elif defined(__RE_USING_WINDOWS_THREADS__)
	_mutexHANDLE(NULL), _successfulLocks(0)
#else
	_successfulLocks(0)
#endif
	{
		memset(&_pthreadMutex, 0, sizeof(pthread_mutex_t));
	}
	
	REMutexInternal::~REMutexInternal()
	{
#if defined(HAVE_PTHREAD_H)
		pthread_mutex_destroy(&_pthreadMutex);
#elif defined(__RE_USING_WINDOWS_THREADS__)
		if (_mutexHANDLE)
		{
			CloseHandle(_mutexHANDLE);
		}
#endif
	}
	
	
	REBOOL REMutex::init(const REMutexType type)
	{
		if (_m)
		{
			return true;
		}
		
		REMutexInternal * m = new REMutexInternal();
		if (m)
		{
			if (m->init(type))
			{
				_m = m;
				return true;
			}
			delete m;
		}
		return false;
	}
	
	REBOOL REMutex::lock()
	{
		return (_m) ? _m->lock() : false;
	}
	
	REBOOL REMutex::unlock()
	{
		return (_m) ? _m->unlock() : false;
	}
	
	REBOOL REMutex::isLocked() const
	{
		return (_m) ? _m->isLocked() : false;
	}
	
	REBOOL REMutex::isInitialized() const
	{
		return (_m) ? true : false;
	}
	
	REMutex::REMutex() : _m(NULL)
	{
		
	}
	
	REMutex::~REMutex()
	{
		if (_m) 
		{
			delete _m;
		}
	}
	
}

