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


namespace FayeCpp {

	__RE_EXTERN__ const char * const kErrorDomainClient = "Faye client";
	__RE_EXTERN__ const char * const kErrorDomainTransport = "Faye transport";
	__RE_EXTERN__ const char * const kErrorLocalizedDescriptionKey = "localizedDescription";
	__RE_EXTERN__ const char * const kErrorPlaceInTheCodeKey = "placeInTheCode";
	__RE_EXTERN__ const char * const kErrorURLKey = "url";
	__RE_EXTERN__ const char * const kErrorChannelKey = "channel";

	bool Error::isExists() const
	{
		return (_code != Error::None);
	}

	void Error::clear()
	{
		_userInfo.clear();
		_domain.clear();
		_code = Error::None;
	}

	const REVariantMap & Error::userInfo() const
	{
		return _userInfo;
	}

	REString Error::domain() const
	{
		return _domain;
	}

	REString Error::localizedDescription() const
	{
		REVariant * descr = _userInfo.findTypedValue(kErrorLocalizedDescriptionKey, REVariant::TypeString);
		return descr ? descr->toString() : REString();
	}

	int Error::code() const
	{
		return _code;
	}

	Error::Error(const REString & domain, int code, const REVariantMap & info) :
		_userInfo(info),
		_domain(domain),
		_code(code)
	{

	}

	Error & Error::operator=(const Error & anotherError)
	{
		_userInfo = anotherError._userInfo;
		_domain = anotherError._domain;
		_code = anotherError._code;
		return (*this);
	}

	Error::Error(const Error & anotherError) :
		_code(Error::None)
	{
		(*this) = anotherError;
	}

	Error::~Error()
	{

	}

	REString Error::localizedStringForErrorCode(const ErrorCode code)
	{
		switch (code)
		{
			case InternalApplicationError:
				return REString("Internal application error.");
				break;

			case SendingBufferTooLarge:
				return REString("Error sending %u bytes. Can't send more than %i bytes.");
				break;

			case FailedConnectToHost:
				return REString("Failed to connect to host %s:%i.");
				break;

			case HandshakeClientIdIsEmpty:
				return REString("Handshake clientId is empty.");
				break;

			case HandshakeSupportedConnectionTypesIsEmpty:
				return REString("Handshake supported connection types is empty.");
				break;

			case HandshakeImplementedTransportNotFound:
				return REString("Can't find implemented faye transport protocol type from supported by the server:");
				break;

			case SubscriptionChannelNotFound:
			case UnsubscriptionChannelNotFound:
				return REString("Can't find subscription key.");
				break;

			case SubscriptionError:
				return REString("Unsuccessful subscribing to channel: %s.");
				break;

			case UnsubscriptionError:
				return REString("Unsuccessful unsubscribing to channel: %s.");
				break;

			default:
				break;
		}
		return REString();
	}

}



