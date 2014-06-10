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


#ifndef __REBUFFER_H__
#define __REBUFFER_H__

#include "../common.h"

namespace FayeCpp {
	
	/// Class of memory buffer.
	class __RE_PUBLIC_CLASS_API__ REBuffer
	{
	protected:
		void * _buff;
		REUInt32 _size;
		int _tag;
		
		static void * defaultMalloc(const REUInt32 size);
		static void defaultFree(void * mem);
		
		virtual void * mallocNewMemory(const REUInt32 size);
		virtual void freeMemory(void * mem);
	public:
		int tag() const;
		void setTag(const int t);
		
		/// Checks is buffer equal with another buffer.
		REBOOL isEqualToBuffer(const REBuffer & anotherBuffer) const;
		
		/// Returns void pointer to memory buffer or NULL.
		void * buffer() const;
		
		/// Returns size of memory byffer.
		REUInt32 size() const;
		
		/// Resizes memory buffer with new size with copying prev data.
		/// Returns true if memory was resized to 'newSize', or false if new memory not created.
		REBOOL resize(const REUInt32 newSize, REBOOL isCopyPrevData);
		
		/// Cleares memory buffer.
		void clear();
		
		/// Set new size of memory buffer, copy from inputed buffer data with size to destination.
		REBOOL set(const void * buff, const REUInt32 buffSize);
		
		/// Setes memory beffer from another buffer.
		REBuffer & operator=(const REBuffer & anotherBuff);
		
		/// Appends new memory with size
		REBOOL append(const void * buff, const REUInt32 buffSize);
		
		/// Appends with another buffer object.
		REBOOL append(const REBuffer & anotherBuff);
		
		/// Appends with another buffer object.
		REBuffer & operator+=(const REBuffer & anotherBuff);
		
		/// Constructs buffer object with content from another buffer object.
		REBuffer(const REBuffer & anotherBuff);
		
		/// Constructs buffer object with content from another buffer with size.
		REBuffer(const void * buff, const REUInt32 buffSize);
		
		/// Constructs buffer object with memory size.
		REBuffer(const REUInt32 buffSize);
		
		/// Constructs empty buffer object.
		REBuffer();
		
		virtual ~REBuffer();
	};
	
}

#endif /* __REBUFFER_H__ */

