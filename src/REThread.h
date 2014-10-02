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

class REThreadInternal;

class REThread 
{
private:
	REThreadInternal * _t;
	bool _isJoinable;
	bool _isDetached;
	
public:
	bool isCancelled() const;
	
	bool isJoinable() const;
	bool isDetached() const;
		
	void setJoinable(bool isJoin);
	void setDetached(bool isDetach);
	
	virtual void threadBody() = 0;
	
	/// Start thread's work
	bool start();
	
	/// Cancels thread
	bool cancel();
	
	/// Checking is executing in main thread.
	static bool isMainThread();
	
	/// Returns identifier of main thread.
	static uintptr_t mainThreadIdentifier();
	
	/// Returns identifier of current thread. Mean thread from which was called this method.
	static uintptr_t currentThreadIdentifier();
	
	/// Sleps current thread for time in micro seconds.
	static void uSleep(const uint32_t microseconds);
	
	/// Sleps current thread for time in micro seconds.
	static void uSleepInSeconds(const double seconds);
	
	REThread();
	virtual ~REThread();
};

}

#endif /* __RETHREAD_H__ */
