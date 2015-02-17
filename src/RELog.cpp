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


#include "../fayecpp.h"

#if defined(HAVE_RECORE_SDK_CONFIG_H) 
#include "recore_sdk_config.h"
#endif

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if defined(HAVE_STDARG_H)
#include <stdarg.h>
#endif

#if defined(HAVE_ANDROID_LOG_H)
#include <android/log.h>
#define LOG_TAG "RECore"
#endif

#if defined(HAVE_QT)
#include <QDebug>
#endif

#include <stdio.h>
#include <time.h>


namespace FayeCpp {
	
	void RELog::log(const char * logString, ...)
	{
		if (logString)
		{
			va_list args;
			va_start(args, logString);
			
#if defined(HAVE_ANDROID_LOG_H)
			__android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, logString, args);
#else
			RELog::logA(logString, args);
#endif
			
			va_end(args);
		}
	}
	
	
	void RELog::logA(const char * logString, va_list arguments)
	{
		if (logString)
		{
#if defined(HAVE_ANDROID_LOG_H)
			__android_log_vprint(ANDROID_LOG_INFO, LOG_TAG, logString, arguments);
#elif defined(HAVE_QT)
            char buff[1024*2] = { 0 };
            vsprintf(buff, logString, arguments);
            qDebug() << buff;
#else
			fprintf(stdout, "\n");
			vfprintf(stdout, logString, arguments);
			fflush(stdout);
#endif
		}
	}
	
}

