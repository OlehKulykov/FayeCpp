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
#include "REStringUtilsPrivate.h"

namespace FayeCpp {
	
	void * REStringBase::stringBuffer() const
	{
		const REBuffer * b = _p;
		return b ? b->buffer() : NULL;
	}
	
	REUInt32 REStringBase::stringBufferSize() const
	{
		const REBuffer * b = _p;
		return b ? b->size() : 0;
	}
	
	REBOOL REStringBase::isEmpty() const
	{
		return ( !REStringUtilsPrivate::isStringExists(_p) );
	}
	
	REBOOL REStringBase::isNotEmpty() const
	{
		return REStringUtilsPrivate::isStringExists(_p);
	}
	
	void REStringBase::clear()
	{
		_p.release();
	}
	
	void REStringBase::setFromUTF8String(const char * utf8String, 
										 const REUInt32 utf8StringLength,
										 const REStringType toType)
	{
		_p.release();
		switch (toType) 
		{
			case REStringTypeUTF8:
				_p = REStringUtilsPrivate::newBuffForUTF8String(utf8String, utf8StringLength);
				break;
				
			case REStringTypeWide:
				_p = REStringUtilsPrivate::getWideFromUTF8(utf8String, utf8StringLength);
				break;
			default:
				break;
		}
	}
	
	void REStringBase::setFromWideString(const wchar_t * wideString, 
										 const REUInt32 wideStringLength,
										 const REStringType toType)
	{
		_p.release();
		switch (toType) 
		{
			case REStringTypeUTF8:
				_p = REStringUtilsPrivate::getUTF8FromWide(wideString, wideStringLength);
				break;
				
			case REStringTypeWide:
				_p = REStringUtilsPrivate::newBuffForWideString(wideString, wideStringLength);
				break;
				
			default:
				break;
		}
	}
	
	bool REStringBase::operator==(const REStringBase & s) const
	{
		return REStringUtilsPrivate::isBuffersEqual(_p, s._p);
	}
	
	REStringBase::REStringBase()
	{
		
	} 
	
	REStringBase::REStringBase(const char * utf8String, 
							   const REUInt32 utf8StringLength,
							   const REStringType toType)
	{
		this->setFromUTF8String(utf8String, utf8StringLength, toType);
	}
	
	REStringBase::REStringBase(const wchar_t * wideString, 
							   const REUInt32 wideStringLength,
							   const REStringType toType)
	{
		this->setFromWideString(wideString, wideStringLength, toType);
	}
	
	REStringBase::REStringBase(const REPtr<REBuffer> & stringBuffer) :
	_p(stringBuffer)
	{
		
	}
	
	REStringBase::~REStringBase()
	{
		_p.release();
	}
	
}

