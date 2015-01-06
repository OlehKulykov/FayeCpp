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

namespace FayeCpp {
	
	
	void * REBuffer::defaultMalloc(const REUInt32 size)
	{
		return malloc((size_t)size);
	}
	
	void REBuffer::defaultFree(void * mem)
	{
		free(mem);
	}
	
	void * REBuffer::mallocNewMemory(const REUInt32 size)
	{
		return REBuffer::defaultMalloc(size);
	}
	
	void REBuffer::freeMemory(void * mem)
	{
		REBuffer::defaultFree(mem);
	}
	
	REBOOL REBuffer::isEqualToBuffer(const REBuffer & anotherBuffer) const
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
	
	REUInt32 REBuffer::size() const
	{
		return _size;
	}
	
	REBOOL REBuffer::resize(const REUInt32 newSize, REBOOL isCopyPrevData)
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
					const REUInt32 copySize = MIN(newSize, _size);
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
	
	REBOOL REBuffer::set(const void * buff, const REUInt32 buffSize)
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
	
	REBOOL REBuffer::append(const void * buff, const REUInt32 buffSize)
	{
		if (_size && _buff)
		{
			const REUInt32 newSize = _size + buffSize;
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
			return false;
		}
		
		return this->set(buff, buffSize);
	}
	
	REBOOL REBuffer::append(const REBuffer & anotherBuff)
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
	_size(0)
	{
		this->set(anotherBuff._buff, anotherBuff._size);
	}
	
	REBuffer::REBuffer(const void * buff, const REUInt32 buffSize) : 
	_buff(NULL), 
	_size(0)
	{
		this->set(buff, buffSize);
	}
	
	REBuffer::REBuffer(const REUInt32 buffSize) : 
	_buff(NULL), 
	_size(0)
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
	_size(0)
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


