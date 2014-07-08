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


#include "../fayecpp.h"

#if defined(HAVE_RECORE_SDK_CONFIG_H) 
#include "recore_sdk_config.h"
#endif

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if defined(HAVE_MACH_MACH_TIME_H)
#include <mach/mach_time.h>
#endif

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

#if defined(__RE_OS_WINDOWS__)
#include <windows.h>
#endif


namespace FayeCpp {
	
	
	class RETimePrivate 
	{
	private:
#if defined(HAVE_MACH_MACH_TIME_H)
		
		uint64_t _absoluteAppStartTime;
		uint64_t _absoluteLastCallTime;
		double _nanoSecond;
		
#elif defined(__RE_OS_WINDOWS__) 
		
		INT64 _performanceCounterFrequency;
		double _performanceCounterTimeScale;
		
#endif	
		
		RETimeInterval _lastPauseTime;
		RETimeInterval _unusedTime;
		REUInt32 _isPaused;
		RETimeInterval get();
		void updatePauseTime();
	public:
		static RETimePrivate _time;
		RETimePrivate();
		~RETimePrivate();
		RETimeInterval Time();
		REBOOL pause();
		REBOOL resume();
		REBOOL isPaused() const;
	};
	
	RETimePrivate RETimePrivate::_time;
	
	RETimeInterval RETimePrivate::get() 
	{
#if defined(HAVE_MACH_MACH_TIME_H)
		
		const double seconds = (_nanoSecond * (mach_absolute_time() - _absoluteAppStartTime));
		return (RETimeInterval)seconds;
		
#elif defined(__RE_OS_WINDOWS__) 
		
		INT64 counterTime = 0;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&counterTime));
		const double returnTime = ((double)counterTime) * _performanceCounterTimeScale;
		return (RETimeInterval)returnTime;
		
#elif defined(HAVE_FUNCTION_CLOCK_GETTIME)
		
		struct timespec now;
		if (clock_gettime(CLOCK_MONOTONIC, &now) == 0)
		{
			return (RETimeInterval)(((double)now.tv_nsec * 1e-9) + now.tv_sec);
		}
		else
		{
			return (RETimeInterval)((double)clock() / (double)CLOCKS_PER_SEC);
		}
		
#else
		
		return (RETimeInterval)((double)clock() / (double)CLOCKS_PER_SEC);
		
#endif
		
	}
	
	void RETimePrivate::updatePauseTime()
	{
		const RETimeInterval curTime = this->get();
		_unusedTime += (curTime - _lastPauseTime);
		_lastPauseTime = curTime;
	}
	
	RETimeInterval RETimePrivate::Time()
	{
		if (_isPaused) 
		{
			this->updatePauseTime();
		}
		
		return (this->get() - _unusedTime);
	}
	
	REBOOL RETimePrivate::pause()
	{
		if (_isPaused == 0)
		{
			_isPaused = 1;
			_lastPauseTime = this->get();
			return true;
		}
		return false;
	}
	
	REBOOL RETimePrivate::resume()
	{
		if (_isPaused) 
		{
			this->updatePauseTime();
			_isPaused = 0;
			return true;
		}
		return false;
	}
	
	REBOOL RETimePrivate::isPaused() const
	{
		return ((_isPaused == 1) ? true : false);
	}
	
	RETimePrivate::RETimePrivate() :
#if defined(HAVE_MACH_MACH_TIME_H) 
	
	_absoluteAppStartTime(0),
	_absoluteLastCallTime(0),
	_nanoSecond((double)0),
	
#elif defined(__RE_OS_WINDOWS__) 
	
	_performanceCounterFrequency(0),
	_performanceCounterTimeScale((double)0),
	
#endif	
	
	_lastPauseTime(0.0),
	_unusedTime(0.0),
	_isPaused(0)
	{
#if defined(HAVE_MACH_MACH_TIME_H) 
		
		_absoluteAppStartTime = mach_absolute_time();
		_absoluteLastCallTime = _absoluteAppStartTime;
		mach_timebase_info_data_t info = { 0 };
		mach_timebase_info(&info);
		_nanoSecond = (double)(((long double)info.numer) * 1e-9)  / ((long double)info.denom);
		
#elif defined(__RE_OS_WINDOWS__) 
		
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_performanceCounterFrequency));
		_performanceCounterTimeScale = ((long double)1.0) / ((long double)_performanceCounterFrequency);
		
#endif	
		
		_unusedTime = this->get();
	}
	
	RETimePrivate::~RETimePrivate()
	{
		
	}
	
	REUInt64 RETime::convertSecondsToMilliseconds(const RETimeInterval seconds)
	{
		double fullSeconds = 0.0;
		const double fractSecond = modf(seconds, &fullSeconds);
		const REUInt64 fullIntMilisec = (REUInt64)fullSeconds * 1000;
		const REUInt64 fractIntMilisec = (REUInt64)fractSecond * 1000;
		return (fullIntMilisec + fractIntMilisec);
	}
	
	REUInt64 RETime::convertSecondsToMicroseconds(const RETimeInterval seconds)
	{
		double fullSeconds = 0.0;
		const double fractSecond = modf(seconds, &fullSeconds);
		const REUInt64 fullIntMicrosec = (REUInt64)fullSeconds * 1000000;
		const REUInt64 fractIntMicrosec = (REUInt64)fractSecond * 1000000;
		return (fullIntMicrosec + fractIntMicrosec);
	}
	
	REBOOL RETime::pauseTime()
	{
		return RETime::pause();
	}
	
	REBOOL RETime::resumeTime()
	{
		return RETime::resume();
	}
	
	REBOOL RETime::pause()
	{
		return RETimePrivate::_time.pause();
	}
	
	REBOOL RETime::resume()
	{
		return RETimePrivate::_time.resume();
	}
	
	REBOOL RETime::isTimePaused() const
	{
		return RETime::isPaused();
	}
	
	REBOOL RETime::isPaused()
	{
		return RETimePrivate::_time.isPaused();
	}
	
	RETimeInterval RETime::getTime() const
	{
		return RETime::time();
	}
	
	RETimeInterval RETime::time()
	{
		return RETimePrivate::_time.Time();
	}
	
}

