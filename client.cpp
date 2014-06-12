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

#include "private/transport.h"
#include "private/classmethodwrapper.h"
#include "private/jsonutils.h"
#include "private/REThread.h"
#include <assert.h>

#if defined(HAVE_SUITABLE_QT_VERSION) && defined(FAYECPP_DEBUG_MESSAGES)
#include <QDebug>
#endif

#if defined(HAVE_DISPATCH_DISPATCH_H)
#include <dispatch/dispatch.h>
#endif

namespace FayeCpp {
	
	unsigned long long Client::_messageId = 0;
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
	
	void Client::processMessage(Message * message)
	{
        if (message->isSuccessfully() && message->type() != Message::MessageTypeNone)
		{
			switch (message->type())
			{
                case Message::MessageTypeTransportConnected: this->onTransportConnected(message); break;
                case Message::MessageTypeTransportDisconnected: this->onTransportDisconnected(message);  break;
                case Message::MessageTypeTransportError: this->onClientError(message); break;
                case Message::MessageTypeServerResponce: this->onClientMessageReceived(message); break;
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
#ifdef FAYECPP_DEBUG_MESSAGES
#ifdef HAVE_SUITABLE_QT_VERSION
        qDebug() << "Client:" << "onTransportConnected.";
#else
		fprintf(stderr, "Client: onTransportConnected\n");
#endif		
#endif
		
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
		dispatch_async(dispatch_get_main_queue(), ^{
#endif
		if (_delegate) _delegate->onFayeTransportConnected(this);
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
		});
#endif			
		this->handshake();
		(void)message;
	}
	
	void Client::onTransportDisconnected(Message * message)
	{
#ifdef FAYECPP_DEBUG_MESSAGES		
#ifdef HAVE_SUITABLE_QT_VERSION
        qDebug() << "Client:" << "onTransportDisconnected.";
#else
		fprintf(stderr, "Client: onTransportDisconnected\n");
#endif		
#endif
		
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
		dispatch_async(dispatch_get_main_queue(), ^{
#endif
		if (_delegate) _delegate->onFayeTransportDisconnected(this);
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
		});
#endif
        (void)message;
	}
	
	void Client::onClientError(Message * message)
	{
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
		dispatch_async(dispatch_get_main_queue(), ^{
#endif
		if (_delegate) _delegate->onFayeErrorString(this, message->errorString());
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
		});
#endif
	}
	
	void Client::onClientMessageReceived(Message * message)
	{
		if (_delegate) _delegate->onFayeClientWillReceiveMessage(this, message);
		switch (message->channelType())
		{
            case Message::ChannelTypeHandshake: this->onHandshakeDone(message); break;
            case Message::ChannelTypeConnect: this->onConnectFayeDone(message); break;
            case Message::ChannelTypeDisconnect: this->onDisconnectFayeDone(message); break;
            case Message::ChannelTypeSubscribe: this->onSubscriptionDone(message); break;
            case Message::ChannelTypeUnsubscribe: this->onUnsubscribingDone(message); break;
			default:
				if (this->isSubscribedToChannel(message->channel()) && !message->data().empty())
				{
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
					dispatch_async(dispatch_get_main_queue(), ^{
#endif
					if (_delegate) _delegate->onFayeClientReceivedDataFromChannel(this, message->data(), message->channel());
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
					});
#endif
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
#ifdef FAYECPP_DEBUG_MESSAGES
#ifdef HAVE_SUITABLE_QT_VERSION
        qDebug() << "Client:" << "onHandshakeDone.";
#else
		fprintf(stderr, "Client: onHandshakeDone\n");
#endif
#endif		
		_clientId = message->clientId();
		if (!_clientId.empty())
		{
#ifdef FAYECPP_DEBUG_MESSAGES			
#ifdef HAVE_SUITABLE_QT_VERSION
			qDebug() << "Client:" << "clientId=" << _clientId.c_str();
#else	
			fprintf(stderr, "Client: clientId=%s\n", _clientId.c_str());
#endif
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
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
					dispatch_async(dispatch_get_main_queue(), ^{
#endif					
					_delegate->onFayeErrorString(this, error);
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
					});
#endif
				}
			}
		}
	}
	
	void Client::handshake()
	{
#ifdef FAYECPP_DEBUG_MESSAGES				
#ifdef HAVE_SUITABLE_QT_VERSION
        qDebug() << "Client:" << "handshake start...";
#else
		fprintf(stderr, "Client: handshake start...\n");
#endif
#endif		
        Message message;
        message.addConnectionType(this->currentTransportName());
        //    message.addConnectionType("long-polling").addConnectionType("callback-polling").addConnectionType("iframe");
        message.setChannelType(Message::ChannelTypeHandshake);
        message.setVersion("1.0");
        message.setMinimumVersion("1.0beta");
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
		char * jsonCString = message.jsonCString();
		if (jsonCString) 
		{
			_transport->sendText(jsonCString, strlen(jsonCString));
			free(jsonCString);
		}
	}
	
	void Client::onConnectFayeDone(Message * message)
	{
		if (!_isFayeConnected)
		{
			_isFayeConnected = true;
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
			dispatch_async(dispatch_get_main_queue(), ^{
#endif					
			if (_delegate) _delegate->onFayeClientConnected(this);
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
			});
#endif
			this->subscribePendingSubscriptions();
		}
		(void)message;
	}
	
	void Client::connectFaye()
	{
#ifdef FAYECPP_DEBUG_MESSAGES	
#ifdef HAVE_SUITABLE_QT_VERSION
        qDebug() << "Client:" << "connect faye start ...";
#else
		fprintf(stderr, "Client: connect faye start ...\n");
#endif		
#endif
		Message message;
        message.setChannelType(Message::ChannelTypeConnect);
		message.setClientId(_clientId);
		message.setConnectionType(this->currentTransportName());
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
		char * jsonCString = message.jsonCString();
		if (jsonCString) 
		{
			_transport->sendText(jsonCString, strlen(jsonCString));
			free(jsonCString);
		}
	}
	
	void Client::disconnect()
	{
#ifdef FAYECPP_DEBUG_MESSAGES			
#ifdef HAVE_SUITABLE_QT_VERSION
        qDebug() << "Client:" << "disconnect faye start ...";
#else
		fprintf(stderr, "Client: disconnect faye start ...\n");
#endif
#endif		
		Message message;
        message.setChannelType(Message::ChannelTypeDisconnect);
		message.setClientId(_clientId);
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
		char * jsonCString = message.jsonCString();
		if (jsonCString) 
		{
			_transport->sendText(jsonCString, strlen(jsonCString));
			free(jsonCString);
		}
	}
	
	bool Client::isSubscribedToChannel(const std::string & channel) const
	{
		return channel.empty() ? false : (std::find(_subscribedChannels.begin(), _subscribedChannels.end(), channel) != _subscribedChannels.end());
	}
	
	bool Client::isPendingChannel(const std::string & channel) const
	{
		return channel.empty() ? false : (std::find(_pendingSubscriptions.begin(), _pendingSubscriptions.end(), channel) != _pendingSubscriptions.end());
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
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
				dispatch_async(dispatch_get_main_queue(), ^{
#endif	
				if (_delegate) _delegate->onFayeClientConnected(this);
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
				});
#endif
			}
#ifdef FAYECPP_DEBUG_MESSAGES					
#ifdef HAVE_SUITABLE_QT_VERSION
			qDebug() << "Client:" << "Subscribed to channel:" << channel.c_str();
#else
			fprintf(stderr, "Client: Subscribed to channel: %s\n", channel.c_str());
#endif
#endif			
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
			dispatch_async(dispatch_get_main_queue(), ^{
#endif	
			if (_delegate) _delegate->onFayeClientSubscribedToChannel(this, channel);
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
			});
#endif
		}
	}
	
	void Client::onUnsubscribingDone(Message * message)
	{
		if (!message->subscription().empty())
		{
			_pendingSubscriptions.remove(message->subscription());
			_subscribedChannels.remove(message->subscription());
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
			dispatch_async(dispatch_get_main_queue(), ^{
#endif	
			if (_delegate) _delegate->onFayeClientUnsubscribedFromChannel(this, message->subscription());
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
			});
#endif
		}
	}
	
	void Client::onDisconnectFayeDone(Message * message)
	{
		_subscribedChannels.clear();
		_pendingSubscriptions.clear();
		_isFayeConnected = false;
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)
		dispatch_async(dispatch_get_main_queue(), ^{
#endif	
		if (_delegate) _delegate->onFayeClientDisconnected(this);
#if defined(HAVE_DISPATCH_DISPATCH_H) && defined(HAVE_FUNCTION_DISPATCH_ASYNC)			
		});
#endif
		(void)message;
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
                    message.setChannelType(Message::ChannelTypeSubscribe).setClientId(_clientId).setSubscription(*i);
					if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
					char * jsonCString = message.jsonCString();
					if (jsonCString) 
					{
						_transport->sendText(jsonCString, strlen(jsonCString));
						free(jsonCString);
					}
				}
			}
		}
	}
	
	bool Client::subscribeToChannel(const std::string & channel)
	{
		if (channel.empty()) 
		{
			return false;
		}
		if (this->isSubscribedToChannel(channel) || this->isPendingChannel(channel)) 
		{
			return true;
		}
		
		_pendingSubscriptions.push_back(channel);
		this->subscribePendingSubscriptions();
		return true;
	}
	
	bool Client::unsubscribeFromChannel(const std::string & channel)
	{
		if (channel.empty())
		{
			return false;
		}
		if (!this->isSubscribedToChannel(channel) || this->isPendingChannel(channel)) 
		{
			return false;
		}
		
		Message message;
        message.setChannelType(Message::ChannelTypeUnsubscribe).setClientId(_clientId).setSubscription(channel);
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, &message);
		char * jsonCString = message.jsonCString();
		if (jsonCString)
		{
			_transport->sendText(jsonCString, strlen(jsonCString));
			free(jsonCString);
			return true;
		}
		return false;
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
#ifdef FAYECPP_DEBUG_MESSAGES				
#ifdef HAVE_SUITABLE_QT_VERSION
			qDebug() << "Client:" << "Send message to channel:" << channel.c_str();
#else
			fprintf(stderr, "Client: Send message to channel: %s\n", channel.c_str());
#endif
#endif			
			std::map<std::string, Variant> mes;
			mes["channel"] = channel;
			mes["clientId"] = _clientId;
			mes["data"] = message;
			mes["id"] = Client::nextMessageId();
			char * jsonCString = JsonUtils::toJsonCString(mes);
			if (jsonCString)
			{
				_transport->sendText(jsonCString, strlen(jsonCString));
				free(jsonCString);
				return true;
			}
		}
		return false;
	}
	
	Client::Client() :
	_transport(NULL),
	_delegate(NULL),
	_isFayeConnected(false)
	{
		REThread::mainThreadIdentifier();
		
        _transport = Transport::createNewTransport(new ClassMethodWrapper<Client, void(Client::*)(Message*), Message>(this, &Client::processMessage));
        assert(_transport);
	}
	
	Client::~Client()
	{
		_delegate = NULL;
		delete _transport;
	}
	
	std::list<std::string> Client::availableConnectionTypes()
	{
        return Transport::availableConnectionTypes();
	}
	
}

