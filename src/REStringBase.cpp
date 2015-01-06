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

