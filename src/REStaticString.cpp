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
	
	void REStaticString::freeNonCopyBuffMem(void * m)
	{
		// do not free, this is static memory
		(void)m;
	}
	
	REStaticString::REStaticString(const char * utf8String, const REUInt32 utf8StringLength /* = RENotFound */ ) : REString()
	{
		const REUInt32 len = REStringUtilsPrivate::actualUTF8StringLength(utf8String, utf8StringLength);
		if (len)
		{
			REBufferNoCopy * newBuff = new REBufferNoCopy(const_cast<char *>(utf8String), len + 1, &REStaticString::freeNonCopyBuffMem);
			if (newBuff) 
			{
				_p = REPtr<REBuffer>(newBuff);
			}
		}
	}
	
	REStaticString::REStaticString(const wchar_t * wideString, const REUInt32 wideStringLength /* = RENotFound */ ) : REString()
	{
		const REUInt32 len = REStringUtilsPrivate::actualWideStringLength(wideString, wideStringLength);
		if (len)
		{
			REBufferNoCopy * newBuff = new REBufferNoCopy(const_cast<wchar_t *>(wideString), sizeof(wchar_t) * (len + 1), &REStaticString::freeNonCopyBuffMem);
			if (newBuff) 
			{
				_p = REPtr<REBuffer>(newBuff);
			}
		}
	}
	
	REStaticString::~REStaticString()
	{
		
	}
	
}

