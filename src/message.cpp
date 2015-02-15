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

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#include "jsonutils.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(o) if(o){delete o;o=NULL;}
#endif

namespace FayeCpp {
	
	Error * Responce::error() const
	{
		return _error;
	}
	
	REVariantList * Responce::messageList() const
	{
		return _messageList;
	}
	
	REVariantMap * Responce::messageMap() const
	{
		return _messageMap;
	}
	
	REBuffer * Responce::messageBuffer() const
	{
		return _messageBuffer;
	}
	
	Responce::ResponceType Responce::type() const
	{
		return _type;
	}
	
	Responce & Responce::setType(Responce::ResponceType type)
	{
		_type = type;
		return *this;
	}
	
	Responce & Responce::setError(const Error & error)
	{
		SAFE_DELETE(_error)
		_error = new Error(error);
		return *this;
	}

	Responce & Responce::setMessageText(const char * text)
	{
		if (text) 
		{
			this->setMessageData((const unsigned char *)text, strlen(text) + 1);
		}
		return *this;
	}
	
	Responce & Responce::setMessageData(const unsigned char * data, const size_t dataSize)
	{
		if (data && dataSize > 0) 
		{
			bool isOk = false;
			JsonParser parser((const char *)data);
			if (parser.isList()) 
			{
				_messageList = new REVariantList();
				if (_messageList) isOk = parser.toList(*_messageList);
			}
			else if (parser.isMap())
			{
				_messageMap = new REVariantMap();
				if (_messageMap) isOk = parser.toMap(*_messageMap);
			}
			
			if (!isOk) 
			{
				_messageBuffer = new REBuffer(data, (REUInt32)dataSize);
			}
		}
		return *this;
	}
	
	Responce::Responce(const Responce::ResponceType type) :
		_messageList(NULL),
		_messageMap(NULL),
		_messageBuffer(NULL),
		_error(NULL),
		_type(type)
	{
		
	}
	
	Responce::Responce() :
		_messageList(NULL),
		_messageMap(NULL),
		_messageBuffer(NULL),
		_error(NULL),
		_type(ResponceNone)
	{
		
	}
	
	Responce::~Responce()
	{
		SAFE_DELETE(_messageList)
		SAFE_DELETE(_messageMap)
		SAFE_DELETE(_messageBuffer)
		SAFE_DELETE(_error)
	}
	
}

