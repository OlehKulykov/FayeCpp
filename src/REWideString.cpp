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
	
	REString REWideString::string() const
	{
		return REString( REStringUtilsPrivate::getUTF8FromWide(this->wideChars(), 
															   this->length()) );
	}
	
	REMutableString REWideString::mutableString() const
	{
		return REMutableString( REStringUtilsPrivate::getUTF8FromWide(this->wideChars(), 
																	  this->length()) );
	}
	
	REUInt32 REWideString::length() const
	{
		return REStringUtilsPrivate::stringLengthFromWideBuffer(_p);
	}
	
	const wchar_t * REWideString::wideChars() const
	{
		const REBuffer * b = _p;
		return b ? (const wchar_t *)b->buffer() : NULL;
	}
	
	REWideString & REWideString::operator=(const char * utf8String)
	{
		_p = REStringUtilsPrivate::getWideFromUTF8(utf8String, 
												   REStringUtilsPrivate::UTF8StringLength(utf8String));
		return (*this);
	}
	
	REWideString & REWideString::operator=(const wchar_t * wideString)
	{
		_p = REStringUtilsPrivate::newBuffForWideString(wideString);
		return (*this);
	}
	
	REWideString & REWideString::operator=(const REWideString & anotherString)
	{
		_p = anotherString._p;
		return (*this);
	}
	
	REWideString & REWideString::operator=(const REString & anotherString)
	{
		_p = REStringUtilsPrivate::getWideFromUTF8(anotherString.UTF8String(), anotherString.length());
		return (*this);
	}
	
	REWideString & REWideString::operator=(const REMutableString & anotherString)
	{
		_p = REStringUtilsPrivate::getWideFromUTF8(anotherString.UTF8String(), anotherString.length());
		return (*this);
	}
	
	REWideString::REWideString() : REStringBase()
	{
		
	}
	
	REWideString::REWideString(const char * utf8String, 
							   const REUInt32 utf8StringLength) :
	REStringBase(utf8String, utf8StringLength, REStringTypeWide)
	{
		
	}
	
	REWideString::REWideString(const wchar_t * wideString, 
							   const REUInt32 wideStringLength) :
	REStringBase(wideString, wideStringLength, REStringTypeWide)
	{
		
	}
	
	REWideString::REWideString(const REWideString & anotherString) : 
	REStringBase(anotherString._p)
	{
		
	}
	
	REWideString::REWideString(const REString & anotherString) : 
	REStringBase(anotherString.UTF8String(), anotherString.length(), REStringTypeWide)
	{
		
	}
	
	REWideString::REWideString(const REMutableString & anotherString) : 
	REStringBase(anotherString.UTF8String(), anotherString.length(), REStringTypeWide)
	{
		
	}
	
	REWideString::REWideString(const REPtr<REBuffer> & wideStringBuffer) :
	REStringBase(wideStringBuffer)
	{
		
	}
	
	REWideString::~REWideString()
	{
		
	}
	
}

