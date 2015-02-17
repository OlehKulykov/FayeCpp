/*
 *   Copyright (c) 2014 - 2015 Kulykov Oleh <nonamedemail@gmail.com>
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

#if defined(HAVE_SUITABLE_QT_VERSION)
#include <QObject>
#endif

#include "transport.h"
#include "classmethodwrapper.h"
#include "jsonutils.h"

#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif

#if defined(HAVE_TIME_H)
#include <time.h>
#endif

#if defined(HAVE_MACH_MACH_TIME_H)
#include <mach/mach_time.h>
#endif

#if defined(HAVE_LIBWEBSOCKETS_H)
#include <libwebsockets.h>
#endif

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


#if !defined(__FUNCTION__)
#if defined(__func__)
#define __FUNCTION__ __func__
#else
#define NO__FUNCTION__
#endif
#endif

#ifndef ERROR_LINE_INFO_STRING
#ifdef NO__FUNCTION__
#define ERROR_LINE_INFO_STRING REString::createWithFormat("File: %s, line: %i", __FILE__, (int)__LINE__)
#else
#define ERROR_LINE_INFO_STRING REString::createWithFormat("File: %s, function: %s, line: %i", __FILE__, __FUNCTION__, (int)__LINE__)
#endif
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
	static const char * const _bayeuxExtKey = "ext";
	static const char * const _bayeuxErrorKey = "error";
	static const char * const _bayeuxSuccessfulKey = "successful";

#if defined(__APPLE__)
	const char * const kFayeCppBundleName = "FayeCpp";
	const char * const kFayeCppBundleLocalizationTableName = "FayeCppLocalizable";
#endif

	Error * Client::lastError() const
	{
		return _lastError;
	}

	const REVariant & Client::extValue() const
	{
		return _extValue;
	}

	REVariant & Client::extValue()
	{
		return _extValue;
	}

	void Client::setExtValue(const REVariant & value)
	{
		_extValue = value;
	}

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
	
    static unsigned int __client_messageId = 1;
	unsigned int Client::nextMessageId()
	{
		const unsigned int mID = __client_messageId++;
		if (__client_messageId >= 9999999) __client_messageId = 1;
		return mID;
	}
	
	void Client::processMessage(Responce * responce)
	{
		SAFE_DELETE(_lastError)
		switch (responce->type())
		{
			case Responce::ResponceTransportConnected: this->onTransportConnected(); break;
			case Responce::ResponceTransportDisconnected: this->onTransportDisconnected();  break;
			case Responce::ResponceTransportError: this->onClientError(responce); break;
			case Responce::ResponceMessage: this->onClientResponceReceived(responce); break;
			case Responce::ResponceTransportWillSelfDestruct: this->onTransportWillSelfDestruct(); break;
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

	void Client::onTransportWillSelfDestruct()
	{
		if (_transport)
		{
			if (_transport->isSelfDestructing()) _transport = NULL;
		}

		this->onTransportDisconnected();
	}
	
	void Client::onClientError(Responce * message)
	{
		SAFE_DELETE(_lastError)

		Error * error = message->error();
		if (error)
		{
			_lastError = new Error(*error);
		}
		else
		{
			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = Error::localizedStringForErrorCode(Error::InternalApplicationError);
			error = new Error(kErrorDomainClient, Error::InternalApplicationError, info);
			_lastError = error;
		}

		if (_delegate) _delegate->onFayeErrorString(this, error->localizedDescription());
	}
	
	void Client::onReceivedMessageOnChannel(const REVariantMap & message, const REString & channel)
	{
		if (_delegate) 
		{
			REVariant * data = message.findTypedValue(_bayeuxDataKey, REVariant::TypeMap);
			if (data) 
			{
				_delegate->onFayeClientReceivedMessageFromChannel(this, data->toMap(), channel);
			}
		}
	}
	
	void Client::onClientResponceMessageReceived(const REVariantMap & message)
	{
		SAFE_DELETE(_lastError)

		REVariantMap::Iterator i = message.iterator();
		while (i.next()) 
		{
			if (i.key().isEqual(_bayeuxChannelKey))
			{
				REString channel = i.value().toString();

				if (channel.isEqual(_bayeuxHandshakeChannel))
				{
					this->onHandshakeDone(message); return;
				}
				else if (channel.isEqual(_bayeuxConnectChannel))
				{
					this->onConnectFayeDone(message); return;
				}
				else if (channel.isEqual(_bayeuxDisconnectChannel))
				{
					this->onDisconnectFayeDone(message); return;
				}
				else if (channel.isEqual(_bayeuxSubscribeChannel))
				{
					this->onSubscriptionDone(message); return;
				}
				else if (channel.isEqual(_bayeuxUnsubscribeChannel))
				{
					this->onUnsubscribingDone(message); return;
				}
				else if (_subscribedChannels.isContaines(channel))
				{
					this->onReceivedMessageOnChannel(message, channel); return;
				}
			}
		}
	}

	void Client::onClientResponceMessagesListReceived(const REVariantList & messagesList)
	{
		REVariantList::Iterator i = messagesList.iterator();
		while (i.next()) 
		{
			switch (i.value().type()) 
			{
				case REVariant::TypeMap:
					this->onClientResponceMessageReceived(i.value().toMap());
					break;
				case REVariant::TypeList:
					this->onClientResponceMessagesListReceived(i.value().toList());
					break;
				default:
					break;
			}
		}
	}
	
	void Client::onClientResponceReceived(Responce * responce)
	{
		SAFE_DELETE(_lastError)

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
			FAYECPP_DEBUG_LOG("Client responce error: unknown responce data.")

			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = Error::localizedStringForErrorCode(Error::InternalApplicationError);
			Error * error = new Error(kErrorDomainClient, Error::InternalApplicationError, info);
			_lastError = error;

			if (_delegate && error) _delegate->onFayeErrorString(this, error->localizedDescription());
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
			REMutableString u(client->_url.UTF8String());
			u.replace("http://", "ws://");
			client->_url = u.UTF8String();
#endif
			client->_isUseSSL = false;
		}
		
		if (urlString.isContaines("https://"))
		{
			urlString.replace("https://");
#if defined(HAVE_SUITABLE_QT_VERSION)
			REMutableString u(client->_url.UTF8String());
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
	
	void Client::onHandshakeDone(const REVariantMap & message)
	{
		FAYECPP_DEBUG_LOG("Client: onHandshakeDone")
		SAFE_DELETE(_lastError)
		REString errorString;
		REVariantMap::Iterator i = message.iterator();
		while (i.next()) 
		{
			if (i.key().isEqual(_bayeuxClientIdKey) && i.value().isString())
			{
				_clientId = i.value().toString();
			}
			else if (i.key().isEqual(_bayeuxSupportedConnectionTypesKey) && i.value().isList())
			{
				REVariantList::Iterator j = i.value().toList().iterator();
				while (j.next()) _supportedConnectionTypes.add(j.value().toString());
			}
			else if (i.key().isEqual(_bayeuxErrorKey) && i.value().isString())
			{
				errorString = i.value().toString();
			}
		}

		if (errorString.isNotEmpty())
		{
			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = errorString;
			Error * error = new Error(kErrorDomainClient, Error::HandshakeBayeuxError, info);
			_lastError = error;
			if (_delegate && error) _delegate->onFayeErrorString(this, error->localizedDescription());
			return;
		}

		if (_clientId.isEmpty()) 
		{
			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = Error::localizedStringForErrorCode(Error::HandshakeClientIdIsEmpty);
			Error * error = new Error(kErrorDomainClient, Error::HandshakeClientIdIsEmpty, info);
			_lastError = error;
			if (_delegate && error) _delegate->onFayeErrorString(this, error->localizedDescription());
			return;
		}

		FAYECPP_DEBUG_LOGA("Client: clientId=%s", _clientId.UTF8String())
		
		if (_supportedConnectionTypes.isEmpty()) 
		{
			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = Error::localizedStringForErrorCode(Error::HandshakeSupportedConnectionTypesIsEmpty);
			Error * error = new Error(kErrorDomainClient, Error::HandshakeSupportedConnectionTypesIsEmpty, info);
			_lastError = error;
			if (_delegate && error) _delegate->onFayeErrorString(this, error->localizedDescription());
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
			REMutableString errorString = Error::localizedStringForErrorCode(Error::HandshakeImplementedTransportNotFound);
			unsigned int index = 0;
			j = _supportedConnectionTypes.iterator();
			while (j.next())
			{
				if (index) errorString.append(", ");
				else errorString.append(" [");
				errorString.append(j.value());
				index++;
			}
			errorString.append("]");

			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = errorString;
			Error * error = new Error(kErrorDomainClient, Error::HandshakeImplementedTransportNotFound, info);
			_lastError = error;
			if (_delegate && error) _delegate->onFayeErrorString(this, error->localizedDescription());
		}
	}
	
	void Client::handshake()
	{		
		if (!_transport) return;

		FAYECPP_DEBUG_LOG("Client: handshake start...")

		REVariantMap message;
		REVariantList connectionTypes;
		connectionTypes.add(_transport->name());
		message[_bayeuxSupportedConnectionTypesKey] = connectionTypes;
		message[_bayeuxMinimumVersionKey] = "1.0beta";
		message[_bayeuxChannelKey] = _bayeuxHandshakeChannel;
		message[_bayeuxVersionKey] = "1.0";
		if (_extValue.type() != REVariant::TypeNone) message[_bayeuxExtKey] = _extValue;
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		if (!_transport) return;
		
		this->sendText(JsonGenerator(message).string());
	}
	
	void Client::onConnectFayeDone(const REVariantMap & message)
	{
		if (!_isFayeConnected)
		{
			_isFayeConnected = true;
			_isDisconnecting = false;

			if (_delegate) _delegate->onFayeClientConnected(this);
			if (!_transport) return;
			this->subscribePendingSubscriptions();
		}
		
		REVariant * advice = message.findTypedValue(_bayeuxAdviceKey, REVariant::TypeMap);
		if (advice && _transport) _transport->receivedAdvice(advice->toMap());
	}
	
	void Client::connectFaye()
	{
		if (!_transport) return;

		FAYECPP_DEBUG_LOG("Client: connect faye start ...")

		REVariantMap message;
		message[_bayeuxChannelKey] = _bayeuxConnectChannel;
		message[_bayeuxClientIdKey] = _clientId;
		message[_bayeuxConnectionTypeKey] = _transport->name();
		if (_extValue.type() != REVariant::TypeNone) message[_bayeuxExtKey] = _extValue;
		if (_delegate) _delegate->onFayeClientWillSendMessage(this, message);
		if (!_transport) return;
		
		this->sendText(JsonGenerator(message).string());
	}
	
	void Client::disconnect()
	{
		FAYECPP_DEBUG_LOG("Client: disconnect faye start ...")

		_isDisconnecting = true;

		REVariantMap message;
		message[_bayeuxChannelKey] = _bayeuxDisconnectChannel;
		message[_bayeuxClientIdKey] = _clientId;
		if (_extValue.type() != REVariant::TypeNone) message[_bayeuxExtKey] = _extValue;
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
	
	void Client::onSubscriptionDone(const REVariantMap & message)
	{
		SAFE_DELETE(_lastError)
		REVariant * channel = message.findTypedValue(_bayeuxSubscriptionKey, REVariant::TypeString);
		if (!channel) 
		{
			FAYECPP_DEBUG_LOG("Client subscription error: can't find subscription key.")

			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = Error::localizedStringForErrorCode(Error::SubscriptionChannelNotFound);
			Error * error = new Error(kErrorDomainClient, Error::SubscriptionChannelNotFound, info);
			_lastError = error;
			if (_delegate && error) _delegate->onFayeErrorString(this, error->localizedDescription());
			return;
		}

		REStringList::Node * pendingChannelNode = _pendingSubscriptions.findNode(channel->toString());
		if (pendingChannelNode)
		{
			REVariant * advice = message.findTypedValue(_bayeuxAdviceKey, REVariant::TypeMap);
			if (advice && _transport) _transport->receivedAdvice(advice->toMap());
			
			_pendingSubscriptions.removeNode(pendingChannelNode);

			REVariantMap::Node * successfulNode = message.findNode(REStaticString(_bayeuxSuccessfulKey));
			const bool isSuccessful = successfulNode ? successfulNode->value.toBool() : true;

			if (!_isFayeConnected)
			{
				_isFayeConnected = true;
				if (_delegate) _delegate->onFayeClientConnected(this);
				if (!_transport) return;
			}

			if (isSuccessful)
			{
				FAYECPP_DEBUG_LOGA("Client: Subscribed to channel: %s", channel->toString().UTF8String())
				_subscribedChannels.add(channel->toString());
				if (_delegate) _delegate->onFayeClientSubscribedToChannel(this, channel->toString());
			}
			else
			{
				FAYECPP_DEBUG_LOGA("Client: Unsuccessful subscribing to channel: %s", channel->toString().UTF8String())

				REString errorString;
				REVariant * errorVariant = message.findTypedValue(_bayeuxErrorKey, REVariant::TypeString);
				if (errorVariant)
				{
					errorString = errorVariant->toString();
				}
				else
				{
					REString format = Error::localizedStringForErrorCode(Error::SubscriptionError);
					errorString = REString::createWithFormat(format.UTF8String(), channel->toString().UTF8String());
				}

				REVariantMap info;
				info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
				info[kErrorLocalizedDescriptionKey] = errorString;
				info[kErrorChannelKey] = channel->toString();
				Error * error = new Error(kErrorDomainClient, Error::SubscriptionError, info);
				_lastError = error;
				if (_delegate && error) _delegate->onFayeErrorString(this, errorString);
			}
		}
	}
	
	void Client::onUnsubscribingDone(const REVariantMap & message)
	{
		SAFE_DELETE(_lastError)
		REVariant * channel = message.findTypedValue(_bayeuxSubscriptionKey, REVariant::TypeString);
		if (!channel) 
		{
			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = Error::localizedStringForErrorCode(Error::UnsubscriptionChannelNotFound);
			Error * error = new Error(kErrorDomainClient, Error::UnsubscriptionChannelNotFound, info);
			_lastError = error;
			if (_delegate && error) _delegate->onFayeErrorString(this, error->localizedDescription());
			return;
		}
		
		REStringList::Node * node = _pendingSubscriptions.findNode(channel->toString());
		if (node) _pendingSubscriptions.removeNode(node);

		REVariantMap::Node * successfulNode = message.findNode(REStaticString(_bayeuxSuccessfulKey));
		const bool isSuccessful = successfulNode ? successfulNode->value.toBool() : true;

		if (isSuccessful)
		{
			node = _subscribedChannels.findNode(channel->toString());
			if (node) _subscribedChannels.removeNode(node);

			if (_delegate) _delegate->onFayeClientUnsubscribedFromChannel(this, channel->toString());
		}
		else
		{
			FAYECPP_DEBUG_LOGA("Client: Unsuccessful unsubscribing from channel: %s", channel->toString().UTF8String())

			REString errorString;
			REVariant * errorVariant = message.findTypedValue(_bayeuxErrorKey, REVariant::TypeString);
			if (errorVariant)
			{
				errorString = errorVariant->toString();
			}
			else
			{
				REString format = Error::localizedStringForErrorCode(Error::UnsubscriptionError);
				errorString = REString::createWithFormat(format.UTF8String(), channel->toString().UTF8String());
			}

			REVariantMap info;
			info[kErrorPlaceInTheCodeKey] = ERROR_LINE_INFO_STRING;
			info[kErrorLocalizedDescriptionKey] = errorString;
			info[kErrorChannelKey] = channel->toString();
			Error * error = new Error(kErrorDomainClient, Error::UnsubscriptionError, info);
			_lastError = error;
			if (_delegate && error) _delegate->onFayeErrorString(this, errorString);
		}
	}
	
	void Client::onDisconnectFayeDone(const REVariantMap & message)
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
					REVariantMap message;
					message[_bayeuxChannelKey] = _bayeuxSubscribeChannel;
					message[_bayeuxClientIdKey] = _clientId;
					message[_bayeuxSubscriptionKey] = i.value();
					if (_extValue.type() != REVariant::TypeNone) message[_bayeuxExtKey] = _extValue;
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
		
		REVariantMap message;
		message[_bayeuxChannelKey] = _bayeuxUnsubscribeChannel;
		message[_bayeuxClientIdKey] = _clientId;
		message[_bayeuxSubscriptionKey] = channel;
		if (_extValue.type() != REVariant::TypeNone) message[_bayeuxExtKey] = _extValue;
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
	
	bool Client::sendMessageToChannel(const REVariantMap & message, const char * channel)
	{
		if (_isFayeConnected && !message.isEmpty() && this->isSubscribedToChannel(channel))
		{
			FAYECPP_DEBUG_LOGA("Client: Send message to channel: %s", channel)

			REVariantMap mes;
			mes[_bayeuxChannelKey] = channel;
			mes[_bayeuxClientIdKey] = _clientId;
			mes[_bayeuxDataKey] = message;
			mes[_bayeuxIdKey] = (unsigned long long)Client::nextMessageId();
			if (_extValue.type() != REVariant::TypeNone) mes[_bayeuxExtKey] = _extValue;

			if (_delegate) _delegate->onFayeClientWillSendMessage(this, mes);

			return this->sendText(JsonGenerator(mes).string());
		}
		return false;
	}
	
	Client::Client() :
		_transport(NULL),
		_delegate(NULL),
		_sslDataSource(NULL),
		_lastError(NULL),
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

		SAFE_DELETE(_lastError)

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

/*   Detect building platform 32bit/64bit  */
#if ( (!defined(__RE_32BIT_PLATFORM__)) && (!defined(__RE_64BIT_PLATFORM__)) )

#if ( defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_AMD64_) || defined(_M_AMD64) )
#define __RE_64BIT_PLATFORM__
#endif /* Detect 64bit AMD64 */

#ifndef __RE_64BIT_PLATFORM__
#if ( defined(__LP64__) || defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64) )
#define __RE_64BIT_PLATFORM__
#endif /* Detect 64bit Intel Architecture-64 */
#endif

#ifndef __RE_64BIT_PLATFORM__
#if ( defined(_WIN64) || defined(__X86_64__) || defined(WIN64) || defined(_LP64) || defined(ppc64) || defined(x86_64) )
#define __RE_64BIT_PLATFORM__
#endif /* Detect 64bit common defines */
#endif

#ifndef __RE_64BIT_PLATFORM__
#if ( defined(__x86_64__) || defined(__ppc64__) )
#define __RE_64BIT_PLATFORM__
#endif /* Detect 64bit common defines */
#endif


#ifndef __RE_64BIT_PLATFORM__
#define __RE_32BIT_PLATFORM__
#endif /* if not 64bit than select 32bit */
	
#endif

#define STRING_I(s) #s
#define TO_STRING(s) STRING_I(s)
	const char * Client::info()
	{
		const char * info = "\nFayeCpp client library based on Bayeux protocol.\n"
		
		"Build info\n"
		" - Version: " TO_STRING(FAYECPP_VERSION_MAJOR) "." TO_STRING(FAYECPP_VERSION_MINOR) "." TO_STRING(FAYECPP_VERSION_PATCH) "\n"
#if defined(FAYECPP_BUILD_NUMBER)
		" - Build number: " TO_STRING(FAYECPP_BUILD_NUMBER) "\n"
#endif
#if defined(__unix__)
		" - Compiler targeting Unix systems\n"
#endif
#if defined(__STDC__)
		" - Compiler conforms to ISO Standard C\n"
#endif
#if defined(__STDC_VERSION__)
		" - C Standard's version number: " TO_STRING(__STDC_VERSION__) "\n"
#endif
#if defined(__OBJC__)
		" - Objective-C compiler is in use\n"
#endif
#if defined(_CPPRTTI)
		" - Enable Run-Time Type Information\n"
#endif
#if defined(__RE_32BIT_PLATFORM__)
		" - Compiled for 32 bit platform\n"
#endif
#if defined(__RE_64BIT_PLATFORM__)
		" - Compiled for 64 bit platform\n"
#endif
#if defined(__arm__) || defined(__thumb__) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__arm)
		" - ARM architecture\n"
#if defined(__ARM_ARCH_7__)
		" - ARMv7\n"
#endif
#if defined(__ARM_ARCH_7A__)
		" - ARMv7a\n"
#endif
#if defined(__ARM_ARCH_7R__)
		" - ARMv7r\n"
#endif
#if defined(__ARM_ARCH_7M__)
		" - ARMv7m\n"
#endif
#if defined(__ARM_ARCH_7S__)
		" - ARMv7s\n"
#endif
#endif
#if defined(__aarch64__)
		" - ARM64 architecture\n"
#endif
#if defined(__ARM_ARCH_8A__)
		" - ARMv8a\n"
#endif
#if defined(__llvm__)
		" - Compiled with LLVM\n"
#endif
#if defined(__clang__)
		" - Compiled with clang"
#if defined(__clang_version__)
		", version: " TO_STRING(__clang_version__) "\n"
#else
		"\n"
#endif	
#endif
#if defined(_M_ARM)
		" - Compiled for ARM processors\n"
#endif
#if defined(_MFC_VER)
		" - MFC version: " TO_STRING(_MFC_VER) "\n"
#endif
#if defined(_MSC_BUILD)
		" - Visual C++ compiler is in use\n"
#endif
#if defined(_MSC_FULL_VER)
		" - Version number of the Visual C++ compiler is: " TO_STRING(_MSC_FULL_VER) "\n"
#endif
#if defined(_ATL_VER)
		" - ATL version:" TO_STRING(_ATL_VER) "\n"
#endif

#if defined(HAVE_LIBWEBSOCKETS_H)
		" - Using Libwebsocket library"
#if defined(LWS_LIBRARY_VERSION)
		", version: " LWS_LIBRARY_VERSION
#endif
#if defined(LWS_BUILD_HASH)
		", build hash: " LWS_BUILD_HASH
#endif
		"\n"
		
#if defined(LWS_USE_IPV6)	
		"     with IPv6 support\n"
#endif
#if defined(LWS_OPENSSL_SUPPORT)
		"     with OpenSSL support\n"
#endif
		
#endif
		
#if defined(HAVE_SUITABLE_QT_VERSION)
		" - Using Qt"
#if defined(QT_VERSION_STR)
		", version: " QT_VERSION_STR "\n"
#else
		"\n"
#endif
#endif

		" - Using jansson library\n"
			
		/* Compiller build date, time and/or timestamp */
#if defined(__DATE__) && defined(__TIME__)
		" - Timestamp: " __DATE__ " " __TIME__ "\n"
#elif defined(__TIMESTAMP__)
		" - Timestamp: " __TIMESTAMP__ "\n"
#endif

		;
		return info;
	}
}

