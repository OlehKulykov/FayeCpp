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


#import "FayeCppClient.h"
#import <CoreGraphics/CoreGraphics.h>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if __has_feature(objc_arc)
#error "ARC enabled. Need disable for this file."
#endif

#include <fayecpp.h>

NSInteger kFayeCppErrorCodeNone = FayeCpp::Error::None;
NSInteger kFayeCppErrorCodeInternalApplicationError = FayeCpp::Error::InternalApplicationError;
NSInteger kFayeCppErrorCodeSendingBufferTooLarge = FayeCpp::Error::SendingBufferTooLarge;
NSInteger kFayeCppErrorCodeFailedConnectToHost = FayeCpp::Error::FailedConnectToHost;
NSInteger kFayeCppErrorCodeHandshakeBayeuxError = FayeCpp::Error::HandshakeBayeuxError;
NSInteger kFayeCppErrorCodeHandshakeClientIdIsEmpty = FayeCpp::Error::HandshakeClientIdIsEmpty;
NSInteger kFayeCppErrorCodeHandshakeSupportedConnectionTypesIsEmpty = FayeCpp::Error::HandshakeSupportedConnectionTypesIsEmpty;
NSInteger kFayeCppErrorCodeHandshakeImplementedTransportNotFound  = FayeCpp::Error::HandshakeImplementedTransportNotFound;
NSInteger kFayeCppErrorCodeSubscriptionChannelNotFound = FayeCpp::Error::SubscriptionChannelNotFound;
NSInteger kFayeCppErrorCodeSubscriptionError = FayeCpp::Error::SubscriptionError;
NSInteger kFayeCppErrorCodeUnsubscriptionChannelNotFound = FayeCpp::Error::UnsubscriptionChannelNotFound;
NSInteger kFayeCppErrorCodeUnsubscriptionError = FayeCpp::Error::UnsubscriptionError;
NSString * kFayeCppErrorPlaceInTheCodeKey = @"kFayeCppErrorPlaceInTheCodeKey";
NSString * kFayeCppErrorChannelKey = @"kFayeCppErrorChannelKey";

using namespace FayeCpp;

class CFHelper
{
public:
	static CFStringRef string(const REString & string);
	static CFMutableArrayRef array(const REVariantList & list);
	static CFTypeRef object(const REVariant & variant);
	static CFMutableDictionaryRef dict(const REVariantMap & map);
	static CFErrorRef error(Error * error);
	static CFURLRef url(const REString & string);
	static REVariant variant(CFTypeRef object);
	static void dictToMap(CFDictionaryRef dict, REVariantMap * map);
};

class FayeCppClientSSLDataSourceWrapper : public SSLDataSource
{
public:
	FayeCppClient * objcClient;
	
	virtual FayeCpp::REString clientLocalCertificateFilePath() const
	{
		id<FayeCppClientSSLDataSource> d = objcClient ? [objcClient sslDataSource] : nil;
		if (d && [d respondsToSelector:@selector(clientLocalCertificateFilePath)])
		{
			NSString * path = [d clientLocalCertificateFilePath];
			if (path && [path length]) 
			{
				return FayeCpp::REString([path UTF8String]);
			}
		}
		return FayeCpp::REString();
	}
	
	virtual FayeCpp::REString clientPrivateKeyFilePath() const
	{
		id<FayeCppClientSSLDataSource> d = objcClient ? [objcClient sslDataSource] : nil;
		if (d && [d respondsToSelector:@selector(clientPrivateKeyFilePath)])
		{
			NSString * path = [d clientPrivateKeyFilePath];
			if (path && [path length]) 
			{
				return FayeCpp::REString([path UTF8String]);
			}
		}
		return FayeCpp::REString();
	}
	
	virtual FayeCpp::REString clientPrivateKeyPassPhrase() const
	{
		id<FayeCppClientSSLDataSource> d = objcClient ? [objcClient sslDataSource] : nil;
		if (d && [d respondsToSelector:@selector(clientPrivateKeyPassPhrase)])
		{
			NSString * pp = [d clientPrivateKeyPassPhrase];
			if (pp && [pp length]) 
			{
				return FayeCpp::REString([pp UTF8String]);
			}
		}
		return FayeCpp::REString();
	}
	
	virtual FayeCpp::REString clientCACertificateFilePath() const
	{
		id<FayeCppClientSSLDataSource> d = objcClient ? [objcClient sslDataSource] : nil;
		if (d && [d respondsToSelector:@selector(clientCACertificateFilePath)])
		{
			NSString * path = [d clientCACertificateFilePath];
			if (path && [path length]) 
			{
				return FayeCpp::REString([path UTF8String]);
			}
		}
		return FayeCpp::REString();
	}
	
	FayeCppClientSSLDataSourceWrapper() : objcClient(NULL) { }
	virtual ~FayeCppClientSSLDataSourceWrapper() { }
};

class FayeCppDelegateWrapper : public Delegate
{
public:
	FayeCppClient * objcClient;
	
	virtual void onFayeTransportConnected(FayeCpp::Client * client)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeTransportConnected:)])
		{
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeTransportConnected:objcClient];
			});
		}
	}
	
	virtual void onFayeTransportDisconnected(FayeCpp::Client * client)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeTransportDisconnected:)])
		{
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeTransportDisconnected:objcClient];
			});
		}
	}
	
	virtual void onFayeClientConnected(FayeCpp::Client * client)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeClientConnected:)])
		{
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeClientConnected:objcClient];
			});
		}
	}
	
	virtual void onFayeClientDisconnected(FayeCpp::Client * client)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeClientDisconnected:)])
		{
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeClientDisconnected:objcClient];
			});
		}
	}
	
	virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client,
												 const FayeCpp::REString & channel)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeClient:subscribedToChannel:)])
		{
			CFStringRef ch = CFHelper::string(channel);
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeClient:objcClient subscribedToChannel:(NSString *)ch];
				if (ch) CFRelease(ch);
			});
		}
	}
	
	virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client,
													 const FayeCpp::REString & channel)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeClient:unsubscribedFromChannel:)])
		{
			CFStringRef ch = CFHelper::string(channel);
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeClient:objcClient unsubscribedFromChannel:(NSString *)ch];
				if (ch) CFRelease(ch);
			});
		}
	}

	virtual void onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client,
														const FayeCpp::REVariantMap & message,
														const FayeCpp::REString & channel)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeClient:receivedMessage:fromChannel:)])
		{
			CFStringRef ch = CFHelper::string(channel);
			CFTypeRef obj = CFHelper::object(message);
			bool isPassed = false;
			if (obj)
			{
				if (CFGetTypeID(obj) == CFDictionaryGetTypeID())
				{
					isPassed = true;
					dispatch_async(dispatch_get_main_queue(), ^{
						[d onFayeClient:objcClient receivedMessage:(NSDictionary *)obj fromChannel:(NSString *)ch];
						if (obj) CFRelease(obj);
						if (ch) CFRelease(ch);
					});
				}
			}
			if (!isPassed)
			{
				if (obj) CFRelease(obj);
				if (ch) CFRelease(ch);
			}
		}
	}
	
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::REVariantMap & message)
	{
		
	}
	
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeClient:error:)])
		{
			CFErrorRef error = CFHelper::error(client->lastError());
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeClient:objcClient error:(NSError *)error];
				if (error) CFRelease(error);
			});
		}
	}
	
	FayeCppDelegateWrapper() : objcClient(NULL) { }
	virtual ~FayeCppDelegateWrapper() { }
};

void CFHelper::dictToMap(CFDictionaryRef dict, REVariantMap * map)
{
	if (dict && map)
	{
		const CFIndex count = CFDictionaryGetCount(dict);
		if (count > 0)
		{
			const void ** values = (const void **)malloc(sizeof(const void *) * count);
			const void ** keys = (const void **)malloc(sizeof(const void *) * count);
			if (values && keys)
			{
				CFDictionaryGetKeysAndValues(dict, keys, values);
				for (CFIndex i = 0; i < count; i++)
				{
					CFTypeRef key = (CFTypeRef)keys[i];
					if (CFGetTypeID(key) == CFStringGetTypeID())
					{
						REVariant kVariant = CFHelper::variant(key);
						if (kVariant.isString())
						{
							map->setKeyValue(kVariant.toString(), CFHelper::variant((CFTypeRef)values[i]));
						}
					}
				}
			}
			if (keys) free(keys);
			if (values) free(values);
		}
	}
}

REVariant CFHelper::variant(CFTypeRef object)
{
	if (object)
	{
		const CFTypeID type = CFGetTypeID(object);
		if (type == CFStringGetTypeID())
		{
			REVariant var;
			const CFIndex len = CFStringGetLength((CFStringRef)object);
			if (len > 0)
			{
				const size_t buffSize = (len + 1) * sizeof(wchar_t);
				char * buff = (char *)malloc(buffSize);
				if (buff)
				{
					if (CFStringGetCString((CFStringRef)object, buff, (CFIndex)buffSize, kCFStringEncodingUTF8)) var = buff;
					free(buff);
				}
			}
			if  (var.type() != REVariant::TypeString) var = "";
			return var;
		}
		else if (type == CFDictionaryGetTypeID())
		{
			REVariantMap m;
			CFHelper::dictToMap((CFDictionaryRef)object, &m);
			return REVariant(m);
		}
		else if (type == CFArrayGetTypeID())
		{
			REVariantList l;
			const CFIndex count = CFArrayGetCount((CFArrayRef)object);
			for (CFIndex i = 0; i < count; i++)
			{
				CFTypeRef element = CFArrayGetValueAtIndex((CFArrayRef)object, i);
				if (element) l.add(CFHelper::variant(element));
			}
			return REVariant(l);
		}
		else if (type == CFBooleanGetTypeID())
		{
			return CFEqual(object, kCFBooleanTrue) ? REVariant(true) : REVariant(false);
		}
		else if (type == CFNumberGetTypeID())
		{
			const CFNumberType numType = CFNumberGetType((CFNumberRef)object);
			switch (numType)
			{
				case kCFNumberSInt8Type: { SInt8 v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((int)v); } break;
				case kCFNumberSInt16Type: { SInt16 v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((int)v); } break;
				case kCFNumberSInt32Type: { SInt32 v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((int)v); } break;
				case kCFNumberSInt64Type: { SInt64 v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((long long)v); } break;
				case kCFNumberFloat32Type: { Float32 v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((double)v); } break;
				case kCFNumberFloat64Type: { Float64 v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((double)v); } break;
				case kCFNumberCharType: { char v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((int)v); } break;
				case kCFNumberShortType: { short v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((int)v); } break;
				case kCFNumberIntType: { int v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant(v); } break;
				case kCFNumberLongType: { long v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((long long)v); } break;
				case kCFNumberLongLongType: { long long v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((long long)v); } break;
				case kCFNumberFloatType: { float v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((double)v); } break;
				case kCFNumberDoubleType: { double v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((double)v); } break;
				case kCFNumberCFIndexType: { CFIndex v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((long long)v); } break;
				case kCFNumberNSIntegerType: { NSInteger v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((long long)v); } break;
				case kCFNumberCGFloatType: { CGFloat v = 0; if (CFNumberGetValue((CFNumberRef)object, numType, &v)) return REVariant((double)v); } break;
				default:
					break;
			}
		}
	}
	return REVariant();
}

CFURLRef CFHelper::url(const REString & string)
{
	const CFIndex len = string.length();
	return (len > 0) ? CFURLCreateWithBytes(NULL, (const UInt8 *)string.UTF8String(), len, kCFStringEncodingUTF8, NULL) : NULL;
}

CFErrorRef CFHelper::error(Error * error)
{
	CFMutableDictionaryRef userInfo = NULL;
	CFIndex code = (CFIndex)kCFNetServiceErrorUnknown;
	CFStringRef domain = NULL;
	if (error)
	{
		userInfo = CFDictionaryCreateMutable(NULL, 4, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		if (userInfo)
		{
			REVariantMap::Iterator iterator = error->userInfo().iterator();
			while (iterator.next())
			{
				if (iterator.key().isEqual(kErrorLocalizedDescriptionKey))
				{
					CFStringRef value = CFHelper::string(iterator.value().toString());
					if (value)
					{
						CFDictionarySetValue(userInfo, kCFErrorLocalizedDescriptionKey, value);
						CFRelease(value);
					}
				}
				else if (iterator.key().isEqual(kErrorURLKey))
				{
					CFURLRef url = CFHelper::url(iterator.value().toString());
					if (url)
					{
						CFDictionarySetValue(userInfo, kCFErrorURLKey, url);
						CFRelease(url);
					}
				}
				else if (iterator.key().isEqual(kErrorPlaceInTheCodeKey))
				{
					CFStringRef value = CFHelper::string(iterator.value().toString());
					if (value)
					{
						CFDictionarySetValue(userInfo, (CFStringRef)kFayeCppErrorPlaceInTheCodeKey, value);
						CFRelease(value);
					}
				}
				else if (iterator.key().isEqual(kErrorChannelKey))
				{
					CFStringRef value = CFHelper::string(iterator.value().toString());
					if (value)
					{
						CFDictionarySetValue(userInfo, (CFStringRef)kFayeCppErrorChannelKey, value);
						CFRelease(value);
					}
				}
			}
		}
		code = (CFIndex)error->code();
		domain = CFHelper::string(error->domain());
	}
	else
	{
		domain = CFStringCreateWithCString(NULL, kErrorDomainClient, kCFStringEncodingUTF8);
	}

	CFErrorRef err = NULL;
	if (domain)
	{
		err = CFErrorCreate(NULL, domain, code, userInfo);
		CFRelease(domain);
	}
	if (userInfo) CFRelease(userInfo);
	return err;
}

CFMutableDictionaryRef CFHelper::dict(const REVariantMap & map)
{
	CFMutableDictionaryRef d = CFDictionaryCreateMutable(NULL, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (d)
	{
		REVariantMap::Iterator i = map.iterator();
		while (i.next())
		{
			CFStringRef key = CFHelper::string(i.key());
			CFTypeRef value = CFHelper::object(i.value());
			if (key && value) CFDictionarySetValue(d, key, value);

			if (key) CFRelease(key);
			if (value) CFRelease(value);
		}
	}
	return d;
}

CFStringRef CFHelper::string(const REString & string)
{
	const char * s = string.UTF8String();
	return s ? CFStringCreateWithCString(NULL, s, kCFStringEncodingUTF8) : CFSTR("");
}

CFMutableArrayRef CFHelper::array(const REVariantList & list)
{
	CFMutableArrayRef arr = CFArrayCreateMutable(NULL, 2, &kCFTypeArrayCallBacks);
	if (arr)
	{
		REVariantList::Iterator i = list.iterator();
		while (i.next())
		{
			CFTypeRef obj = CFHelper::object(i.value());
			if (obj)
			{
				CFArrayAppendValue(arr, obj);
				CFRelease(obj);
			}
		}
	}
	return arr;
}

CFTypeRef CFHelper::object(const REVariant & variant)
{
	switch (variant.type())
	{
		case REVariant::TypeInteger:
		case REVariant::TypeUnsignedInteger:
		{
			const long long v = variant.toInt64();
			return CFNumberCreate(NULL, kCFNumberLongLongType, &v);
		}
			break;
		case REVariant::TypeReal:
		{
			const double v = variant.toDouble();
			return CFNumberCreate(NULL, kCFNumberDoubleType, &v);
		}
			break;
		case REVariant::TypeBool:
			return variant.toBool() ? kCFBooleanTrue : kCFBooleanFalse;
			break;
		case REVariant::TypeString:
			return CFHelper::string(variant.toString());
			break;
		case REVariant::TypeMap:
			return CFHelper::dict(variant.toMap());
			break;
		case REVariant::TypeList:
			return CFHelper::array(variant.toList());
			break;
		default:
			break;
	}
	return kCFNull;
}

#if !defined(SAFE_RETAIN) && !__has_feature(objc_arc)
#define SAFE_RETAIN(to,from) if(to){[to release];to=nil;}if(from)to=[from retain];
#endif

@interface FayeCppClient()
{
@private
	Client * _cppClient;
	FayeCppDelegateWrapper * _cppDelegate;
	FayeCppClientSSLDataSourceWrapper * _SSLDataSourceWrapper;
	
#if !__has_feature(objc_arc)
	id<FayeCppClientDelegate> _delegateObject;
	id<FayeCppClientSSLDataSource> _sslDataSourceObject;
	NSString * _urlStringObject;
#endif
}

#if __has_feature(objc_arc)
@property (nonatomic, weak) id<FayeCppClientDelegate> delegateObject;
@property (nonatomic, weak) id<FayeCppClientSSLDataSource> sslDataSourceObject;
@property (nonatomic, strong) NSString * urlStringObject;
#endif

@end


@implementation FayeCppClient

- (id) extValue
{
	REVariant e;
	if (_cppClient) e = _cppClient->extValue();

	if (e.type() != REVariant::TypeNone)
	{
		CFTypeRef ext = CFHelper::object(e);
		return (id)CFAutorelease(ext);
	}

	return nil;
}

- (void) setExtValue:(id) value
{
	if (_cppClient) _cppClient->setExtValue(CFHelper::variant((CFTypeRef)value));
}

- (BOOL) isUsingIPV6
{
	return _cppClient ? (BOOL)_cppClient->isUsingIPV6() : NO;
}

- (void) setUsingIPV6:(BOOL) isUse
{
	if (_cppClient) _cppClient->setUsingIPV6((bool)isUse);
}

- (BOOL) connect
{
	return _cppClient ? (BOOL)_cppClient->connect() : NO;
}

- (BOOL) isTransportConnected
{
	return _cppClient ? (BOOL)_cppClient->isTransportConnected() : NO;
}

- (BOOL) isFayeConnected
{
	return _cppClient ? (BOOL)_cppClient->isFayeConnected() : NO;
}

- (void) disconnect
{
	if (_cppClient) _cppClient->disconnect();
}

- (BOOL) sendMessage:(NSDictionary *) message
		   toChannel:(NSString *) channel
{
	if (_cppClient && message && channel)
	{
		REVariantMap map;
		CFHelper::dictToMap((CFDictionaryRef)message, &map);
		return _cppClient->sendMessageToChannel(map, [channel UTF8String]);
	}
	return NO;
}

- (BOOL) isSubscribedToChannel:(NSString *) channel
{
	if (_cppClient)
	{
		return (BOOL)_cppClient->isSubscribedToChannel([channel UTF8String]);
	}
	return NO;
}

- (BOOL) subscribeToChannel:(NSString *) channel
{
	if (_cppClient && channel)
	{
		return (BOOL)_cppClient->subscribeToChannel([channel UTF8String]);
	}
	return NO;
}

- (BOOL) unsubscribeFromChannel:(NSString *) channel
{
	if (_cppClient && channel)
	{
		return (BOOL)_cppClient->unsubscribeFromChannel([channel UTF8String]);
	}
	return NO;
}

- (void) setUrlString:(NSString *) url
{
#if __has_feature(objc_arc)
	self.urlStringObject = url;
#else
	SAFE_RETAIN(_urlStringObject, url)
#endif
	
	if (_cppClient) _cppClient->setUrl(_urlStringObject ? [_urlStringObject UTF8String] : NULL);
}

- (NSString *) urlString
{
	const char * processedUrl = _cppClient ? _cppClient->url().UTF8String() : NULL;
	return processedUrl ? [NSString stringWithUTF8String:processedUrl] : _urlStringObject;
}

- (void) setDelegate:(id<FayeCppClientDelegate>) fayeDelegate
{
#if __has_feature(objc_arc)
	self.delegateObject = fayeDelegate;
#else
	_delegateObject = fayeDelegate;
#endif
	
	_cppDelegate->objcClient = [self delegate] ? self : nil;
}

- (id<FayeCppClientDelegate>) delegate
{
#if __has_feature(objc_arc)
	return self.delegateObject;
#else
	return _delegateObject;
#endif
}

- (void) setSSLDataSource:(id<FayeCppClientSSLDataSource>) fayeSSLDataSource
{
#if __has_feature(objc_arc)
	self.sslDataSourceObject = fayeSSLDataSource;
#else
	_sslDataSourceObject = fayeSSLDataSource;
#endif
	
	_SSLDataSourceWrapper->objcClient = [self sslDataSource] ? self : nil;
}

- (id<FayeCppClientSSLDataSource>) sslDataSource
{
#if __has_feature(objc_arc)
	return self.sslDataSourceObject;
#else
	return _sslDataSourceObject;
#endif
}

- (id) init
{
	self = [super init];
	if (self)
	{
		_cppDelegate = new FayeCppDelegateWrapper();
		if (!_cppDelegate)
		{
#if !__has_feature(objc_arc)
			[self release];
#endif
			return nil;
		}
		
		_SSLDataSourceWrapper = new FayeCppClientSSLDataSourceWrapper();
		if (!_SSLDataSourceWrapper) 
		{
			delete _cppDelegate;
#if !__has_feature(objc_arc)
			[self release];
#endif
			return nil;			
		}
		
		_cppClient = new Client();
		if (!_cppClient)
		{
			delete _cppDelegate;
			delete _SSLDataSourceWrapper;
#if !__has_feature(objc_arc)
			[self release];
#endif
			return nil;
		}
		
		_cppClient->setDelegate(_cppDelegate);
		_cppClient->setSSLDataSource(_SSLDataSourceWrapper);
	}
	return self;
}

- (void) dealloc
{
	if (_cppDelegate) 
	{
		_cppDelegate->objcClient = nil;
		delete _cppDelegate;
	}
	
	if (_SSLDataSourceWrapper) 
	{
		_SSLDataSourceWrapper->objcClient = nil;
		delete _SSLDataSourceWrapper;
	}
	
	if (_cppClient)
	{
		_cppClient->setDelegate(NULL);
		_cppClient->setSSLDataSource(NULL);
		delete _cppClient;
	}

#if !__has_feature(objc_arc)
	if (_urlStringObject) [_urlStringObject release];
	[super dealloc];
#endif
}

+ (BOOL) isSupportsIPV6
{
	return (BOOL)Client::isSupportsIPV6();
}

+ (BOOL) isSupportsSSLConnection
{
	return (BOOL)Client::isSupportsSSLConnection();
}

@end

