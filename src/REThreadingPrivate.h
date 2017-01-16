/*
 *   Copyright (c) 2014 - 2017 Kulykov Oleh <info@resident.name>
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


#ifndef __RETHREADINGPRIVATE_H__
#define __RETHREADINGPRIVATE_H__

#include "../fayecpp.h"

#if defined(HAVE_RECORE_SDK_CONFIG_H)
#include "recore_sdk_config.h"
#endif

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if defined(RE_HAVE_ASSERT_H)
#include <assert.h>
#endif

#if defined(RE_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

#if defined(RE_HAVE_TIME_H)
#include <time.h>
#endif

#if !defined(__RE_HAVE_THREADS__) && defined(__RE_OS_WINDOWS__)
#include <Windows.h>
#define __RE_THREADING_WINDOWS__ 1
#define __RE_HAVE_THREADS__ 1
#endif

#if !defined(__RE_HAVE_THREADS__) && defined(RE_HAVE_PTHREAD_H)
#include <pthread.h>
#define __RE_THREADING_PTHREAD__ 1
#define __RE_HAVE_THREADS__ 1
#endif

namespace FayeCpp {

	class REMutex
	{
	protected:
		void * _m;
	public:
		void lock();
		void unlock();
		REMutex();
		~REMutex();
	};

	class RETime
	{
	public:
		static REUInt32 seconds();
	};
}

#endif /* __RETHREADINGPRIVATE_H__  */

