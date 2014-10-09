/*
 *   Copyright (c) 2014 Kulykov Oleh <nonamedemail@gmail.com>
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

#include "transport.h"
#include "classmethodwrapper.h"
#include "jsonutils.h"

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#define RE_ASSERT(r) assert(r);
#else
#define RE_ASSERT(r) r;
#endif

#define HANDSHAKE_CHANNEL "/meta/handshake"
#define CONNECT_CHANNEL "/meta/connect"
#define DISCONNECT_CHANNEL "/meta/disconnect"
#define SUBSCRIBE_CHANNEL "/meta/subscribe"
#define UNSUBSCRIBE_CHANNEL "/meta/unsubscribe"

namespace FayeCpp {
	
	bool Client::isUsingIPV6() const
	{
		return _isUsingIPV6;
	}
	
	bool Client::setUsingIPV6(bool isUse)
	{
		if (isUse && Client::isSupportsIPV6()) 
		{
			_isUsingIPV6 = true;
		}
		else
		{
			_isUsingIPV6 = false;
		}
		return _isUsingIPV6;
	}
	
    //TODO: ....
    //unsigned long long Client::
    unsigned long long _messageId = 0;
	unsigned long long Client::nextMessageId()
	{
		_messageId++;

#if defined(ULONG_LONG_MAX)		
		if (_messageId == ULONG_LONG_MAX) _messageId = 1;
#elif defined(ULLONG_MAX)
		if (_messageId == ULLONG_MAX) _messageId = 1;
#endif

		return _messageId;
	}
	
	void Client::processMessage(Responce * responce)
	{
		switch (responce->type())
		{
			case Responce::ResponceTransportConnected: this->onTransportConnected(); break;
			case Responce::ResponceTransportDisconnected: this->onTransportDisconnected();  break;
			case Responce::ResponceTransportError: this->onClientError(responce); break;
			case Responce::ResponceMessage: this->onClientResponceReceived(responce); break;
			default: this->onClientError(responce); break;
		}
	}
	
	void Client::onTransportConnected()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: onTransportConnected");
#endif
		if (_delegate) _delegate->onFayeTransportConnected(this);
		this->handshake();
	}
	
	void Client::onTransportDisconnected()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: onTransportDisconnected");
#endif
		_isDisconnecting = false;
		_isFayeConnected = false;
		
		_clientId.clear();
		_subscribedChannels.clear();
		_pendingSubscriptions.clear();
		_supportedConnectionTypes.clear();
		
		if (_delegate) _delegate->onFayeTransportDisconnected(this);
	}
	
	void Client::onClientError(Responce * message)
	{
		if (_delegate)
		{
			if (message->errorString())	_delegate->onFayeErrorString(this, *message->errorString());
			else _delegate->onFayeErrorString(this, REStaticString("Internal application error."));
		}
	}
	
	void Client::onReceivedMessageOnChannel(const VariantMap & message, const REString & channel)
	{
		if (_delegate) 
		{
			Variant * data = message.findTypedValue("data", Variant::TypeMap);
			if (data) 
			{
				_delegate->onFayeClientReceivedMessageFromChannel(this, data->toMap(), channel);
			}
		}
	}
	
	void Client::onClientResponceMessageReceived(const VariantMap & message)
	{
		VariantMap::Iterator i = message.iterator();
		while (i.next()) 
		{
			if (i.key().isEqual("channel")) 
			{
				REString channel = i.value().toString();
				if (channel.isEqual(HANDSHAKE_CHANNEL)) 
				{
					this->onHandshakeDone(message);
				}
				else if (channel.isEqual(CONNECT_CHANNEL)) 
				{
					this->onConnectFayeDone(message);
				}
				else if (channel.isEqual(DISCONNECT_CHANNEL)) 
				{
					this->onDisconnectFayeDone(message);
				}
				else if (channel.isEqual(SUBSCRIBE_CHANNEL)) 
				{
					this->onSubscriptionDone(message);
				}
				else if (channel.isEqual(UNSUBSCRIBE_CHANNEL)) 
				{
					this->onUnsubscribingDone(message);
				}
				else if (_subscribedChannels.isContaines(channel))
				{
					this->onReceivedMessageOnChannel(message, channel);
				}
			}
		}
	}

	void Client::onClientResponceMessagesListReceived(const VariantList & messagesList)
	{
		VariantList::Iterator i = messagesList.iterator();
		while (i.next()) 
		{
			switch (i.value().type()) 
			{
				case Variant::TypeMap:
					this->onClientResponceMessageReceived(i.value().toMap());
					break;
				case Variant::TypeList:
					this->onClientResponceMessagesListReceived(i.value().toList());
					break;
				default:
					break;
			}
		}
	}
	
	void Client::onClientResponceReceived(Responce * responce)
	{
		if (responce->messageMap()) 
		{
			this->onClientResponceMessageReceived(*responce->messageMap());
		}
		if (responce->messageList())
		{
			this->onClientResponceMessagesListReceived(*responce->messageList());
		}
		if (responce->messageBuffer())
		{
			//TODO: process unknown buffer data.
		}
	}
	
	Delegate * Client::delegate() const
	{
		return _delegate;
	}
	
	void Client::setDelegate(Delegate * delegate)
	{
		_delegate = delegate;
	}

	SSLDataSource * Client::sslDataSource() const
	{
		return _sslDataSource;
	}
	
	void Client::setSSLDataSource(SSLDataSource * dataSource)
	{
		_sslDataSource = dataSource;
	}
	
	const REStringList & Client::subscribedChannels() const
	{
		return _subscribedChannels;
	}
	
	const REStringList & Client::supportedTransportNames() const
	{
		return _supportedConnectionTypes;
	}
	
	REString Client::currentTransportName() const
	{
		return _transport->name();
	}
	
	const REString & Client::clientId() const
	{
		return _clientId;
	}
	
    const REString & Client::url() const
	{
		return _transport->url();
	}
	
    void Client::setUrl(const char * urlString)
	{
		_transport->setUrl(urlString);
	}
	
	bool Client::connect()
	{
		if (!this->isTransportConnected())
		{
			_transport->connectToServer();
			return true;
		}
		return false;
	}
	
	bool Client::isTransportConnected() const
	{
        return _transport ? _transport->isConnected() : false;
	}
	
	bool Client::isFayeConnected() const
	{
		return _isFayeConnected;
	}
	
	void Client::onHandshakeDone(const VariantMap & message)
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: onHandshakeDone");
#endif
		
		VariantMap::Iterator i = message.iterator();
		while (i.next()) 
		{
			if (i.key().isEqual("clientId") && i.value().isString())
			{
				_clientId = i.value().toString();
			}
			else if (i.key().isEqual("supportedConnectionTypes") && i.value().isList())
			{
				VariantList::Iterator j = i.value().toList().iterator();
				while (j.next()) _supportedConnectionTypes.add(j.value().toString());
			}
		}
		
		if (_clientId.isEmpty()) 
		{
			if (_delegate) _delegate->onFayeErrorString(this, REStaticString("Handshake clientId is empty."));
			return;
		}
		
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: clientId=%s", _clientId.UTF8String());
#endif
		
		if (_supportedConnectionTypes.isEmpty()) 
		{
			if (_delegate) _delegate->onFayeErrorString(this, REStaticString("Handshake supported connection types is empty."));
			return;
		}
		
		REStringList availableTypes = Client::availableConnectionTypes();
		const REString currentType = this->currentTransportName();
		bool isCurrentTypeFound = false;
		REStringList::Iterator j = _supportedConnectionTypes.iterator();
		while (!isCurrentTypeFound && j.next()) 
		{
			isCurrentTypeFound = availableTypes.isContaines(j.value()) && currentType.isEqual(j.value());
		}
		
		if (isCurrentTypeFound)
		{
			this->connectFaye();
			this->subscribePendingSubscriptions();
		}
		else
		{
			if (_delegate)
			{
				REMutableString error("Can't find implemented faye transport protocol type from supported by the server: [");
				unsigned int index = 0;
				j = _supportedConnectionTypes.iterator();
				while (i.next()) 
				{
					if (index) error.append(", ");
					error.append(i.value().toString());
					index++;
				}
				error.append("]");
				_delegate->onFayeErrorString(this, error);
			}
		}
	}
	
	void Client::handshake()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: handshake start...");
#endif
		VariantMap message;
		VariantList connectionTypes;
		connectionTypes.add(this->currentTransportName());
		message["supportedConnectionTypes"] = connectionTypes;
		message["minimumVersion"] = "1.0beta";
		message["channel"] = HANDSHAKE_CHANNEL;
		message["version"] = "1.0";
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		
		this->sendText(JsonGenerator(message).string());
	}
	
	void Client::onConnectFayeDone(const VariantMap & message)
	{
		if (!_isFayeConnected)
		{
			_isFayeConnected = true;
			_isDisconnecting = false;

			if (_delegate) _delegate->onFayeClientConnected(this);
			this->subscribePendingSubscriptions();
		}
		
		Variant * advice = message.findTypedValue("advice", Variant::TypeMap);
		if (advice && _transport) _transport->receivedAdvice(advice->toMap());
	}
	
	void Client::connectFaye()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: connect faye start ...");
#endif
		VariantMap message;
		message["channel"] = CONNECT_CHANNEL;
		message["clientId"] = _clientId;
		message["connectionType"] = this->currentTransportName();
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		
		this->sendText(JsonGenerator(message).string());
	}
	
	void Client::disconnect()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: disconnect faye start ...");
#endif
		_isDisconnecting = true;

		VariantMap message;
		message["channel"] = DISCONNECT_CHANNEL;
		message["clientId"] = _clientId;
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		
		this->sendText(JsonGenerator(message).string());
	}
	
	bool Client::isDisconnecting() const
	{
		return _isDisconnecting;
	}

	bool Client::isSubscribedToChannel(const char * channel) const
	{
		return channel ? _subscribedChannels.isContaines(REString(channel)) : false;
	}
	
	bool Client::isPendingChannel(const char * channel) const
	{
		return channel ? _pendingSubscriptions.isContaines(REString(channel)) : false;
	}
	
	void Client::onSubscriptionDone(const VariantMap & message)
	{
		Variant * channel = message.findTypedValue("subscription", Variant::TypeString);
		if (!channel) 
		{
			//TODO: error;
			return;
		}
		
		if (_pendingSubscriptions.isContaines(channel->toString())) 
		{
			Variant * advice = message.findTypedValue("advice", Variant::TypeMap);
			if (advice && _transport) _transport->receivedAdvice(advice->toMap());
			
			REStringList::Node * node = _pendingSubscriptions.findNode(channel->toString());
			if (node) _pendingSubscriptions.removeNode(node);
			
			_subscribedChannels.add(channel->toString());
			
			if (!_isFayeConnected)
			{
				_isFayeConnected = true;
				if (_delegate) _delegate->onFayeClientConnected(this);
			}
			
#ifdef FAYECPP_DEBUG_MESSAGES
			RELog::log("Client: Subscribed to channel: %s", channel->toString().UTF8String());
#endif
			if (_delegate) _delegate->onFayeClientSubscribedToChannel(this, channel->toString());
		}
	}
	
	void Client::onUnsubscribingDone(const VariantMap & message)
	{
		Variant * channel = message.findTypedValue("subscription", Variant::TypeString);
		if (!channel) 
		{
			//TODO: error;
			return;
		}
		
		REStringList::Node * node = _pendingSubscriptions.findNode(channel->toString());
		if (node) _pendingSubscriptions.removeNode(node);
		
		node = _subscribedChannels.findNode(channel->toString());
		if (node) _subscribedChannels.removeNode(node);
		
		if (_delegate) _delegate->onFayeClientUnsubscribedFromChannel(this, channel->toString());
	}
	
	void Client::onDisconnectFayeDone(const VariantMap & message)
	{
		_subscribedChannels.clear();
		_pendingSubscriptions.clear();
		_isFayeConnected = false;

		if (_delegate) _delegate->onFayeClientDisconnected(this);
		_transport->disconnectFromServer();
		(void)message;
	}
	
	void Client::subscribePendingSubscriptions()
	{
		if (!_pendingSubscriptions.isEmpty() && this->isTransportConnected())
		{
			if (_clientId.isNotEmpty() || _isFayeConnected)
			{
				REStringList arr = _pendingSubscriptions;
				REStringList::Iterator i = arr.iterator();
				while (i.next()) 
				{
					VariantMap message;
					message["channel"] = SUBSCRIBE_CHANNEL;
					message["clientId"] = _clientId;
					message["subscription"] = i.value();
					if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);

					this->sendText(JsonGenerator(message).string());
				}
			}
		}
	}
	
	bool Client::subscribeToChannel(const char * channel)
	{
		if (!channel) 
		{
			return false;
		}
		if (this->isSubscribedToChannel(channel) || this->isPendingChannel(channel)) 
		{
			return true;
		}
		
		_pendingSubscriptions.add(REString(channel));
		this->subscribePendingSubscriptions();
		return true;
	}
	
	bool Client::unsubscribeFromChannel(const char * channel)
	{
		if (!channel)
		{
			return false;
		}
		if (!this->isSubscribedToChannel(channel) || this->isPendingChannel(channel)) 
		{
			return false;
		}
		
		VariantMap message;
		message["channel"] = UNSUBSCRIBE_CHANNEL;
		message["clientId"] = _clientId;
		message["subscription"] = channel;
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		
		return this->sendText(JsonGenerator(message).string());
	}
	
	void Client::unsubscribeAllChannels()
	{
		_pendingSubscriptions.clear();
		REStringList::Iterator i = _subscribedChannels.iterator();
		while (i.next()) 
		{
			this->unsubscribeFromChannel(i.value().UTF8String());
		}
	}
	
	bool Client::sendText(const char * text, const REUInt32 textLenght)
	{
		if (text && _transport) 
		{
			const REUInt32 len = textLenght == RENotFound ? (REUInt32)strlen(text) : textLenght;
			_transport->sendText(text, len);
			return true;
		}
		return false;
	}
	
	bool Client::sendMessageToChannel(const VariantMap & message, const char * channel)
	{
		if (_isFayeConnected && !message.isEmpty() && this->isSubscribedToChannel(channel))
		{
#ifdef FAYECPP_DEBUG_MESSAGES	
			RELog::log("Client: Send message to channel: %s", channel);
#endif
			VariantMap mes;
			mes["channel"] = channel;
			mes["clientId"] = _clientId;
			mes["data"] = message;
			mes["id"] = Client::nextMessageId();
			
			return this->sendText(JsonGenerator(mes).string());
		}
		return false;
	}
	
	Client::Client() :
		_transport(NULL),
		_delegate(NULL),
		_sslDataSource(NULL),
		_isFayeConnected(false),
		_isDisconnecting(false),
		_isUsingIPV6(false)
	{
        _transport = Transport::createNewTransport(new ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce>(this, &Client::processMessage));
#if defined(HAVE_ASSERT_H)	
        assert(_transport);
#endif
	}
	
	Client::~Client()
	{
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: descructor ~Client() ...");
#endif
		_delegate = NULL;
		
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: try delete transport ...");
#endif
		
		if (_transport) 
		{
			if (!_transport->isMainThread()) 
			{
				RELog::log("FayeCpp client error: you are trying to delete client transport not in the same thread that was created!");
#if defined(HAVE_ASSERT_H) 	
				assert(0);
#endif	
			}
			
			_transport->disconnectFromServer();
			delete _transport;
			_transport = NULL;
		}
		
#ifdef FAYECPP_DEBUG_MESSAGES
		RELog::log("Client: delete transport OK");
		RELog::log("Client: descructor ~Client() OK");
#endif
	}
	
	REStringList Client::availableConnectionTypes()
	{
        return Transport::availableConnectionTypes();
	}
	
	bool Client::isSupportsIPV6()
	{
		return Transport::isSupportsIPV6();
	}
	
	bool Client::isSupportsSSLConnection()
	{
		return Transport::isSupportsSSLConnection();
	}
}

