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


#include "REBuffer.h"

namespace FayeCpp {
	

	int REBuffer::tag() const
	{
		return _tag;
	}
	
	void REBuffer::setTag(const int t)
	{
		_tag = t;
	}
	
	void * REBuffer::defaultMalloc(const uint32_t size)
	{
		return malloc((size_t)size);
	}
	
	void REBuffer::defaultFree(void * mem)
	{
		free(mem);
	}
	
	void * REBuffer::mallocNewMemory(const uint32_t size)
	{
		return REBuffer::defaultMalloc(size);
	}
	
	void REBuffer::freeMemory(void * mem)
	{
		REBuffer::defaultFree(mem);
	}
	
	bool REBuffer::isEqualToBuffer(const REBuffer & anotherBuffer) const
	{
		if (_size == anotherBuffer._size)
		{
			return _size ? (memcmp(_buff, anotherBuffer._buff, _size) == 0) : true;
		}
		return false;
	}
	
	void * REBuffer::buffer() const
	{
		return _buff;
	}
	
	uint32_t REBuffer::size() const
	{
		return _size;
	}
	
	bool REBuffer::resize(const uint32_t newSize, bool isCopyPrevData)
	{
		if (_size == newSize)
		{
			return true;
		}
		
		void * newBuff = this->mallocNewMemory(newSize);
		if (newBuff)
		{
			if (_buff)
			{
				if (isCopyPrevData)
				{
					const uint32_t copySize = MIN(newSize, _size);
					if (copySize)
					{
						memcpy(newBuff, _buff, (size_t)copySize);
					}
				}
				
				this->freeMemory(_buff);
			}
			
			_buff = newBuff;
			_size = newSize;
			
			return true;
		}
		
		return false;
	}
	
	void REBuffer::clear()
	{
		if (_buff)
		{
			this->freeMemory(_buff);
			_buff = NULL;
			_size = 0;
		}
	}
	
	bool REBuffer::set(const void * buff, const uint32_t buffSize)
	{
		this->clear();
		
		if (buff && buffSize)
		{
			void * newBuff = this->mallocNewMemory(buffSize);
			if (newBuff)
			{
				memcpy(newBuff, buff, (size_t)buffSize);
				_buff = newBuff;
				_size = buffSize;
				return true;
			}
		}
		
		return false;
	}
	
	bool REBuffer::append(const void * buff, const uint32_t buffSize)
	{
		if (_size && _buff)
		{
			const uint32_t newSize = _size + buffSize;
			char * newBuff = (char *)this->mallocNewMemory(newSize);
			if (newBuff)
			{
				memcpy(newBuff, _buff, (size_t)_size);
				memcpy(&newBuff[_size], buff, (size_t)buffSize);
				this->freeMemory(_buff);
				_buff = newBuff;
				_size = newSize;
				return true;
			}
		}
		
		return this->set(buff, buffSize);
	}
	
	bool REBuffer::append(const REBuffer & anotherBuff)
	{
		return this->append(anotherBuff.buffer(), anotherBuff.size());
	}
	
	REBuffer & REBuffer::operator+=(const REBuffer & anotherBuff)
	{
		this->append(anotherBuff.buffer(), anotherBuff.size());
		return (*this);
	}
	
	REBuffer & REBuffer::operator=(const REBuffer & anotherBuff)
	{
		this->set(anotherBuff._buff, anotherBuff._size);
		return (*this);
	}
	
	REBuffer::REBuffer(const REBuffer & anotherBuff) :
		_buff(NULL),
		_size(0),
		_tag(0)
	{
		this->set(anotherBuff._buff, anotherBuff._size);
	}
	
	REBuffer::REBuffer(const void * buff, const uint32_t buffSize) :
		_buff(NULL),
		_size(0),
		_tag(0)
	{
		this->set(buff, buffSize);
	}
	
	REBuffer::REBuffer(const uint32_t buffSize) :
		_buff(NULL),
		_size(0),
		_tag(0)
	{
		if (buffSize)
		{
			void * newBuff = this->mallocNewMemory(buffSize);
			if (newBuff)
			{
				_buff = newBuff;
				_size = buffSize;
			}
		}
	}
	
	REBuffer::REBuffer():
		_buff(NULL),
		_size(0),
		_tag(0)
	{
		
	}
	
	REBuffer::~REBuffer()
	{
		if (_buff) 
		{
			REBuffer::defaultFree(_buff);
			_buff = NULL;
			_size = 0;
		}
	}
	
	
}




