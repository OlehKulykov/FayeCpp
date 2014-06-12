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


#ifndef __RESTRINGUTILSPRIVATE_H__ 
#define __RESTRINGUTILSPRIVATE_H__ 

#include "../fayecpp.h"

namespace FayeCpp {

class REStringUtilsPrivate
{
private:
	static int charsToWide(const char * charsString, int charsStringLengthOrNegative, wchar_t * wideString);
	
	static int wideToChars(const wchar_t * wideString, int wideStringLength, char * charsString);
	
	static REBuffer * newBufferWithSize(const REUInt32 newSize);
public:
	static REPtr<REBuffer> newBuffForUTF8String(const char * utf8String, const REUInt32 utf8StringLength = RENotFound);
	
	static REPtr<REBuffer> newBuffForWideString(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound);
	
	static REPtr<REBuffer> getWideFromUTF8(const REPtr<REBuffer> & utf8StringBuffer);
	
	static REPtr<REBuffer> getUTF8FromWide(const REPtr<REBuffer> & wideStringBuffer);
	
	static REPtr<REBuffer> getWideFromUTF8(const char * utf8String, const REUInt32 utf8StringLength);
	
	static REPtr<REBuffer> getUTF8FromWide(const wchar_t * wideString, const REUInt32 wideStringLength);
	
	static REPtr<REBuffer> makeCopy(const REPtr<REBuffer> & sourceBuffer);
	
	static REUInt32 UTF8StringLength(const char * utf8String)
	{
		return utf8String ? (REUInt32)strlen(utf8String) : 0;
	}
	
	static REUInt32 wideStringLength(const wchar_t * wideString)
	{
		return wideString ? (REUInt32)wcslen(wideString) : 0;
	}
	
	static REBOOL isStringExists(const REPtr<REBuffer> & stringBuffer)
	{
		if (stringBuffer.isNotEmpty())
		{
			return stringBuffer->buffer() ? true : false;
		}
		return false;
	}
	
	static REUInt32 actualUTF8StringLength(const char * utf8String, const REUInt32 utf8StringLength = RENotFound)
	{
		const REUInt32 len = (utf8StringLength == RENotFound) ? REStringUtilsPrivate::UTF8StringLength(utf8String) : utf8StringLength;
		return ( (len > 0) && (len != RENotFound) ) ? len : 0;
	}
	
	static REUInt32 actualWideStringLength(const wchar_t * wideString, const REUInt32 wideStringLength = RENotFound)
	{
		const REUInt32 len = (wideStringLength == RENotFound) ? REStringUtilsPrivate::wideStringLength(wideString) : wideStringLength;
		return ( (len > 0) && (len != RENotFound) ) ? len : 0;
	}
	
	static REUInt32 stringLengthFromUTF8Buffer(const REPtr<REBuffer> & utf8StringBuffer);
	
	static REUInt32 stringLengthFromWideBuffer(const REPtr<REBuffer> & wideStringBuffer);
	
	static REPtr<REBuffer> getAppendedWithPathComponent(const REPtr<REBuffer> & utf8Buffer, 
														const char * comp);
	
	static REPtr<REBuffer> getRemovedPathExtension(const REPtr<REBuffer> & utf8Buffer);
	
	static REPtr<REBuffer> getPathExtension(const REPtr<REBuffer> & utf8Buffer);
	
	static REPtr<REBuffer> getRemovedLastPathComponent(const REPtr<REBuffer> & utf8Buffer);
	
	static REBOOL isBuffersEqual(const REPtr<REBuffer> & b1, const REPtr<REBuffer> & b2);
	
	static REUInt32 writeArrayF32(const REFloat32 * arr, char * buff, const REUInt32 count, const char sep = ';');
	
	static REUInt32 readArrayF32(const char * buff, REFloat32 * arr, const REUInt32 count, const char sep = ';');

	static REBOOL readFirstNumber(const char * str, REInt64 * i64vOrNull, REFloat64 * f64vOrNull);
};

}
	
#endif /* __RESTRINGUTILSPRIVATE_H__  */

