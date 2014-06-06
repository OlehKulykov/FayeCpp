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


#ifndef __FAYECPP_MESSAGE_H__
#define __FAYECPP_MESSAGE_H__

#include "common.h"
#include "variant.h"

namespace FayeCpp {
	
	/**
	 @brief Type of Faye channel
	 */
	typedef enum _channelType
	{
		/**
		 @brief Undefined Faye channel
		 */
		ChannelTypeNone = 0,
		
		
		/**
		 @brief Handshake Faye channel
		 */
		ChannelTypeHandshake,
		
		
		/**
		 @brief Connect Faye channel
		 */
		ChannelTypeConnect,
		
		
		/**
		 @brief Disconnect Faye channel
		 */
		ChannelTypeDisconnect,
		
		
		/**
		 @brief Subscribe Faye channel
		 */
		ChannelTypeSubscribe,
		
		
		/**
		 @brief Unsubscribe Faye channel
		 */
		ChannelTypeUnsubscribe
	}
	/**
	 @brief Type of Faye channel
	 */
	ChannelType;
	
	
	
	/**
	 @brief Faye message type.
	 */
	typedef enum _messageType
	{
		/**
		 @brief Undefined, default type.
		 */
		MessageTypeNone = 0,
		
		
		/**
		 @brief Faye transport protocol connected to server.
		 */
		MessageTypeTransportConnected,
		
		
		/**
		 @brief Faye transport protocol disconnected from server.
		 */
		MessageTypeTransportDisconnected,
		
		
		/**
		 @brief Faye transport protocol error.
		 */
		MessageTypeTransportError,
		
		
		/**
		 @brief Faye transport protocol received message.
		 */
		MessageTypeServerResponce
	}
	/**
	 @brief Faye message type.
	 */
	MessageType;
	
	
	class Message
	{
	private:
		std::string _clientId;
		std::string _channel;
		std::string _subscription;
		std::string _errorString;
		std::string _version;
		std::string _minimumVersion;
		std::string _connectionType;
		std::list<std::string> _connectionTypes;
		std::vector<unsigned char> _data;
		MessageType _type;
		bool _isSuccessfully;
		
		void fromJsonDataBuff(const char * jsonData, const size_t dataSize);
        char * jsonString() const;

	public:
		MessageType type() const;
		const std::string & clientId() const;
		const std::string & channel() const;
		ChannelType channelType() const;
		const std::string & subscription() const;
		const std::string & errorString() const;
		bool isSuccessfully() const;
		const std::string & version() const;
		const std::string & minimumVersion() const;
		const std::string & connectionType() const;
		const std::list<std::string> & connectionTypes() const;
		const std::vector<unsigned char> & data() const;
		
		Message & setType(MessageType type);
		Message & setClientId(const std::string & value);
		Message & setChannel(const std::string & value);
		Message & setChannelType(const ChannelType type);
		Message & setSubscription(const std::string & value);
		Message & setSuccessfully(bool value);
		Message & setErrorString(const std::string & value);
		Message & setVersion(const std::string & value);
		Message & addConnectionType(const std::string & connectionType);
		Message & setMinimumVersion(const std::string & value);
		Message & setConnectionType(const std::string & value);
		
		bool isEmpty() const;
        std::vector<unsigned char> toJsonData() const;
        std::string toJsonString() const;
		Message();
		Message(const std::string & jsonString);
		Message(const std::vector<unsigned char> & jsonData);
		
		static ChannelType stringToType(const std::string & typeString);
		static std::string typeToString(const ChannelType type);
	};
	
}

#endif /* __FAYECPP_MESSAGE_H__ */
