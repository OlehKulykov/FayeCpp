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


#ifndef __REMUTEX_H__
#define __REMUTEX_H__

#include "../fayecpp.h"

namespace FayeCpp {
	
	class REMutexInternal;
	
	/**
	 @brief Class of thread mutex.
	 @detailed Class of thread mutex.
	 */
	class REMutex
	{
	public:
		/**
		 @brief Enums type of mutexes.
		 @detailed Enums type of mutexes.
		 */
		typedef enum _reMutexType
		{
			/**
			 @brief Mutex is not initialized.
			 @detailed Mutex is not initialized.
			 */
			REMutexTypeNone = 0,
			
			/**
			 @brief Normal mutex type.
			 @detailed This type of mutex does not detect deadlock.
			 An attempt to relock this mutex without first unlocking it deadlocks.
			 Attempting to unlock a mutex locked by a different thread results in undefined behavior.
			 Attempting to unlock an unlocked mutex results in undefined behavior.
			 */
			REMutexTypeNormal = 1,
			
			/**
			 @brief Recursive mutex type.
			 @detailed A thread attempting to relock this mutex without first unlocking it succeeds in locking the mutex.
			 The relocking deadlock that can occur with mutexes of type NORMAL cannot occur with this type of mutex.
			 Multiple locks of this mutex require the same number of unlocks to release the mutex before another thread can acquire the mutex.
			 An attempt to unlock a mutex that another thread has locked returns with an error.
			 An attempt to unlock an unlocked mutex returns with an error.
			 */
			REMutexTypeRecursive = 2
			
		}
		/**
		 @brief Enums type of mutexes.
		 @detailed Enums type of mutexes.
		 */
		REMutexType;
		
	private:
		REMutexInternal * _m;
		
	public:
		/**
		 @brief Checks is mutex initialized.
		 @return True if initialized otherwice false.
		 */
		bool isInitialized() const;
		
		/**
		 @brief Initializes mutex with selected type.
		 @param type required mutex type.
		 @return Returns true if successfuly initialized or allready initialized.
		 */
		bool init(const REMutex::REMutexType type);
		
		/**
		 @brief Locks mutex.
		 @return Returns 'true' if success and 'false' - if fail or not initialized.
		 */
		bool lock();
		
		/**
		 @brief Unlocks mutex.
		 @return Returns 'true' if success and 'false' - if fail or not initialized.
		 */
		bool unlock();
		
		/**
		 @brief Checks is mutex locked.
		 @return Returns value indicated that mutex is successfuly locked.
		 */
		bool isLocked() const;
		
		REMutex();
		virtual ~REMutex();
	};
	
}

#endif /* __REMUTEX_H__ */
