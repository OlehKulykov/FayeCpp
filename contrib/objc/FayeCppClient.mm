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


#ifndef __has_feature
#define __has_feature(x) 0
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
	
	FayeCppClientSSLDataSourceWrapper() : objcClient(NULL)
	{
		
	}
	
	virtual ~FayeCppClientSSLDataSourceWrapper()
	{
		
	}
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
			NSString * ch = FayeCppDelegateWrapper::objcString(channel);
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeClient:objcClient subscribedToChannel:ch];
			});
		}
	}
	
	virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client,
													 const FayeCpp::REString & channel)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeClient:unsubscribedFromChannel:)])
		{
			NSString * ch = FayeCppDelegateWrapper::objcString(channel);
			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeClient:objcClient unsubscribedFromChannel:ch];
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
			NSString * ch = FayeCppDelegateWrapper::objcString(channel);
			id obj = FayeCppDelegateWrapper::objcObject(message);
			if (obj && [obj isKindOfClass:[NSDictionary class]])
			{
				dispatch_async(dispatch_get_main_queue(), ^{
					[d onFayeClient:objcClient receivedMessage:(NSDictionary*)obj fromChannel:ch];
				});
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
			NSError * error = FayeCppDelegateWrapper::objcError(client->lastError());

			dispatch_async(dispatch_get_main_queue(), ^{
				[d onFayeClient:objcClient error:error];
			});
		}
	}
	
	FayeCppDelegateWrapper() : objcClient(NULL)
	{
		
	}
	
	virtual ~FayeCppDelegateWrapper()
	{
		
	}
	
	static NSString * objcString(const REString & string);
	static id objcObject(const REVariant & variant);
	static NSMutableArray * objcArray(const REVariantList & list);
	static NSMutableDictionary * objcDict(const REVariantMap & map);
	static NSError * objcError(Error * error);
	static void objcDictToMap(NSDictionary * dict, REVariantMap & map);
	static REVariant objcObjectToVariant(id object);
};

NSError * FayeCppDelegateWrapper::objcError(Error * error)
{
	NSMutableDictionary * userInfo = nil;
	NSInteger code = (NSInteger)kCFHostErrorUnknown;
	NSString * domain = nil;
	if (error)
	{
		userInfo = [NSMutableDictionary dictionary];
		REVariantMap::Iterator iterator = error->userInfo().iterator();
		while (iterator.next())
		{
			if (iterator.key().isEqual(kErrorLocalizedDescriptionKey))
			{
				[userInfo setObject:FayeCppDelegateWrapper::objcString(iterator.value().toString())
							 forKey:NSLocalizedDescriptionKey];
			}
			else if (iterator.key().isEqual(kErrorURLKey))
			{
				NSURL * url = [NSURL URLWithString:FayeCppDelegateWrapper::objcString(iterator.value().toString())];
				if (url) [userInfo setObject:url forKey:NSURLErrorKey];
			}
			else if (iterator.key().isEqual(kErrorPlaceInTheCodeKey))
			{
				[userInfo setObject:FayeCppDelegateWrapper::objcString(iterator.value().toString())
							 forKey:kFayeCppErrorPlaceInTheCodeKey];
			}
			else if (iterator.key().isEqual(kErrorChannelKey))
			{
				[userInfo setObject:FayeCppDelegateWrapper::objcString(iterator.value().toString())
							 forKey:kFayeCppErrorChannelKey];
			}
		}
		code = (NSInteger)error->code();
		domain = FayeCppDelegateWrapper::objcString(error->domain());
		if ([userInfo count] == 0) userInfo = nil;
	}
	else
	{
		domain = [NSString stringWithUTF8String:kErrorDomainClient];
	}

	return [NSError errorWithDomain:domain code:code userInfo:userInfo];
}

id FayeCppDelegateWrapper::objcObject(const REVariant & variant)
{
	switch (variant.type())
	{
		case REVariant::TypeInteger:
			return [NSNumber numberWithLongLong:variant.toInt64()];
			break;
		case REVariant::TypeUnsignedInteger:
			return [NSNumber numberWithUnsignedLongLong:variant.toUInt64()];
			break;
		case REVariant::TypeReal:
			return [NSNumber numberWithDouble:variant.toDouble()];
			break;
		case REVariant::TypeBool:
			return [NSNumber numberWithBool:(BOOL)variant.toBool()];
			break;
		case REVariant::TypeString:
			return FayeCppDelegateWrapper::objcString(variant.toString());
			break;
		case REVariant::TypeMap:
			return FayeCppDelegateWrapper::objcDict(variant.toMap());
			break;
		case REVariant::TypeList:
			return FayeCppDelegateWrapper::objcArray(variant.toList());
			break;
		default:
			break;
	}
	return [NSNull null];
}

NSMutableArray * FayeCppDelegateWrapper::objcArray(const REVariantList & list)
{
	NSMutableArray * arr = [NSMutableArray array];
	REVariantList::Iterator i = list.iterator();
	while (i.next())
	{
		[arr addObject:FayeCppDelegateWrapper::objcObject(i.value())];
	}
	return arr;
}

NSMutableDictionary * FayeCppDelegateWrapper::objcDict(const REVariantMap & map)
{
	NSMutableDictionary * dict = [NSMutableDictionary dictionary];
	REVariantMap::Iterator i = map.iterator();
	while (i.next())
	{
		[dict setObject:FayeCppDelegateWrapper::objcObject(i.value())
				 forKey:FayeCppDelegateWrapper::objcString(i.key())];
	}
	return dict;
}

NSString * FayeCppDelegateWrapper::objcString(const REString & string)
{
	const char * s = string.UTF8String();
	return s ? [NSString stringWithUTF8String:s] : @"";
}

REVariant FayeCppDelegateWrapper::objcObjectToVariant(id object)
{
	if ([object isKindOfClass:[NSDictionary class]])
	{
		REVariantMap m;
		FayeCppDelegateWrapper::objcDictToMap((NSDictionary *)object, m);
		return REVariant(m);
	}
	else if ([object isKindOfClass:[NSArray class]])
	{
		REVariantList l;
		for (id obj in (NSArray*)object)
		{
			l.add(FayeCppDelegateWrapper::objcObjectToVariant(obj));
		}
		return REVariant(l);
	}
	else if ([object isKindOfClass:[NSString class]])
	{
		return REVariant([(NSString*)object UTF8String]);
	}
	else if ([object isKindOfClass:[NSNumber class]])
	{
		NSNumber * num = (NSNumber *)object;
		if (strcmp([num objCType], @encode(BOOL)) == 0) return REVariant((bool)[num boolValue]);
		else
		{
			switch (CFNumberGetType((CFNumberRef)num)) {
				case kCFNumberFloatType:
				case kCFNumberDoubleType:
				case kCFNumberCGFloatType:
				case kCFNumberFloat32Type:
				case kCFNumberFloat64Type:
					return REVariant((double)[num doubleValue]);
					break;
					
				default:
					return REVariant((long long)[num longLongValue]);
					break;
			}
		}
	}
	return REVariant();
}

void FayeCppDelegateWrapper::objcDictToMap(NSDictionary * dict, REVariantMap & map)
{
	[dict enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL * stop){
		if ([key isKindOfClass:[NSString class]])
		{
			map[[(NSString *)key UTF8String]] = FayeCppDelegateWrapper::objcObjectToVariant(obj);
		}
	}];
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
		id ext = FayeCppDelegateWrapper::objcObject(e);
		return ext;
	}

	return nil;
}

- (void) setExtValue:(id) value
{
	if (!_cppClient) return;

	if (value)
	{
		if ([value isKindOfClass:[NSNull class]]) _cppClient->setExtValue(REVariant());
		else
		{
			REVariant e = FayeCppDelegateWrapper::objcObjectToVariant(value);
			_cppClient->setExtValue(e);
		}
	}
	else
	{
		_cppClient->setExtValue(REVariant());
	}
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
		FayeCppDelegateWrapper::objcDictToMap(message, map);
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

