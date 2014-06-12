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
