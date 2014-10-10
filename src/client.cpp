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
#define RE_ASSERT(r) assert(r)
#else
#define RE_ASSERT(r) r
#endif

#if defined(FAYECPP_DEBUG_MESSAGES)
#define FAYECPP_DEBUG_LOG(s) RELog::log(s);
#define FAYECPP_DEBUG_LOGA(s, ...) RELog::log(s, __VA_ARGS__);
#else
#define FAYECPP_DEBUG_LOG(s)
#define FAYECPP_DEBUG_LOGA(s, ...)
#endif

namespace FayeCpp {

	/* Bayeux protocol constants */
	/* Channels */
	static const char * const _bayeuxHandshakeChannel = "/meta/handshake";
	static const char * const _bayeuxConnectChannel = "/meta/connect";
	static const char * const _bayeuxDisconnectChannel = "/meta/disconnect";
	static const char * const _bayeuxSubscribeChannel = "/meta/subscribe";
	static const char * const _bayeuxUnsubscribeChannel = "/meta/unsubscribe";

	/* Keys */
	static const char * const _bayeuxChannelKey = "channel";
	static const char * const _bayeuxClientIdKey = "clientId";
	static const char * const _bayeuxDataKey = "data";
	static const char * const _bayeuxSubscriptionKey = "subscription";
	static const char * const _bayeuxSupportedConnectionTypesKey = "supportedConnectionTypes";
	static const char * const _bayeuxConnectionTypeKey = "connectionType";
	static const char * const _bayeuxIdKey = "id";
	static const char * const _bayeuxAdviceKey = "advice";
	static const char * const _bayeuxMinimumVersionKey = "minimumVersion";
	static const char * const _bayeuxVersionKey = "version";

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
	
    static unsigned int __client_messageId = 0;
	unsigned int Client::nextMessageId()
	{
		__client_messageId++;
#if defined(UINT_MAX)
		if (__client_messageId == UINT_MAX) __client_messageId = 1;
#else
		/// more than enougth
		if (__client_messageId == 9999999) __client_messageId = 1;
#endif
		return __client_messageId;
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
		FAYECPP_DEBUG_LOG("Client: onTransportConnected")

		if (_delegate) _delegate->onFayeTransportConnected(this);
		
		if (_transport) this->handshake();
	}
	
	void Client::onTransportDisconnected()
	{
		FAYECPP_DEBUG_LOG("Client: onTransportDisconnected")

		_isDisconnecting = false;
		_isFayeConnected = false;
		
		_clientId.clear();
		_subscribedChannels.clear();
		_pendingSubscriptions.clear();
		_supportedConnectionTypes.clear();
		
		Transport * unusedTransport = _transport; _transport = NULL;
		Transport::deleteTransport(unusedTransport);
		
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
			Variant * data = message.findTypedValue(_bayeuxDataKey, Variant::TypeMap);
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
			if (i.key().isEqual(_bayeuxChannelKey))
			{
				REString channel = i.value().toString();
				if (channel.isEqual(_bayeuxHandshakeChannel))
				{
					this->onHandshakeDone(message);
				}
				else if (channel.isEqual(_bayeuxConnectChannel))
				{
					this->onConnectFayeDone(message);
				}
				else if (channel.isEqual(_bayeuxDisconnectChannel))
				{
					this->onDisconnectFayeDone(message);
				}
				else if (channel.isEqual(_bayeuxSubscribeChannel))
				{
					this->onSubscriptionDone(message);
				}
				else if (channel.isEqual(_bayeuxUnsubscribeChannel))
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
	
	const REString & Client::clientId() const
	{
		return _clientId;
	}
	
    const REString & Client::url() const
	{
		return _url;
	}
	
	void Client::parseURL(Client * client)
	{
		REMutableString urlString(client->_url.UTF8String());
		
		if (urlString.isContaines("ws://"))
		{
			urlString.replace("ws://");
			client->_isUseSSL = false;
		}
		
		if (urlString.isContaines("wss://"))
		{
			urlString.replace("wss://");
			client->_isUseSSL = true;
		}
		
		if (urlString.isContaines("http://"))
		{
			urlString.replace("http://");
#if defined(HAVE_SUITABLE_QT_VERSION)
			REMutableString u(url);
			u.replace("http://", "ws://");
			client->_url = u.UTF8String();
#endif
			client->_isUseSSL = false;
		}
		
		if (urlString.isContaines("https://"))
		{
			urlString.replace("https://");
#if defined(HAVE_SUITABLE_QT_VERSION)
			REMutableString u(url);
			u.replace("https://", "wss://");
			client->_url = u.UTF8String();
#endif
			client->_isUseSSL = true;
		}
		
		const char * sub = strstr(urlString.UTF8String(), ":");
		if (sub)
		{
			int port = -1;
			if (sscanf(++sub, "%i", &port) == 1)
			{
				client->_port = port;
			}
		}
		
		sub = strstr(urlString.UTF8String(), "/");
		if (sub)
		{
			client->_path = sub;
		}
		else
		{
			client->_path = "/";
		}
		
		sub = strstr(urlString.UTF8String(), ":");
		if (!sub) sub = strstr(urlString.UTF8String(), "/");
		if (sub)
		{
			const REUInt32 len = (REUInt32)(sub - urlString.UTF8String());
			client->_host = REString(urlString.UTF8String(), len);
		}
		else
		{
			client->_host = urlString.UTF8String();
		}
	}
	
    void Client::setUrl(const char * url)
	{
		_url = url;
		Client::parseURL(this);
	}
	
	const REString & Client::host() const
	{
		return _host;
	}
	
	const REString & Client::path() const
	{
		return _path;
	}
	
	int Client::port() const
	{
		return _port;
	}
	
	bool Client::isUseSSL() const
	{
		return _isUseSSL;
	}
	
	bool Client::connect()
	{
		if (!this->isTransportConnected())
		{
			Transport * unusedTransport = _transport; _transport = NULL;
			Transport::deleteTransport(unusedTransport);
			
			_transport = Transport::createNewTransport(new ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce>(this, &Client::processMessage));
#if defined(HAVE_ASSERT_H)	
			assert(_transport);
#endif
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
		FAYECPP_DEBUG_LOG("Client: onHandshakeDone")
		
		VariantMap::Iterator i = message.iterator();
		while (i.next()) 
		{
			if (i.key().isEqual(_bayeuxClientIdKey) && i.value().isString())
			{
				_clientId = i.value().toString();
			}
			else if (i.key().isEqual(_bayeuxSupportedConnectionTypesKey) && i.value().isList())
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

		FAYECPP_DEBUG_LOGA("Client: clientId=%s", _clientId.UTF8String())
		
		if (_supportedConnectionTypes.isEmpty()) 
		{
			if (_delegate) _delegate->onFayeErrorString(this, REStaticString("Handshake supported connection types is empty."));
			return;
		}
		
		REStringList availableTypes = Client::availableConnectionTypes();
		const REString currentType = _transport->name();
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
		if (!_transport) return;

		FAYECPP_DEBUG_LOG("Client: handshake start...")

		VariantMap message;
		VariantList connectionTypes;
		connectionTypes.add(_transport->name());
		message[_bayeuxSupportedConnectionTypesKey] = connectionTypes;
		message[_bayeuxMinimumVersionKey] = "1.0beta";
		message[_bayeuxChannelKey] = _bayeuxHandshakeChannel;
		message[_bayeuxVersionKey] = "1.0";
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		if (!_transport) return;
		
		this->sendText(JsonGenerator(message).string());
	}
	
	void Client::onConnectFayeDone(const VariantMap & message)
	{
		if (!_isFayeConnected)
		{
			_isFayeConnected = true;
			_isDisconnecting = false;

			if (_delegate) _delegate->onFayeClientConnected(this);
			if (!_transport) return;
			this->subscribePendingSubscriptions();
		}
		
		Variant * advice = message.findTypedValue(_bayeuxAdviceKey, Variant::TypeMap);
		if (advice && _transport) _transport->receivedAdvice(advice->toMap());
	}
	
	void Client::connectFaye()
	{
		if (!_transport) return;

		FAYECPP_DEBUG_LOG("Client: connect faye start ...")

		VariantMap message;
		message[_bayeuxChannelKey] = _bayeuxConnectChannel;
		message[_bayeuxClientIdKey] = _clientId;
		message[_bayeuxConnectionTypeKey] = _transport->name();
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		if (!_transport) return;
		
		this->sendText(JsonGenerator(message).string());
	}
	
	void Client::disconnect()
	{
		FAYECPP_DEBUG_LOG("Client: disconnect faye start ...")

		_isDisconnecting = true;

		VariantMap message;
		message[_bayeuxChannelKey] = _bayeuxDisconnectChannel;
		message[_bayeuxClientIdKey] = _clientId;
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		if (!_transport) return;
		
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
		Variant * channel = message.findTypedValue(_bayeuxSubscriptionKey, Variant::TypeString);
		if (!channel) 
		{
			//TODO: error;
			return;
		}
		
		if (_pendingSubscriptions.isContaines(channel->toString())) 
		{
			Variant * advice = message.findTypedValue(_bayeuxAdviceKey, Variant::TypeMap);
			if (advice && _transport) _transport->receivedAdvice(advice->toMap());
			
			REStringList::Node * node = _pendingSubscriptions.findNode(channel->toString());
			if (node) _pendingSubscriptions.removeNode(node);
			
			_subscribedChannels.add(channel->toString());
			
			if (!_isFayeConnected)
			{
				_isFayeConnected = true;
				if (_delegate) _delegate->onFayeClientConnected(this);
				if (!_transport) return;
			}

			FAYECPP_DEBUG_LOGA("Client: Subscribed to channel: %s", channel->toString().UTF8String())

			if (_delegate) _delegate->onFayeClientSubscribedToChannel(this, channel->toString());
		}
	}
	
	void Client::onUnsubscribingDone(const VariantMap & message)
	{
		Variant * channel = message.findTypedValue(_bayeuxSubscriptionKey, Variant::TypeString);
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
		
		Transport * unusedTransport = _transport; _transport = NULL;
		Transport::deleteTransport(unusedTransport);
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
					message[_bayeuxChannelKey] = _bayeuxSubscribeChannel;
					message[_bayeuxClientIdKey] = _clientId;
					message[_bayeuxSubscriptionKey] = i.value();
					if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
					if (!_transport) return;
					
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
		message[_bayeuxChannelKey] = _bayeuxUnsubscribeChannel;
		message[_bayeuxClientIdKey] = _clientId;
		message[_bayeuxSubscriptionKey] = channel;
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		if (!_transport) return false;
		
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
			FAYECPP_DEBUG_LOGA("Client: Send message to channel: %s", channel)

			VariantMap mes;
			mes[_bayeuxChannelKey] = channel;
			mes[_bayeuxClientIdKey] = _clientId;
			mes[_bayeuxDataKey] = message;
			mes[_bayeuxIdKey] = (unsigned long long)Client::nextMessageId();
			
			return this->sendText(JsonGenerator(mes).string());
		}
		return false;
	}
	
	Client::Client() :
		_transport(NULL),
		_delegate(NULL),
		_sslDataSource(NULL),
		_port(0),
		_isUseSSL(false),
		_isFayeConnected(false),
		_isDisconnecting(false),
		_isUsingIPV6(false)
	{
       
	}
	
	Client::~Client()
	{
		FAYECPP_DEBUG_LOG("Client: descructor ~Client() ...")

		_delegate = NULL;

		FAYECPP_DEBUG_LOG("Client: try delete transport ...")
		
		Transport * unusedTransport = _transport; _transport = NULL;
		Transport::deleteTransport(unusedTransport);

		FAYECPP_DEBUG_LOG("Client: delete transport OK")
		FAYECPP_DEBUG_LOG("Client: descructor ~Client() OK")
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

