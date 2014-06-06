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


#ifndef __FAYECPP_DELEGATE_H__
#define __FAYECPP_DELEGATE_H__

#include "common.h"


namespace FayeCpp {
	
	class Delegate
	{
	public:
		/**
		 @brief Client transport protocol connected to server.
		 @param client Faye client object.
		 */
		virtual void onFayeTransportConnected(FayeCpp::Client * client) = 0;
		
		
		/**
		 @brief Client transport protocol disconnected from server.
		 @param client Faye client object.
		 */
		virtual void onFayeTransportDisconnected(FayeCpp::Client * client) = 0;
		
		
		/**
		 @brief Faye client connected to faye server.
		 @param client Faye client object.
		 */
		virtual void onFayeClientConnected(FayeCpp::Client * client) = 0;
		
		
		/**
		 @brief Faye client disconnected to faye server.
		 @param client Faye client object.
		 */
		virtual void onFayeClientDisconnected(FayeCpp::Client * client) = 0;
		
		
		/**
		 @brief Faye clent subscribed to channel.
		 @param client Faye client object.
		 @param channel Channel name.
		 */
		virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client, const std::string & channel) = 0;
		
		
		/**
		 @brief Faye clent unsubscribed from channel.
		 @param client Faye client object.
		 @param channel Target channel name.
		 */
		virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client, const std::string & channel) = 0;
		
		
		/**
		 @brief Called when faye client received non empty data from server responce using subscribed channel.
		 @param client Faye client object.
		 @param data Received non empty responce data.
		 @param channel Subscribed channel which received message data.
		 */
		virtual void onFayeClientReceivedDataFromChannel(FayeCpp::Client * client, const std::vector<unsigned char> & data, const std::string & channel) = 0;
		
		
		/**
		 @brief Called before message will be sended. You can modify this message if needed.
		 @param client Faye client object.
		 @param message Message object.
		 */
		virtual void onFayeClientWillSendMessage(FayeCpp::Client * client, FayeCpp::Message * message) = 0;
		
		
		/**
		 @brief Called before message will be processed by the client object.
		 @param client Faye client object.
		 @param message Message object.
		 */
		virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client, FayeCpp::Message * message) = 0;
		
		
		/**
		 @brief Called on faye client or transport error.
		 @param client Faye client object.
		 @param errorString Readable error string.
		 */
		virtual void onFayeErrorString(FayeCpp::Client * client, const std::string & errorString) = 0;
		
		
		virtual ~Delegate() { }
	};
	
}

#endif /* __FAYECPP_DELEGATE_H__ */
