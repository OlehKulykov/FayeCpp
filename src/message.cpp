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

#include <jansson.h>

namespace FayeCpp {
	

    Message::MessageType Message::type() const
	{
		return _type;
	}
	
	const REString & Message::clientId() const
	{
		return _clientId;
	}
	
	const REString & Message::channel() const
	{
		return _channel;
	}
	
    Message::ChannelType Message::channelType() const
	{
		return Message::stringToType(_channel);
	}
	
	const REString & Message::subscription() const
	{
		return _subscription;
	}
	
	const REString & Message::errorString() const
	{
		return _errorString;
	}
	
	bool Message::isSuccessfully() const
	{
		return _isSuccessfully;
	}
	
	const REString & Message::version() const
	{
		return _version;
	}
	
	const REString & Message::minimumVersion() const
	{
		return _minimumVersion;
	}
	
	const REString & Message::connectionType() const
	{
		return _connectionType;
	}
	
	const REStringList & Message::connectionTypes() const
	{
		return _connectionTypes;
	}
	
	const REBuffer & Message::data() const
	{
		return _data;
	}
	
    Message & Message::setType(Message::MessageType type)
	{
		_type = type;
		return *this;
	}
	
	Message & Message::setClientId(const char * value)
	{
		_clientId = value;
		return *this;
	}
	
	Message & Message::setChannel(const char * value)
	{
		_channel = value;
		return *this;
	}
	
    Message & Message::setChannelType(const Message::ChannelType type)
	{
		_channel = Message::typeToString(type);
		return *this;
	}
	
	Message & Message::setSubscription(const char * value)
	{
		_subscription = value;
		return *this;
	}
	
	Message & Message::setSuccessfully(bool value)
	{
		_isSuccessfully = value;
		return *this;
	}
	
	Message & Message::setErrorString(const char * value)
	{
		_errorString = value;
		return *this;
	}
	
	Message & Message::setVersion(const char * value)
	{
		_version = value;
		return *this;
	}
	
	Message & Message::addConnectionType(const char * connectionType)
	{
		if (connectionType)
		{
			_connectionTypes.add(REString(connectionType));
		}
		return *this;
	}
	
	Message & Message::setMinimumVersion(const char * value)
	{
		_minimumVersion = value;
		return *this;
	}
	
	Message & Message::setConnectionType(const char * value)
	{
		_connectionType = value;
		return *this;
	}
	
	void Message::fromJsonDataBuff(const char * jsonData, const size_t dataSize)
	{
		json_error_t error;
//		except that the string doesn’t need to be null-terminated
		json_t * json = json_loadb(jsonData, dataSize, 0, &error);
		if (json)
		{
			if(json_is_array(json))
			{
				for(size_t i = 0; i < json_array_size(json); i++)
				{
					json_t * object = json_array_get(json, i);
					if (json_is_object(object))
					{
						json_t * o = json_object_get(object, "clientId");
						if (json_is_string(o)) _clientId = json_string_value(o);
						
						o = json_object_get(object, "channel");
						if (json_is_string(o)) _channel = json_string_value(o);
						
						o = json_object_get(object, "successful");
						_isSuccessfully = json_is_true(o) ? true : false;
						
						o = json_object_get(object, "error");
						if (json_is_string(o)) _errorString = json_string_value(o);
						
						o = json_object_get(object, "subscription");
						if (json_is_string(o)) _subscription = json_string_value(o);
						
						o = json_object_get(object, "version");
						if (json_is_string(o)) _version = json_string_value(o);
						
						o = json_object_get(object, "supportedConnectionTypes");
						if (json_is_array(o))
						{
							for(size_t j = 0; j < json_array_size(o); j++)
							{
								json_t * t = json_array_get(o, j);
								if (json_is_string(t))
								{
									const char * typePtr = json_string_value(t);
									if (typePtr)
									{
										REString typeString(typePtr);
										if (typeString.isNotEmpty())
										{
											_connectionTypes.add(typeString);
										}
									}
								}
							}
						}
					}
				}
			}
			json_decref(json);
		}
	}
	
	bool Message::isEmpty() const
	{
		return _clientId.isEmpty() && _channel.isEmpty() && _subscription.isEmpty() && _errorString.isEmpty();
	}
	
    char * Message::jsonCString() const
    {
        char * jsonString = NULL;
        json_t * json = json_object();
        if (json)
        {
            if (!_channel.isEmpty()) json_object_set_new(json, "channel", json_string(_channel.UTF8String()));
            if (!_clientId.isEmpty()) json_object_set_new(json, "clientId", json_string(_clientId.UTF8String()));
            if (!_subscription.isEmpty()) json_object_set_new(json, "subscription", json_string(_subscription.UTF8String()));
            if (!_version.isEmpty()) json_object_set_new(json, "version", json_string(_version.UTF8String()));
            if (!_minimumVersion.isEmpty()) json_object_set_new(json, "minimumVersion", json_string(_minimumVersion.UTF8String()));
            if (!_connectionType.isEmpty()) json_object_set_new(json, "connectionType", json_string(_connectionType.UTF8String()));
            if (!_connectionTypes.isEmpty())
            {
                json_t * types = json_array();
                if (types)
                {
					REStringList::Iterator i = _connectionTypes.iterator();
					while (i.next()) 
					{
						if (i.value().isNotEmpty()) 
						{
							json_array_append_new(types, json_string(i.value().UTF8String()));
						}
					}
                }
                json_object_set(json, "supportedConnectionTypes", types);
                json_decref(types);
            }

            jsonString = json_dumps(json, 0);
            json_decref(json);
        }
        return jsonString;
    }

    REString Message::toJsonString() const
    {
        char * jsonString = this->jsonCString();
        if (jsonString)
        {
            REString strng(jsonString);
            free(jsonString);
            return strng;
        }
        return REString();
    }

	Message::Message() :
        _type(MessageTypeNone),
        _isSuccessfully(false)
	{
		
	}
	
	Message::Message(const char * jsonString) :
		_type(MessageTypeNone),
		_isSuccessfully(false)
	{
		if (jsonString)
		{
			this->fromJsonDataBuff(jsonString, strlen(jsonString));
		}
	}
	
	Message::Message(const unsigned char * data, const size_t dataSize) :
		_type(MessageTypeNone),
		_isSuccessfully(false)
	{
		if (data && dataSize > 0)
		{
			this->fromJsonDataBuff((const char *)data, dataSize);
		}
	}
	
#define HANDSHAKE_CHANNEL "/meta/handshake"
#define CONNECT_CHANNEL "/meta/connect"
#define DISCONNECT_CHANNEL "/meta/disconnect"
#define SUBSCRIBE_CHANNEL "/meta/subscribe"
#define UNSUBSCRIBE_CHANNEL "/meta/unsubscribe"
	
    Message::ChannelType Message::stringToType(const char * typeString)
	{
		if (typeString)
		{
			if (strcmp(typeString, HANDSHAKE_CHANNEL) == 0) return Message::ChannelTypeHandshake;
			else if (strcmp(typeString, CONNECT_CHANNEL) == 0) return Message::ChannelTypeConnect;
			else if (strcmp(typeString, DISCONNECT_CHANNEL) == 0) return Message::ChannelTypeDisconnect;
			else if (strcmp(typeString, SUBSCRIBE_CHANNEL) == 0) return Message::ChannelTypeSubscribe;
			else if (strcmp(typeString, UNSUBSCRIBE_CHANNEL) == 0) return Message::ChannelTypeUnsubscribe;
		}
        return Message::ChannelTypeNone;
	}
	
    const char * Message::typeToString(const Message::ChannelType type)
	{
		switch (type)
		{
            case Message::ChannelTypeHandshake: return HANDSHAKE_CHANNEL; break;
            case Message::ChannelTypeConnect: return CONNECT_CHANNEL; break;
            case Message::ChannelTypeDisconnect: return DISCONNECT_CHANNEL; break;
            case Message::ChannelTypeSubscribe: return SUBSCRIBE_CHANNEL; break;
            case Message::ChannelTypeUnsubscribe: return UNSUBSCRIBE_CHANNEL; break;
			default: break;
		}
		return "";
	}
}


