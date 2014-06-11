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


#include "fayecpp.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#include <jansson.h>

namespace FayeCpp {
	

    Message::MessageType Message::type() const
	{
		return _type;
	}
	
	const std::string & Message::clientId() const
	{
		return _clientId;
	}
	
	const std::string & Message::channel() const
	{
		return _channel;
	}
	
    Message::ChannelType Message::channelType() const
	{
		return Message::stringToType(_channel);
	}
	
	const std::string & Message::subscription() const
	{
		return _subscription;
	}
	
	const std::string & Message::errorString() const
	{
		return _errorString;
	}
	
	bool Message::isSuccessfully() const
	{
		return _isSuccessfully;
	}
	
	const std::string & Message::version() const
	{
		return _version;
	}
	
	const std::string & Message::minimumVersion() const
	{
		return _minimumVersion;
	}
	
	const std::string & Message::connectionType() const
	{
		return _connectionType;
	}
	
	const std::list<std::string> & Message::connectionTypes() const
	{
		return _connectionTypes;
	}
	
	const std::vector<unsigned char> & Message::data() const
	{
		return _data;
	}
	
    Message & Message::setType(Message::MessageType type)
	{
		_type = type;
		return *this;
	}
	
	Message & Message::setClientId(const std::string & value)
	{
		_clientId = value;
		return *this;
	}
	
	Message & Message::setChannel(const std::string & value)
	{
		_channel = value;
		return *this;
	}
	
    Message & Message::setChannelType(const Message::ChannelType type)
	{
		_channel = Message::typeToString(type);
		return *this;
	}
	
	Message & Message::setSubscription(const std::string & value)
	{
		_subscription = value;
		return *this;
	}
	
	Message & Message::setSuccessfully(bool value)
	{
		_isSuccessfully = value;
		return *this;
	}
	
	Message & Message::setErrorString(const std::string & value)
	{
		_errorString = value;
		return *this;
	}
	
	Message & Message::setVersion(const std::string & value)
	{
		_version = value;
		return *this;
	}
	
	Message & Message::addConnectionType(const std::string & connectionType)
	{
		if (!connectionType.empty())
		{
			_connectionTypes.push_back(connectionType);
		}
		return *this;
	}
	
	Message & Message::setMinimumVersion(const std::string & value)
	{
		_minimumVersion = value;
		return *this;
	}
	
	Message & Message::setConnectionType(const std::string & value)
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
										std::string typeString(typePtr);
										if (!typeString.empty()) _connectionTypes.push_back(typeString);
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
		return _clientId.empty() && _channel.empty() && _subscription.empty() && _errorString.empty();
	}
	
    char * Message::jsonCString() const
    {
        char * jsonString = NULL;
        json_t * json = json_object();
        if (json)
        {
            if (!_channel.empty()) json_object_set_new(json, "channel", json_string(_channel.c_str()));
            if (!_clientId.empty()) json_object_set_new(json, "clientId", json_string(_clientId.c_str()));
            if (!_subscription.empty()) json_object_set_new(json, "subscription", json_string(_subscription.c_str()));
            if (!_version.empty()) json_object_set_new(json, "version", json_string(_version.c_str()));
            if (!_minimumVersion.empty()) json_object_set_new(json, "minimumVersion", json_string(_minimumVersion.c_str()));
            if (!_connectionType.empty()) json_object_set_new(json, "connectionType", json_string(_connectionType.c_str()));
            if (!_connectionTypes.empty())
            {
                json_t * types = json_array();
                if (types)
                {
                    for (std::list<std::string>::const_iterator i = _connectionTypes.begin(); i != _connectionTypes.end(); ++i)
                    {
                        if (!(*i).empty())
                        {
                            json_array_append_new(types, json_string((*i).c_str()));
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

    std::vector<unsigned char> Message::toJsonData() const
	{
        char * jsonString = this->jsonCString();
        if (jsonString)
        {
            std::vector<unsigned char> data(jsonString, jsonString + strlen(jsonString) + 1);
            free(jsonString);
            return data;
        }
        return std::vector<unsigned char>();
	}
	
    std::string Message::toJsonString() const
    {
        char * jsonString = this->jsonCString();
        if (jsonString)
        {
            std::string strng(jsonString);
            free(jsonString);
            return strng;
        }
        return std::string();
    }

	Message::Message() :
        _type(MessageTypeNone),
        _isSuccessfully(false)
	{
		
	}
	
	Message::Message(const std::string & jsonString) :
		_type(MessageTypeNone),
		_isSuccessfully(false)
	{
		if (!jsonString.empty())
		{
			const char * s = jsonString.data();
			this->fromJsonDataBuff(s, strlen(s));
		}
	}
	
	Message::Message(const std::vector<unsigned char> & jsonData) :
		_type(MessageTypeNone),
		_isSuccessfully(false)
	{
		if (!jsonData.empty())
		{
			this->fromJsonDataBuff((const char *)&jsonData.front(), jsonData.size());
		}
	}
	
#define HANDSHAKE_CHANNEL "/meta/handshake"
#define CONNECT_CHANNEL "/meta/connect"
#define DISCONNECT_CHANNEL "/meta/disconnect"
#define SUBSCRIBE_CHANNEL "/meta/subscribe"
#define UNSUBSCRIBE_CHANNEL "/meta/unsubscribe"
	
    Message::ChannelType Message::stringToType(const std::string & typeString)
	{
        if (typeString == HANDSHAKE_CHANNEL) return Message::ChannelTypeHandshake;
        else if (typeString == CONNECT_CHANNEL) return Message::ChannelTypeConnect;
        else if (typeString == DISCONNECT_CHANNEL) return Message::ChannelTypeDisconnect;
        else if (typeString == SUBSCRIBE_CHANNEL) return Message::ChannelTypeSubscribe;
        else if (typeString == UNSUBSCRIBE_CHANNEL) return Message::ChannelTypeUnsubscribe;
        return Message::ChannelTypeNone;
	}
	
    std::string Message::typeToString(const Message::ChannelType type)
	{
		switch (type)
		{
            case Message::ChannelTypeHandshake: return std::string(HANDSHAKE_CHANNEL); break;
            case Message::ChannelTypeConnect: return std::string(CONNECT_CHANNEL); break;
            case Message::ChannelTypeDisconnect: return std::string(DISCONNECT_CHANNEL); break;
            case Message::ChannelTypeSubscribe: return std::string(SUBSCRIBE_CHANNEL); break;
            case Message::ChannelTypeUnsubscribe: return std::string(UNSUBSCRIBE_CHANNEL); break;
			default: break;
		}
		return std::string();
	}
}


