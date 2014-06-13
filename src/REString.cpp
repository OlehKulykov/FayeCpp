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

#if defined(HAVE_RECORE_SDK_CONFIG_H) 
#include "recore_sdk_config.h"
#endif

#if defined(HAVE_STDARG_H)
#include <stdarg.h>
#endif

namespace FayeCpp {
	
	REString & REString::operator=(const char * utf8String)
	{
		this->setFromUTF8String(utf8String,
								RENotFound,
								REStringTypeUTF8);
		
		return (*this);
	}
	
	REString & REString::operator=(const wchar_t * wideString)
	{
		this->setFromWideString(wideString, 
								RENotFound,
								REStringTypeUTF8);
		
		return (*this);
	}
	
	REString & REString::operator=(const REWideString & anotherString)
	{
		this->setFromWideString(anotherString.wideChars(), 
								anotherString.length(), 
								REStringTypeUTF8);
		return (*this);
	}
	
	REString & REString::operator=(const REString & anotherString)
	{
		_p = anotherString._p;
		return (*this);
	}
	
	REString & REString::operator=(const REMutableString & anotherString)
	{
		_p = REStringUtilsPrivate::makeCopy(anotherString._p);
		return (*this);
	}
	
	bool REString::operator==(const REString & s) const
	{
		return this->isEqual(s); 
	}
	
	REMutableString REString::mutableString() const
	{
		return REMutableString( REStringUtilsPrivate::makeCopy(_p) );
	}
	
	REWideString REString::wideString() const
	{
		return REWideString( REStringUtilsPrivate::getWideFromUTF8(_p) );
	}
	
	const char * REString::UTF8String() const
	{
		const REBuffer * b = _p;
		return b ? (const char *)b->buffer() : NULL;
	}
	
	REUInt32 REString::length() const
	{
		return REStringUtilsPrivate::stringLengthFromUTF8Buffer(_p);
	}
	
	REBOOL REString::isContainsNonASCII() const
	{
		const char * ch = (const char *)this->UTF8String();
		if (ch)
		{
			while (*ch) 
			{
				const wchar_t u = (*ch);
				if ( u >= ((char)0x80) )
				{
					return true;
				}
				ch++;
			}
		}
		return false;
	}
	
	REBOOL REString::isContaines(const char * utf8String) const
	{
		if (utf8String)
		{
			const char * thisUTF8String = this->UTF8String();
			if (thisUTF8String)
			{
				return (strstr(thisUTF8String, utf8String) != 0);
			}
		}
		return false;
	}
	
	REBOOL REString::isContaines(const wchar_t * wideString) const
	{
		if (wideString)
		{
			REPtr<REBuffer> utf8Buff = REStringUtilsPrivate::getUTF8FromWide(wideString, 
																			 RENotFound);
			
			const REUInt32 len = REStringUtilsPrivate::stringLengthFromUTF8Buffer(utf8Buff);
			if (len)
			{
				return this->isContaines((const char *)utf8Buff->buffer());
			}
		}
		return false;
	}
	
	REBOOL REString::isDigit() const
	{
		const char * ch = this->UTF8String();
		if (ch)
		{
			REUInt32 processed = 0;
			while (*ch)
			{
				const int dig = *ch++;
				processed++;
				if (!isdigit(dig))
				{
					if ((dig != '+') || (dig != '-') || (dig != '.'))
					{
						return false;
					}
				}
			}
			return (processed > 0);
		}
		return false;
	}
	
	REBOOL REString::isEqual(const REWideString & anotherString) const
	{
		REString utf8String(anotherString);
		return REStringUtilsPrivate::isBuffersEqual(_p, utf8String._p);
	}
	
	REBOOL REString::isEqual(const REString & anotherString) const
	{
		return REStringUtilsPrivate::isBuffersEqual(_p, anotherString._p);
	}
	
	REBOOL REString::isEqual(const REMutableString & anotherString) const
	{
		return REStringUtilsPrivate::isBuffersEqual(_p, anotherString._p);
	}
	
	REBOOL REString::isEqual(const char * utf8String, 
							 const REUInt32 utf8StringLength) const
	{
		const REUInt32 len = REStringUtilsPrivate::actualUTF8StringLength(utf8String, utf8StringLength);
		if (len == this->length()) 
		{
			if (len > 0) 
			{
				return (memcmp(this->UTF8String(), utf8String, len) == 0);
			}
			return true;
		}
		return false;
	}
	
	REBOOL REString::isEqual(const wchar_t * wideString, 
							 const REUInt32 wideStringLength) const
	{
		REPtr<REBuffer> p = REStringUtilsPrivate::getUTF8FromWide(wideString, wideStringLength);
		return REStringUtilsPrivate::isBuffersEqual(_p, p);
	}
	
	REString REString::pathExtension() const
	{
		return REString( REStringUtilsPrivate::getPathExtension(_p) );
	}
	
	REInt64 REString::integerValue(REBOOL * isOk) const
	{
		REInt64 value = 0;
		REBOOL reslt = false;
		const char * str = (const char *)this->stringBuffer();
		if (str && (this->length() > 0)) 
		{
			reslt = REStringUtilsPrivate::readFirstNumber(str, &value, NULL);
		}
		if (isOk) *isOk = reslt;
		return value;
	}
	
	REFloat64 REString::floatValue(REBOOL * isOk) const
	{
		REFloat64 value = 0.0;
		REBOOL reslt = false;
		const char * str = (const char *)this->stringBuffer();
		if (str && (this->length() > 0)) 
		{
			reslt = REStringUtilsPrivate::readFirstNumber(str, NULL, &value);
		}
		if (isOk) *isOk = reslt;
		return value;
	}
	
	REString::REString() : REStringBase()
	{
		
	}
	
	REString::REString(const wchar_t * wideString, const REUInt32 wideStringLength) :
	REStringBase(wideString, wideStringLength, REStringTypeUTF8)
	{
		
	}
	
	//REString::REString(const wchar_t * wideString, const RERange & range) : REStringBase()
	//{
	//	if (wideString && range.isValid()) 
	//	{
	//		const wchar_t * s = wideString + range.location;
	//		this->setFromWideString(s, range.length, REStringTypeWide);
	//	}
	//}
	//
	//REString::REString(const char * utf8String, const RERange & range) : REStringBase()
	//{
	//	if (utf8String && range.isValid())
	//	{
	//		const char * s = utf8String + range.location;
	//		this->setFromUTF8String(s, range.length, REStringTypeUTF8);
	//	}
	//}
	
	REString::REString(const char * utf8String, const REUInt32 utf8StringLength) :
	REStringBase(utf8String, utf8StringLength, REStringTypeUTF8)
	{
		
	}
	
	REString::REString(const REWideString & anotherString) : 
	REStringBase(anotherString.wideChars(), anotherString.length(), REStringTypeUTF8)
	{
		
	}
	
	
	REString::REString(const REString & anotherString) : 
	REStringBase(anotherString._p)
	{
		
	}
	
	REString::REString(const REMutableString & anotherString) :
	REStringBase(anotherString.UTF8String(), anotherString.length(), REStringTypeUTF8)
	{
		
	}
	
	REString::REString(const REPtr<REBuffer> & utf8StringBuffer) :
	REStringBase(utf8StringBuffer)
	{
		
	}
	
	REString::~REString()
	{
		
	}
	
	REString REString::createWithFormat(const char * format, ...)
	{
		if (format)
		{
			va_list args;
			va_start(args, format);
			char strBuff[1024];
			
#if defined(HAVE_FUNCTION_VSNPRINTF)		
			const int writed = vsnprintf(strBuff, 1024, format, args);
#elif defined(HAVE_FUNCTION_VSPRINTF_S)		
			const int writed = vsprintf_s(strBuff, format, args);
#else		
			const int writed = vsprintf(strBuff, format, args);
#endif		
			va_end(args);
			if (writed > 0)
			{
				strBuff[writed] = 0;
				return REString(strBuff, (REUInt32)writed);
			}
		}
		return REString();
	}
	
}


