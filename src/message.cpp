/*
 *   Copyright 2014 Kulykov Oleh
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

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#include "jsonutils.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(o) if(o){delete o;o=NULL;}
#endif

namespace FayeCpp {
	
	REString * Responce::errorString() const
	{
		return _errorString;
	}
	
	VariantList * Responce::messageList() const
	{
		return _messageList;
	}
	
	VariantMap * Responce::messageMap() const
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
	
	Responce & Responce::setErrorString(const char * value)
	{
		if (_errorString) 
		{
			_errorString->setFromUTF8String(value);
		}
		else
		{
			_errorString = new REString(value);
		}
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
				_messageList = new VariantList();
				if (_messageList) isOk = parser.toList(*_messageList);
			}
			else if (parser.isMap())
			{
				_messageMap = new VariantMap();
				if (_messageMap) isOk = parser.toMap(*_messageMap);
			}
			
			if (!isOk) 
			{
				_messageBuffer = new REBuffer(data, (REUInt32)dataSize);
			}
		}
		return *this;
	}
	
	Responce::Responce() :
		_messageList(NULL),
		_messageMap(NULL),
		_messageBuffer(NULL),
		_errorString(NULL),
		_type(ResponceNone)
	{
		
	}
	
	Responce::~Responce()
	{
		SAFE_DELETE(_messageList)
		SAFE_DELETE(_messageMap)
		SAFE_DELETE(_messageBuffer)
		SAFE_DELETE(_errorString)
	}
	
}


