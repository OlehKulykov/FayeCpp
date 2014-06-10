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


#ifndef __REPTR_H__
#define __REPTR_H__

#include "../fayecpp.h"

namespace FayeCpp {
	
	template <typename PointerType>
	class REPtr
	{
	private:
		PointerType* _object;
		REInt32* _referenceCount;
		
		void retain()
		{
			if (_referenceCount)
			{
				*_referenceCount = (*_referenceCount) + 1;
			}
		}
		
		void deleteObject()
		{
			if (_object)
			{
				delete _object;
			}
			_object = NULL;
		}
		
	public:
		REBOOL isEmpty() const
		{
			return (_object == NULL);
		}
		
		REBOOL isNotEmpty() const
		{
			return (_object != NULL);
		}
		
		REBOOL isSingleOwner() const
		{
			return _referenceCount ? ((*_referenceCount) <= 1) : false;
		}
		
		void release()
		{
			if (_referenceCount)
			{
				*_referenceCount = (*_referenceCount) - 1;
				if ((*_referenceCount) <= 0)
				{
					this->deleteObject();
					
					free(_referenceCount);
					_referenceCount = NULL;
				}
			}
			
			_object = NULL;
			_referenceCount = NULL;
		}
		
		REPtr<PointerType> & operator=(const REPtr<PointerType> & anotherPtr)
		{
			if (this != &anotherPtr)
			{
				this->release();
				if (anotherPtr.isNotEmpty())
				{
					_object = anotherPtr._object;
					_referenceCount = anotherPtr._referenceCount;
					this->retain();
				}
			}
			return (*this);
		}
		
		PointerType* operator->()
		{
			return _object;
		}
		
		const PointerType* operator->() const
		{
			return _object;
		}
		
		operator PointerType* ()
		{
			return _object;
		}
		
		operator const PointerType* () const
		{
			return _object;
		}
		
		REPtr() :
		_object(NULL),
		_referenceCount(NULL)
		{
			
		}
		
		REPtr(PointerType* object) :
		_object(object),
		_referenceCount(NULL)
		{
			if (_object)
			{
				REInt32 * count = (REInt32 *)malloc(sizeof(REInt32 *));
				if (count)
				{
					*count = 1;
					_referenceCount = count;
				}
				else
				{
					_object = NULL;
				}
			}
		}
		
		REPtr(const REPtr<PointerType> & anotherPtr) :
		_object(NULL),
		_referenceCount(NULL)
		{
			if (this != &anotherPtr)
			{
				if (anotherPtr.isNotEmpty())
				{
					_object = anotherPtr._object;
					_referenceCount = anotherPtr._referenceCount;
					this->retain();
				}
			}
		}
		
		~REPtr()
		{
			this->release();
		}
	};
	
	
	template <typename resultType, typename sourceType>
	static resultType* REPtrCast(sourceType* sourcePointer)
	{
		return static_cast<resultType*>( static_cast<void*>(sourcePointer) );
	}
	
	template <typename resultType, typename sourceType>
	static const resultType* REPtrCast(const sourceType* sourcePointer)
	{
		return static_cast<const resultType*>( static_cast<const void*>(sourcePointer) );
	}
	
}

#endif
