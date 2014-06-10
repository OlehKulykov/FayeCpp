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


#ifndef __FAYECPP_CLIENT_H__
#define __FAYECPP_CLIENT_H__

#include "common.h"
#include "delegate.h"
#include "variant.h"
#include "message.h"

namespace FayeCpp {

    class Transport;

    class Client
    {
    private:
        Transport * _transport;
        Delegate * _delegate;
        std::string _clientId;

		std::list<std::string> _subscribedChannels;
		std::list<std::string> _pendingSubscriptions;
		std::list<std::string> _supportedConnectionTypes;
		
		bool _isFayeConnected;
		
		void processMessage(Message * message);
		
		void onTransportConnected(Message * message);
		void onTransportDisconnected(Message * message);
		
		void onClientMessageReceived(Message * message);
		
		void onClientError(Message * message);
		
		void onHandshakeDone(Message * message);
		void handshake();
		
		void onConnectFayeDone(Message * message);
		void connectFaye();
		
		void onSubscriptionDone(Message * message);
		void subscribePendingSubscriptions();
		
		void onUnsubscribingDone(Message * message);
		void onDisconnectFayeDone(Message * message);
		
		static unsigned long long _messageId;
		static unsigned long long nextMessageId();
		
	public:
		/**
		 @return List of subscribed channels.
		 */
		const std::list<std::string> & subscribedChannels() const;
		
		
		/**
		 @return List of supported connection names. Based on server responce(handshake) and on implemented types.
		 */
		const std::list<std::string> & supportedTransportNames() const;
		
		
		/**
		 @return Currently connected transport protocol connection type.
		 */
		std::string currentTransportName() const;
		
		
		/**
		 @brief Getter for client delegate.
		 @return Pointer to faye client delegate.
		 */
		Delegate * delegate();
		
		
		/**
		 @brief Setter for client delegate.
		 */
		void setDelegate(Delegate * delegate);
		
		
		/**
		 @brief Getter for faye client id. Returned from the server on handshe step.
		 */
		const std::string & clientId() const;
		
		
		/**
		 @brief Getter for faye url.
		 */
        const std::string & url() const;
		
		
		/**
		 @brief Setter for faye url.
		 @param url Faye server url string.
		 */
        void setUrl(const std::string & url);
		
		
		/**
		 @brief Start connection sequence. On the first step connecting transport protocol to server(inform delegate).
		 @brief Next - connecting faye(also, inform delegate).
		 @return True if sequence initialized, otherwice false.
		 */
		bool connect();
		
		
		/**
		 @brief Check connection of the transport protocol.
		 @return True - connected, otherwice false.
		 */
		bool isTransportConnected() const;
		
		
		/**
		 @brief Check faye client connection.
		 @return True - connected, otherwice false.
		 */
		bool isFayeConnected() const;
		
		
		/**
		 @brief Disconnects from faye server. Transport protocol could be still opened/connected.
		 */
		void disconnect();
		
		
		/**
		 @brief Start send message to subscribed channel via connected faye.
		 @param message Non empty message for send.
		 @param channel Non empty, subscribed channel.
		 @return True - if connected and parameters non empty and sended, othervice false.
		 */
		bool sendMessageToChannel(const std::map<std::string, Variant> & message, const std::string & channel);
		
		
		/**
		 @brief Check channel is subscribed.
		 @param channel Non empty channel.
		 @return True - if subscribed, otherwice false.
		 */
		bool isSubscribedToChannel(const std::string & channel) const;
		
		
		/**
		 * @brief Subscribes or storing to pendnig subscriptions required channel.
		 * @param channel Non empty channel.
		 * @return True - if already suscribed, started or stored to peding subscriptions, otherwice false.
		 */
		bool subscribeToChannel(const std::string & channel);
		
		
		/**
		 @brief Unsubscribe from specific channel.
		 @param channel Non empty subscribed channel.
		 @return True on unsubscription started, otherwice false or channel is empty or not subscribed.
		 */
		bool unsubscribeFromChannel(const std::string & channel);
		
		
		/**
		 @brief Unsubscribe from all subscribed channels. Also cleanup pending subscriptions.
		 */
		void unsubscribeAllChannels();
		
		
		Client();
		virtual ~Client();
		
		
		/**
		 * @brief List with implemented/available connection types.
		 * @return Type strings list.
		 */
		static std::list<std::string> availableConnectionTypes();
	};
}


#endif /* __FAYECPP_CLIENT_H__ */
