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


#include "REStringUtilsPrivate.h"

#if defined(HAVE_RECORE_SDK_CONFIG_H) 
#include "recore_sdk_config.h"
#endif

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

namespace FayeCpp {
	
	REBuffer * REStringUtilsPrivate::newBufferWithSize(const REUInt32 newSize)
	{
		REBuffer * b = new REBuffer(newSize);
		if (b) 
		{
			if (b->size() == newSize) 
			{
				return b;
			}
			delete b;
		}
		return NULL;
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::newBuffForUTF8String(const char * utf8String, 
															   const REUInt32 utf8StringLength)
	{
		const REUInt32 len = REStringUtilsPrivate::actualUTF8StringLength(utf8String, utf8StringLength);
		if (len)
		{
			REBuffer * newBuff = REStringUtilsPrivate::newBufferWithSize(len + 1);
			if (newBuff)
			{
				char * buff = (char *)newBuff->buffer();
				memcpy(buff, utf8String, len);
				buff[len] = 0;
				return REPtr<REBuffer>(newBuff);
			}
		}
		return REPtr<REBuffer>();
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::newBuffForWideString(const wchar_t * wideString, 
															   const REUInt32 wideStringLength)
	{
		const REUInt32 len = REStringUtilsPrivate::actualWideStringLength(wideString, wideStringLength);
		if (len)
		{
			REBuffer * newBuff = REStringUtilsPrivate::newBufferWithSize((len + 1) * sizeof(wchar_t));
			if (newBuff)
			{
				wchar_t * buff = (wchar_t *)newBuff->buffer();
				memcpy(buff, wideString, (len * sizeof(wchar_t)));
				buff[len] = 0;
				return REPtr<REBuffer>(newBuff);
			}
		}
		return REPtr<REBuffer>();
	}
	
	
	REPtr<REBuffer> REStringUtilsPrivate::getWideFromUTF8(const char * utf8String, 
														  const REUInt32 utf8StringLength)
	{
		const REUInt32 len = REStringUtilsPrivate::actualUTF8StringLength(utf8String, utf8StringLength);
		if (len)
		{
			REBuffer * newBuff = REStringUtilsPrivate::newBufferWithSize((len + 1) * sizeof(wchar_t));
			if (newBuff)
			{
				wchar_t * wideString = (wchar_t *)newBuff->buffer();
				const int resLen = REStringUtilsPrivate::charsToWide(utf8String, (int)len, wideString);
				if (newBuff->resize((resLen + 1) * sizeof(wchar_t), true))
				{
					wideString = (wchar_t *)newBuff->buffer();
					wideString[resLen] = 0;
					return REPtr<REBuffer>(newBuff);
				}
			}
		}
		return REPtr<REBuffer>();
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::getUTF8FromWide(const wchar_t * wideString, 
														  const REUInt32 wideStringLength)
	{
		const REUInt32 len = REStringUtilsPrivate::actualWideStringLength(wideString, wideStringLength);
		if (len)
		{
			REBuffer * newBuffer = REStringUtilsPrivate::newBufferWithSize((len + 1) * sizeof(wchar_t));
			if (newBuffer)
			{
				char * charString = (char *)newBuffer->buffer();
				const int resLen = REStringUtilsPrivate::wideToChars(wideString,
																	 (int)len,
																	 charString);
				if (newBuffer->resize(resLen + 1, true))
				{
					charString = (char *)newBuffer->buffer();
					charString[resLen] = 0;
					return REPtr<REBuffer>(newBuffer);
				}
				delete newBuffer;
			}
		}
		return REPtr<REBuffer>();
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::getWideFromUTF8(const REPtr<REBuffer> & utf8StringBuffer)
	{
		const REUInt32 len = REStringUtilsPrivate::stringLengthFromUTF8Buffer(utf8StringBuffer);
		if (len > 0)
		{
			return REStringUtilsPrivate::getWideFromUTF8((const char *)utf8StringBuffer->buffer(), 
														 len);
		}
		return REPtr<REBuffer>();
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::getUTF8FromWide(const REPtr<REBuffer> & wideStringBuffer)
	{
		const REUInt32 len = REStringUtilsPrivate::stringLengthFromWideBuffer(wideStringBuffer);
		if (len > 0)
		{
			return REStringUtilsPrivate::getUTF8FromWide((const wchar_t *)wideStringBuffer->buffer(), 
														 len);
		}
		return REPtr<REBuffer>();
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::makeCopy(const REPtr<REBuffer> & sourceBuffer)
	{
		if (REStringUtilsPrivate::isStringExists(sourceBuffer))
		{
			const REUInt32 size = sourceBuffer->size();
			if (size > 0)
			{
				REBuffer * newBuff = REStringUtilsPrivate::newBufferWithSize(size);
				if (newBuff)
				{
					memcpy(newBuff->buffer(), sourceBuffer->buffer(), size);
					return REPtr<REBuffer>(newBuff);
				}
			}
		}
		return REPtr<REBuffer>();
	}
	
	REUInt32 REStringUtilsPrivate::stringLengthFromUTF8Buffer(const REPtr<REBuffer> & utf8StringBuffer)
	{
		if (REStringUtilsPrivate::isStringExists(utf8StringBuffer)) 
		{
			const REUInt32 size = utf8StringBuffer->size();
			return (size > 0) ? (size - 1) : 0;
		}
		return 0;
	}
	
	REUInt32 REStringUtilsPrivate::stringLengthFromWideBuffer(const REPtr<REBuffer> & wideStringBuffer)
	{
		if (REStringUtilsPrivate::isStringExists(wideStringBuffer)) 
		{
			const REUInt32 size = wideStringBuffer->size();
			const REUInt32 charSize = (REUInt32)sizeof(wchar_t);
			return (size >= charSize) ? ((size / charSize) - 1) : 0;
		}
		return 0;
	}
	
	
	int REStringUtilsPrivate::wideToChars(const wchar_t * wideString, int wideStringLength, char * charsString)
	{
		int stringLength = 0;
		if (wideStringLength > 0)
		{
			int l = wideStringLength;
			unsigned char * cursor = (unsigned char *)charsString;
			const wchar_t * ch = wideString;
			for (int i = 0; i < l; i++) 
			{
				wchar_t u = *ch;
				if (u < (unsigned char)0x80) 
				{
					*cursor++ = (unsigned char)u;
				} 
				else 
				{
					if (u < 0x0800) 
					{
						*cursor++ = (unsigned char)0xc0 | ((unsigned char) (u >> 6));
					} 
					else 
					{
						if ( (u >= 0xd800) && (u < 0xdc00) && (i < l-1)) 
						{
							unsigned short low = ch[1];
							if ((low >= 0xdc00) && (low < 0xe000)) 
							{
								++ch;
								++i;
								u = ((u - 0xd800)*0x400) + (low - 0xdc00) + 0x10000;
							}
						}
						if (u > 0xffff) 
						{
							// if people are working in utf8, but strings are encoded in eg. latin1, the resulting
							// name might be invalid utf8. This and the corresponding code in fromUtf8 takes care
							// we can handle this without loosing information. This can happen with latin filenames
							// and a utf8 locale under Unix.
							if ( (u > 0x10fe00) && (u < 0x10ff00) )
							{
								*cursor++ = (u - 0x10fe00);
								++ch;
								continue;
							} 
							else 
							{
								*cursor++ = (unsigned char)0xf0 | ((unsigned char) (u >> 18));
								*cursor++ = (unsigned char)0x80 | (((unsigned char) (u >> 12)) & (unsigned char)0x3f);
							}
						} 
						else 
						{
							*cursor++ = (unsigned char)0xe0 | ((unsigned char) (u >> 12));
						}
						*cursor++ = (unsigned char)0x80 | (((unsigned char) (u >> 6)) & (unsigned char)0x3f);
					}
					*cursor++ = (unsigned char)0x80 | ((unsigned char) (u & (unsigned char)0x3f));
				}
				++ch;
			}
			stringLength = (int)(cursor - (unsigned char*)charsString);
		}
		return stringLength; 
	}
	
	int REStringUtilsPrivate::charsToWide(const char * charsString, int charsStringLengthOrNegative, wchar_t * wideString)
	{
		if ( charsStringLengthOrNegative < 0)
		{
			charsStringLengthOrNegative = (int)strlen(charsString);
		}
		
		wchar_t uc = 0;
		int need = 0;
		int error = -1;
		unsigned char ch;
		int stringLength = 0;
		for (int i = 0; i < charsStringLengthOrNegative; ++i) 
		{
			ch = charsString[i];
			if (need) 
			{
				if ((ch&(unsigned char)0xc0) == (unsigned char)0x80) 
				{
					uc = (uc << 6) | (ch & (unsigned char)0x3f);
					need--;
					if (!need) 
					{
						if (uc > 0xffff) 
						{
							uc -= 0x10000;
							wchar_t high = (uc/0x400) + 0xd800;
							wchar_t low = (uc%0x400) + 0xdc00;
							*wideString++ = high;
							*wideString++ = low;
							stringLength++;
						} 
						else 
						{
							*wideString++ = uc;
							stringLength++;
						}
					}
				} 
				else 
				{
					i = error;
					*wideString++ = (wchar_t)0xdbff;
					*wideString++ = (wchar_t)0xde00 + ((unsigned char)charsString[i]);
					stringLength++;
					need = 0;
				}
			}
			else 
			{
				if (ch < 128) 
				{
					*wideString++ = ch;
					stringLength++;
				} 
				else if ((ch & 0xe0) == 0xc0) 
				{
					uc = ch & 0x1f;
					need = 1;
					error = i;
				} 
				else if ((ch & 0xf0) == 0xe0) 
				{
					uc = ch & 0x0f;
					need = 2;
					error = i;
				} 
				else if ((ch & 0xf8) == 0xf0) 
				{
					uc = ch & 0x07;
					need = 3;
					error = i;
				} 
				else 
				{
					*wideString++ = 0xdbff;
					*wideString++ = 0xde00 + ((unsigned char)charsString[i]);
					stringLength++;
				}
			}
		}
		if (need) 
		{
			for (int i = error; i < charsStringLengthOrNegative; ++i) 
			{
				*wideString++ = 0xdbff;
				*wideString++ = 0xde00 + (unsigned char)charsString[i];
				stringLength++;
			}
		}
		return stringLength;
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::getAppendedWithPathComponent(const REPtr<REBuffer> & utf8Buffer, 
																	   const char * comp)
	{
#ifdef __RE_OS_WINDOWS__
	//TODO: make separator windows logic
#endif
		
		REBOOL rep2 = false;
		REBOOL rep1 = false;
		REUInt32 l1 = 0;
		REUInt32 l2 = 0;
		const char * p1 = NULL;
		const char * p2 = NULL;
		if (REStringUtilsPrivate::isStringExists(utf8Buffer))
		{
			char * p = (char *)utf8Buffer->buffer();
			while (*p) 
			{
				if (*p == '\\') rep1 = true;
				p++;
				l1++;
			}
			if (l1 > 0) 
			{ 
				p1 = (const char *)utf8Buffer->buffer();
				if ((p1[l1 - 1] == '/') || (p1[l1 - 1] == '\\')) l1--;
			}
		}
		
		if (comp)
		{
			const char * p = comp;
			while (*p) 
			{
				if (*p++ == '\\') rep2 = true;
				l2++;
			}
			if (l2 > 0) 
			{ 
				p2 = comp; 
				if ((l1 > 0) && ((p2[0] == '/') || (p2[0] == '\\'))) { p2++; l2--; }
				if ((l2 > 0) && ((p2[l2 - 1] == '/') || (p2[l2 - 1] == '\\'))) { l2--; }
			}
		}
		
		if ((l1 > 0) || (l2 > 0))
		{
			const REUInt32 l = l1 + l2 + 2; // +1 - separator, +1 - NULL char
			REBuffer * newBuff = new REBuffer();
			if (newBuff)
			{
				if (newBuff->resize(l, false))
				{
					char * p = (char *)newBuff->buffer();
					if (l1 > 0) 
					{
						if (rep1)
						{
							REUInt32 wl = 0;
							const char * from = p1;
							while (wl++ < l1) 
							{
								*p++ = (*from == '\\') ? '/' : *from;
								from++;
							}
						}
						else
						{
							memcpy(p, p1, l1);
							p += l1;
						}
					}
					
					if (l2 > 0)
					{
						if (l1 > 0) *p++ = '/'; // +1 - separator
						if (rep2)
						{
							REUInt32 wl = 0;
							const char * from = p2;
							while (wl++ < l2) 
							{
								*p++ = (*from == '\\') ? '/' : *from;
								from++;
							}
						}
						else
						{
							memcpy(p, p2, l2);
							p += l2;
						}
					}
					*p = 0;
					return REPtr<REBuffer>(newBuff);
				}
				delete newBuff;
			}
		}
		return REPtr<REBuffer>();
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::getRemovedPathExtension(const REPtr<REBuffer> & utf8Buffer)
	{
		REUInt32 l = REStringUtilsPrivate::stringLengthFromUTF8Buffer(utf8Buffer);
		if (l > 0)
		{
			const char * e = (const char *)utf8Buffer->buffer();
			e += (l - 1); // to last
			const REBOOL needRepSingleSeparator = (*e == '\\');
			if ((*e == '/') || needRepSingleSeparator)
			{
				e--; l--;
				if (l == 0) 
				{
					if (needRepSingleSeparator)
					{
						REPtr<REBuffer> newB = REStringUtilsPrivate::makeCopy(utf8Buffer);
						if (REStringUtilsPrivate::isStringExists(newB))
						{
							char * newBuff = (char *)newB->buffer();
							*newBuff = '/';
						}
						return newB;
					}
					else
					{
						return REStringUtilsPrivate::makeCopy(utf8Buffer);
					}
				}
			}
			REUInt32 needLen = l;
			if (l > 0)
			{
				REUInt32 l1 = l;
				while (l1 > 0) 
				{
					if (*e == '.') { needLen = l1 - 1; break; }
					else if ((*e == '/') || (*e == '\\')) { break; }
					e--;
					l1--;
				}
			}
			
			if (needLen > 0)
			{
				REBuffer * newBuff = new REBuffer();
				if (newBuff)
				{
					if (newBuff->resize(needLen + 1, false))
					{
						char * n = (char *)newBuff->buffer();
						const char * o = (const char *)utf8Buffer->buffer();
						REUInt32 copyedLen = 0;
						while (copyedLen++ != needLen) 
						{
							*n++ = (*o == '\\') ? '/' : *o;
							o++;
						}
						*n = 0;
						return REPtr<REBuffer>(newBuff);
					}
					delete newBuff;
				}
			}
		}
		return REPtr<REBuffer>();
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::getRemovedLastPathComponent(const REPtr<REBuffer> & utf8Buffer)
	{
		REUInt32 l = REStringUtilsPrivate::stringLengthFromUTF8Buffer(utf8Buffer);
		if (l > 0)
		{
			const char * e = (const char *)utf8Buffer->buffer();
			e += (l - 1); // to last
			const REBOOL needRepSingleSeparator = (*e == '\\');
			if ((*e == '/') || needRepSingleSeparator)
			{
				e--; l--;
				if (l == 0) 
				{
					if (needRepSingleSeparator)
					{
						REPtr<REBuffer> newB = REStringUtilsPrivate::makeCopy(utf8Buffer);
						if (REStringUtilsPrivate::isStringExists(newB))
						{
							char * newBuff = (char *)newB->buffer();
							*newBuff = '/';
						}
						return newB;
					}
					else
					{
						return REStringUtilsPrivate::makeCopy(utf8Buffer);
					}
				}
			}
			
			REUInt32 needLen = 0;
			if (l > 0)
			{
				REUInt32 l1 = l;
				while (l1 > 0) 
				{
					if ((*e == '/') || (*e == '\\')) { needLen = l1 - 1; break; }
					e--;
					l1--;
				}
			}
			
			if (needLen > 0)
			{
				REBuffer * newBuff = new REBuffer();
				if (newBuff)
				{
					if (newBuff->resize(needLen + 1, false))
					{
						char * n = (char *)newBuff->buffer();
						const char * o = (const char *)utf8Buffer->buffer();
						REUInt32 copyedLen = 0;
						while (copyedLen++ != needLen) 
						{
							*n++ = (*o == '\\') ? '/' : *o;
							o++;
						}
						*n = 0;
						return REPtr<REBuffer>(newBuff);
					}
					delete newBuff;
				}
			}
		}
		return REPtr<REBuffer>();
	}
	
	REPtr<REBuffer> REStringUtilsPrivate::getPathExtension(const REPtr<REBuffer> & utf8Buffer)
	{
		REUInt32 l = REStringUtilsPrivate::stringLengthFromUTF8Buffer(utf8Buffer);
		if (l > 0)
		{
			const char * e = (const char *)utf8Buffer->buffer();
			e += (l - 1); // to last
			if ((*e == '/') || (*e == '\\'))
			{
				e--; l--;
			}
			
			const char * from = NULL;
			REUInt32 needLen = 0;
			if (l > 0)
			{
				REUInt32 l1 = l;
				while (l1 > 0) 
				{
					if (*e == '.') { if (needLen > 0) from = ++e; break; }
					else if ((*e == '/') || (*e == '\\')) { needLen = 0; break; }
					e--;
					l1--;
					needLen++;
				}
			}
			
			if (from && (needLen > 0))
			{
				REBuffer * newBuff = new REBuffer();
				if (newBuff)
				{
					if (newBuff->resize(needLen + 1, false))
					{
						char * n = (char *)newBuff->buffer();
						memcpy(n, from, needLen);
						n += needLen;
						*n = 0;
						return REPtr<REBuffer>(newBuff);
					}
					delete newBuff;
				}
			}
		}
		return REPtr<REBuffer>();
	}
	
	REBOOL REStringUtilsPrivate::isBuffersEqual(const REPtr<REBuffer> & b1, const REPtr<REBuffer> & b2)
	{
		if (b1.isNotEmpty() && b2.isNotEmpty()) 
		{		
			return b1->isEqualToBuffer(*b2);
		}
		return false;
	}
	
	REBOOL REStringUtilsPrivate::readFirstNumber(const char * str, REInt64 * i64vOrNull, REFloat64 * f64vOrNull)
	{
		bool isPlus = false, isMinus = false;
		int isDig = false;
		while (*str) 
		{
			isPlus = (*str == '+');
			isMinus = (*str == '-');
			isDig = isdigit(*str);
			if (isDig || isPlus || isMinus)
			{
				const char * str1 = str;
				if (isPlus || isMinus) 
				{
					str1++; 
					if (*str1) isDig = isdigit(*str);
				}
				if (isDig) 
				{
					REBOOL reslt = false;
					if (i64vOrNull) 
					{
						long long int v = 0;
						if (sscanf(str1, "%lli", &v) == 1)
						{
							*i64vOrNull = isMinus ? (v * -1) : v;
							reslt = true;
						}	
					}
					if (f64vOrNull)
					{
						long double d = 0;
						if (sscanf(str1, "%Lf", &d) == 1)
						{
							*f64vOrNull = isMinus ? (d * -1) : d;
							reslt = true;
						}	
					}
					return reslt;
				}
			}
			str++;
		}
		
		return false;
	}
	
}

