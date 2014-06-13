/*
 *   Copyright 2012 - 2014 Kulykov Oleh
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


#include "../fayecpp.h"

namespace FayeCpp {
	
	void REBufferNoCopy::freeMemory(void * mem)
	{
		if (_isNeedToFreeOriginalBuff) 
		{
			_freeOriginalBuff(mem);
			_isNeedToFreeOriginalBuff = false;
		}
		else
		{
			REBuffer::defaultFree(mem);
		}	
	}
	
	REBufferNoCopy::REBufferNoCopy(const void * originalBuff, const REUInt32 buffSize, REBufferNoCopy::FreeOriginalBuff freeOriginalBuff) : REBuffer(),
	_freeOriginalBuff(freeOriginalBuff),
	_isNeedToFreeOriginalBuff(false)
	{
		if (_freeOriginalBuff) 
		{
			if (originalBuff && buffSize) 
			{
				void * ob = const_cast<void *>(originalBuff);
				if (originalBuff == ob) 
				{
					_buff = ob;
					_size = buffSize;
					_isNeedToFreeOriginalBuff = true;
				}
			}
		}
		else
		{
			this->set(originalBuff, buffSize);
		}
	}
	
	REBufferNoCopy::~REBufferNoCopy()
	{
		if (_isNeedToFreeOriginalBuff) 
		{
			_freeOriginalBuff(_buff);
			_isNeedToFreeOriginalBuff = false;
		}
		else
		{
			REBuffer::defaultFree(_buff);
		}
		_buff = NULL;
		_size = 0;
	}
	
}
