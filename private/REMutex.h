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


#ifndef __REMUTEX_H__
#define __REMUTEX_H__

#include "../fayecpp.h"

namespace FayeCpp {

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

class REMutexInternal;

/**
 @brief Class of thread mutex.
 @detailed Class of thread mutex.
 */
class REMutex
{
private:
	REMutexInternal * _m;
	
public:
	/**
	 @brief Checks is mutex initialized.
	 @return True if initialized otherwice false.
	 */
	REBOOL isInitialized() const;
	
	/**
	 @brief Initializes mutex with selected type.
	 @param type required mutex type.
	 @return Returns true if successfuly initialized or allready initialized.
	 */
	REBOOL init(const REMutexType type);
	
	/**
	 @brief Locks mutex.
	 @return Returns 'true' if success and 'false' - if fail or not initialized.
	 */
	REBOOL lock();
	
	/**
	 @brief Unlocks mutex.
	 @return Returns 'true' if success and 'false' - if fail or not initialized.
	 */
	REBOOL unlock();
	
	/**
	 @brief Checks is mutex locked.
	 @return Returns value indicated that mutex is successfuly locked.
	 */
	REBOOL isLocked() const;
	
	REMutex();
	virtual ~REMutex();
};

}

#endif /* __REMUTEX_H__ */
