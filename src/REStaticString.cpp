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
	
	void REStaticString::freeNonCopyBuffMem(void * m)
	{
		// do not free, this is static memory
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

