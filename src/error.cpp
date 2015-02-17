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

#if defined(__APPLE__) && defined(HAVE_COREFOUNDATION_FRAMEWORK)
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace FayeCpp {

#if defined(__APPLE__) && defined(HAVE_COREFOUNDATION_FRAMEWORK)
	class AppleErrorLocalizer
	{
	private:
		CFBundleRef _localizedBundle;
		CFStringRef _localizedTableName;

		void init()
		{
			CFBundleRef mainBundle = CFBundleGetMainBundle();
			if (!mainBundle) return;

			CFStringRef resourceName = CFStringCreateWithCString(NULL, kFayeCppBundleName ,kCFStringEncodingUTF8);
			if (resourceName)
			{
				CFURLRef url = CFBundleCopyResourceURL(mainBundle, resourceName, CFSTR("bundle"), NULL);
				CFRelease(resourceName);
				if (url)
				{
					_localizedBundle = CFBundleCreate(NULL, url);
					CFRelease(url);
				}
			}
			CFRelease(mainBundle);
			_localizedTableName = CFStringCreateWithCString(NULL, kFayeCppBundleLocalizationTableName ,kCFStringEncodingUTF8);
		}

	public:
		REString localized(const char * s)
		{
			REString resString;
			if (s && _localizedTableName && _localizedBundle)
			{
				CFStringRef key = CFStringCreateWithCString(NULL, s ,kCFStringEncodingUTF8);
				if (key)
				{
					CFStringRef loc = CFBundleCopyLocalizedString(_localizedBundle, key, key, _localizedTableName);
					if (loc)
					{
						const size_t buffSize = 512 * sizeof(wchar_t);
						char buff[buffSize] = { 0 };
						if (CFStringGetCString(loc, buff, buffSize, kCFStringEncodingUTF8))
						{
							resString = buff;
						}
						CFRelease(loc);
					}
					CFRelease(key);
				}
			}
			if (resString.isEmpty()) resString = s;
			return resString;
		}

		AppleErrorLocalizer() :
			_localizedBundle(NULL),
			_localizedTableName(NULL)
		{
			this->init();
		}

		~AppleErrorLocalizer()
		{
			if (_localizedBundle) CFRelease(_localizedBundle);
			if (_localizedTableName) CFRelease(_localizedTableName);
		}
	};
#endif


	const char * const kErrorDomainClient = "Faye client";
	const char * const kErrorDomainTransport = "Faye transport";
	const char * const kErrorLocalizedDescriptionKey = "localizedDescription";
	const char * const kErrorPlaceInTheCodeKey = "placeInTheCode";
	const char * const kErrorURLKey = "url";
	const char * const kErrorChannelKey = "channel";

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
		const char * s = NULL;

		switch (code)
		{
			case InternalApplicationError:
				s = "Internal application error.";
				break;

			case SendingBufferTooLarge:
				s = "Error sending %u bytes. Can't send more than %i bytes.";
				break;

			case FailedConnectToHost:
				s = "Failed to connect to host %s:%i.";
				break;

			case HandshakeClientIdIsEmpty:
				s = "Handshake clientId is empty.";
				break;

			case HandshakeSupportedConnectionTypesIsEmpty:
				s = "Handshake supported connection types is empty.";
				break;

			case HandshakeImplementedTransportNotFound:
				s = "Can't find implemented faye transport protocol type from supported by the server:";
				break;

			case SubscriptionChannelNotFound:
			case UnsubscriptionChannelNotFound:
				s = "Can't find subscription key.";
				break;

			case SubscriptionError:
				s = "Unsuccessful subscribing to channel: %s.";
				break;

			case UnsubscriptionError:
				s = "Unsuccessful unsubscribing from channel: %s.";
				break;

			default:
				break;
		}

#if defined(__APPLE__) && defined(HAVE_COREFOUNDATION_FRAMEWORK)
		AppleErrorLocalizer localizer;
		return localizer.localized(s);
#else
		return REString(s);
#endif

	}

}



