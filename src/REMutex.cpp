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
		uint32_t _successfulLocks;
	public:
        bool init(const REMutex::REMutexType type);
		bool lock();
		bool unlock();
		bool isLocked() const;
		REMutexInternal();
		~REMutexInternal();
	};
	
    bool REMutexInternal::init(const REMutex::REMutexType type)
	{
#if defined(HAVE_PTHREAD_H)
		
		pthread_mutexattr_t attr;
		memset(&attr, 0, sizeof(pthread_mutexattr_t));
		if (pthread_mutexattr_init(&attr) == 0)
		{
			int setTypeResult = -1;
			switch (type)
			{
                case REMutex::REMutexTypeNormal:
					setTypeResult = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
					break;
                case REMutex::REMutexTypeRecursive:
					setTypeResult = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
					break;
				default:
					break;
			}
			bool isInit = false;
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
	
	bool REMutexInternal::unlock()
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
	
	bool REMutexInternal::lock()
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
	
	bool REMutexInternal::isLocked() const
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
#if defined(HAVE_PTHREAD_H)
		memset(&_pthreadMutex, 0, sizeof(pthread_mutex_t));
#endif
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
	
	
    bool REMutex::init(const REMutex::REMutexType type)
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
	
	bool REMutex::lock()
	{
		return (_m) ? _m->lock() : false;
	}
	
	bool REMutex::unlock()
	{
		return (_m) ? _m->unlock() : false;
	}
	
	bool REMutex::isLocked() const
	{
		return (_m) ? _m->isLocked() : false;
	}
	
	bool REMutex::isInitialized() const
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

