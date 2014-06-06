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


#include "client.h"
#include "message.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#ifdef HAVE_SUITABLE_QT_VERSION
#include "private/websocketqt.h"
#else
#include "private/websocket.h"
#endif

#include "private/transport.h"
#include "private/classmethodwrapper.h"
#include "private/jsonutils.h"
#include "private/REThread.h"
#include <assert.h>

namespace FayeCpp {
	
	unsigned long long Client::_messageId = 0;
	unsigned long long Client::nextMessageId()
	{
		_messageId++;
		if (_messageId == ULONG_LONG_MAX) _messageId = 1;
		return _messageId;
	}
	
	void Client::processMessage(Message * message)
	{
		if (message->isSuccessfully() && message->type() != MessageTypeNone)
		{
			switch (message->type())
			{
				case MessageTypeTransportConnected: this->onTransportConnected(message); break;
				case MessageTypeTransportDisconnected: this->onTransportDisconnected(message);  break;
				case MessageTypeTransportError: this->onClientError(message); break;
				case MessageTypeServerResponce: this->onClientMessageReceived(message); break;
				default: break;
			}
		}
		else
		{
			this->onClientError(message);
		}
	}
	
	void Client::onTransportConnected(Message * message)
	{
#ifdef DEBUG_QT
        qDebug() << "Client:" << "onTransportConnected.";
#endif
		if (_delegate) _delegate->onFayeTransportConnected(this);
		this->handshake();
		message = NULL;
	}
	
	void Client::onTransportDisconnected(Message * message)
	{
#ifdef DEBUG_QT
        qDebug() << "Client:" << "onTransportConnected.";
#endif
		if (_delegate) _delegate->onFayeTransportDisconnected(this);
        (void)message;
	}
	
	void Client::onClientError(Message * message)
	{
		if (_delegate) _delegate->onFayeErrorString(this, message->errorString().c_str());
	}
	
	void Client::onClientMessageReceived(Message * message)
	{
		if (_delegate) _delegate->onFayeClientWillReceiveMessage(this, message);
		switch (message->channelType())
		{
			case ChannelTypeHandshake: this->onHandshakeDone(message); break;
			case ChannelTypeConnect: this->onConnectFayeDone(message); break;
			case ChannelTypeDisconnect: this->onDisconnectFayeDone(message); break;
			case ChannelTypeSubscribe: this->onSubscriptionDone(message); break;
			case ChannelTypeUnsubscribe: this->onUnsubscribingDone(message); break;
			default:
				if (this->isSubscribedToChannel(message->channel()) && !message->data().empty())
				{
					if (_delegate) _delegate->onFayeClientReceivedDataFromChannel(this, message->data(), message->channel());
				}
				break;
		}
	}
	
	Delegate * Client::delegate()
	{
		return _delegate;
	}
	
	void Client::setDelegate(Delegate * delegate)
	{
		_delegate = delegate;
	}
	
	const std::list<std::string> & Client::subscribedChannels() const
	{
		return _subscribedChannels;
	}
	
	const std::list<std::string> & Client::supportedTransportNames() const
	{
		return _supportedConnectionTypes;
	}
	
	std::string Client::currentTransportName() const
	{
		return _transport->name();
	}
	
	const std::string & Client::clientId() const
	{
		return _clientId;
	}
	
    const std::string & Client::url() const
	{
		return _transport->url();
	}
	
    void Client::setUrl(const std::string & urlString)
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
	
	void Client::onHandshakeDone(Message * message)
	{
#ifdef DEBUG_QT
        qDebug() << "Client:" << "onHandshakeDone.";
#endif
		_clientId = message->clientId();
		if (!_clientId.empty())
		{
#ifdef DEBUG_QT
			qDebug() << "Client:" << "clientId=" << _clientId.c_str();
#endif
			std::list<std::string> availableTypes = Client::availableConnectionTypes();
			std::list<std::string> supportedTypes = message->connectionTypes();
			const std::string currentType = this->currentTransportName();
			bool isCurrentTypeFound = false;
			for (std::list<std::string>::const_iterator i = supportedTypes.begin(); i != supportedTypes.end(); ++i)
			{
				if (std::find(availableTypes.begin(), availableTypes.end(), *i) != availableTypes.end())
					if (currentType.compare(*i) == 0) isCurrentTypeFound = true;
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
					std::string error("Can't find implemented faye transport protocol type from supported by the server: [");
					unsigned int index = 0;
					for (std::list<std::string>::iterator i = supportedTypes.begin(); i != supportedTypes.end(); ++i)
					{
						if (index) error.append(", ");
						error.append(*i);
						index++;
					}
					error.append("]");
					_delegate->onFayeErrorString(this, error);
				}
			}
		}
	}
	
	void Client::handshake()
	{
#ifdef DEBUG_QT
        qDebug() << "Client:" << "handshake start...";
#endif
        Message message;
        message.addConnectionType(this->currentTransportName());
        //    message.addConnectionType("long-polling").addConnectionType("callback-polling").addConnectionType("iframe");
        message.setChannelType(ChannelTypeHandshake);
        message.setVersion("1.0");
        message.setMinimumVersion("1.0beta");
        if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
        _transport->sendText(message.toJsonString());
	}
	
	void Client::onConnectFayeDone(Message * message)
	{
		if (!_isFayeConnected)
		{
			_isFayeConnected = true;
			if (_delegate) _delegate->onFayeClientConnected(this);
			this->subscribePendingSubscriptions();
		}
		message = NULL;
	}
	
	void Client::connectFaye()
	{
#ifdef DEBUG_QT
        qDebug() << "Client:" << "connect faye start ...";
#endif
		Message message;
		message.setChannelType(ChannelTypeConnect);
		message.setClientId(_clientId);
		message.setConnectionType(this->currentTransportName());
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
        _transport->sendText(message.toJsonString());
	}
	
	void Client::disconnect()
	{
#ifdef DEBUG_QT
        qDebug() << "Client:" << "disconnect faye start ...";
#endif
		Message message;
		message.setChannelType(ChannelTypeDisconnect);
		message.setClientId(_clientId);
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
        _transport->sendText(message.toJsonString());
	}
	
	bool Client::isSubscribedToChannel(const std::string & channel) const
	{
		return channel.empty() ? false : (std::find(_subscribedChannels.begin(), _subscribedChannels.end(), channel) != _subscribedChannels.end());
	}
	
	void Client::onSubscriptionDone(Message * message)
	{
		const std::string channel = message->subscription();
		if (std::find(_pendingSubscriptions.begin(), _pendingSubscriptions.end(), channel) != _pendingSubscriptions.end())
		{
			_pendingSubscriptions.remove(channel);
			_subscribedChannels.push_back(channel);
			if (!_isFayeConnected)
			{
				_isFayeConnected = true;
				if (_delegate) _delegate->onFayeClientConnected(this);
			}
#ifdef DEBUG_QT
			qDebug() << "Client:" << "Subscribed to channel:" << channel.c_str();
#endif
			if (_delegate) _delegate->onFayeClientSubscribedToChannel(this, channel);
		}
	}
	
	void Client::onUnsubscribingDone(Message * message)
	{
		if (!message->subscription().empty())
		{
			_pendingSubscriptions.remove(message->subscription());
			_subscribedChannels.remove(message->subscription());
			if (_delegate) _delegate->onFayeClientUnsubscribedFromChannel(this, message->subscription());
		}
	}
	
	void Client::onDisconnectFayeDone(Message * message)
	{
		_subscribedChannels.clear();
		_pendingSubscriptions.clear();
		_isFayeConnected = false;
		if (_delegate) _delegate->onFayeClientDisconnected(this);
		message = NULL;
	}
	
	void Client::subscribePendingSubscriptions()
	{
		if (!_pendingSubscriptions.empty() && this->isTransportConnected())
		{
			if (!_clientId.empty() || _isFayeConnected)
			{
				std::list<std::string> arr = _pendingSubscriptions;
				for (std::list<std::string>::iterator i = arr.begin(); i != arr.end(); ++i)
				{
					Message message;
					message.setChannelType(ChannelTypeSubscribe).setClientId(_clientId).setSubscription(*i);
					if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
                    _transport->sendText(message.toJsonString());
				}
			}
		}
	}
	
	bool Client::subscribeToChannel(const std::string & channel)
	{
		if (channel.empty()) return false;
		if (this->isSubscribedToChannel(channel) || std::find(_pendingSubscriptions.begin(), _pendingSubscriptions.end(), channel) != _pendingSubscriptions.end()) return true;
		
		_pendingSubscriptions.push_back(channel);
		this->subscribePendingSubscriptions();
		return true;
	}
	
	bool Client::unsubscribeFromChannel(const std::string & channel)
	{
		if (channel.empty()) return false;
		if (!this->isSubscribedToChannel(channel) || std::find(_pendingSubscriptions.begin(), _pendingSubscriptions.end(), channel) == _pendingSubscriptions.end()) return false;
		
		Message message;
		message.setChannelType(ChannelTypeUnsubscribe).setClientId(_clientId).setSubscription(channel);
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
        _transport->sendText(message.toJsonString());
		return true;
	}
	
	void Client::unsubscribeAllChannels()
	{
		_pendingSubscriptions.clear();
		for (std::list<std::string>::iterator i = _subscribedChannels.begin(); i != _subscribedChannels.end(); ++i)
		{
			this->unsubscribeFromChannel(*i);
		}
	}
	
	bool Client::sendMessageToChannel(const std::map<std::string, Variant> & message, const std::string & channel)
	{
		if (_isFayeConnected && !message.empty() && this->isSubscribedToChannel(channel))
		{
#ifdef DEBUG_QT
			qDebug() << "Client:" << "Send message to channel:" << channel.c_str();
#endif
			std::map<std::string, Variant> mes;
			mes["channel"] = channel;
			mes["clientId"] = _clientId;
			mes["data"] = message;
			mes["id"] = Client::nextMessageId();
            _transport->sendText(JsonUtils::toJsonString(mes));
			return true;
		}
		return false;
	}
	
	Client::Client() :
	_transport(NULL),
	_delegate(NULL),
	_isFayeConnected(false)
	{
		REThread::mainThreadIdentifier();
		
#ifdef HAVE_SUITABLE_QT_VERSION
		_transport = new WebSocketQt(new ClassMethodWrapper<Client, void(Client::*)(Message*), Message>(this, &Client::processMessage));
#else
		_transport = new WebSocket(new ClassMethodWrapper<Client, void(Client::*)(Message*), Message>(this, &Client::processMessage));
#endif
        assert(_transport);
	}
	
	Client::~Client()
	{
		_delegate = NULL;
		delete _transport;
	}
	
	std::list<std::string> Client::availableConnectionTypes()
	{
		std::list<std::string> types;
		
#ifdef HAVE_SUITABLE_QT_VERSION
        types.push_back(WebSocketQt::transportName());
#else
        types.push_back(WebSocket::transportName());
#endif
		
		return types;
	}
	
}

