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


#ifndef __RETHREAD_H__
#define __RETHREAD_H__

#include "../common.h"
#include "REPtr.h"

namespace FayeCpp {
	
	// http://locklessinc.com/articles/pthreads_on_windows/
	
	class REThreadInternal;
	
	/// Thread class for aditional work
	class __RE_PUBLIC_CLASS_API__ REThread
	{
	private:
		REThreadInternal * _t;
		REBOOL _isTaskFinished;
		
		static void invokeThreadBody(REThread * thread);
		
	protected:
		virtual void threadBody() = 0;
		
	public:
		typedef void(*PerformFunction)(void * userData);
		
		REBOOL isTaskFinished() const;
		
		/// Returns working thread priority. Value in range: [0.0f, 1.0f]
		REFloat32 priority() const;
		
		/// Setting working thread priority. Value must be in range: [0.0f, 1.0f]
		REBOOL setPriority(const REFloat32 newPriority);
				
		/// Start thread's work
		REBOOL start();
		
		/// Cancels thread
		REBOOL cancel();
		
		REThread();
		virtual ~REThread();
		
		/// Checking is executing in main thread.
		static REBOOL isMainThread();
		
		/// If at least one aditional thread is created.
		static REBOOL isMultiThreaded();
		
		/// Returns identifier of main thread.
		static REUIdentifier mainThreadIdentifier();
		
		/// Returns identifier of current thread. Mean thread from which was called this method.
		static REUIdentifier currentThreadIdentifier();
		
		/// Sleps current thread for time in micro seconds.
		static void uSleep(const REUInt32 microseconds);
		
		/// Sleps current thread for time in micro seconds.
		static void uSleepInSeconds(const RETimeInterval seconds);
	};
	
}

#endif /* __RETHREAD_H__ */
