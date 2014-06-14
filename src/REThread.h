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


#ifndef __RETHREAD_H__
#define __RETHREAD_H__

#include "../fayecpp.h"

namespace FayeCpp {
	
	// http://locklessinc.com/articles/pthreads_on_windows/
	
	class REThreadInternal;
	
	/// Thread class for aditional work
	class REThread
	{
	private:
		REThreadInternal * _t;
		bool _isTaskFinished;
		
		static void invokeThreadBody(REThread * thread);
		
	protected:
		virtual void threadBody() = 0;
		
	public:
		typedef void(*PerformFunction)(void * userData);
		
		bool isTaskFinished() const;
		
		/// Returns working thread priority. Value in range: [0.0f, 1.0f]
		float priority() const;
		
		/// Setting working thread priority. Value must be in range: [0.0f, 1.0f]
		bool setPriority(const float newPriority);
				
		/// Start thread's work
		bool start();
		
		/// Cancels thread
		bool cancel();
		
		REThread();
		virtual ~REThread();
		
		/// Checking is executing in main thread.
		static bool isMainThread();
		
		/// If at least one aditional thread is created.
		static bool isMultiThreaded();
		
		/// Returns identifier of main thread.
		static uintptr_t mainThreadIdentifier();
		
		/// Returns identifier of current thread. Mean thread from which was called this method.
		static uintptr_t currentThreadIdentifier();
		
		/// Sleps current thread for time in micro seconds.
		static void uSleep(const uint32_t microseconds);
		
		/// Sleps current thread for time in micro seconds.
		static void uSleepInSeconds(const double seconds);
	};
	
}

#endif /* __RETHREAD_H__ */
