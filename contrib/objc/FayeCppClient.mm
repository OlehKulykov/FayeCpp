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


#import "FayeCppClient.h"


#ifndef __has_feature
#define __has_feature(x) 0
#endif


#include <fayecpp.h>

using namespace FayeCpp;

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
														const FayeCpp::VariantMap & message,
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
											 FayeCpp::VariantMap & message)
	{
		
	}
	
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{
		id<FayeCppClientDelegate> d = objcClient ? [objcClient delegate] : nil;
		if (d && [d respondsToSelector:@selector(onFayeClient:error:)])
		{
			NSString * errString = FayeCppDelegateWrapper::objcString(errorString);
			NSError * error = [NSError errorWithDomain:@"Faye client"
												  code:-1
											  userInfo:errString ? [NSDictionary dictionaryWithObject:errString forKey:NSLocalizedDescriptionKey] : nil];
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
	static id objcObject(const Variant & variant);
	static NSMutableArray * objcArray(const VariantList & list);
	static NSMutableDictionary * objcDict(const VariantMap & map);
	static void objcDictToMap(NSDictionary * dict, VariantMap & map);
	static Variant objcObjectToVariant(id object);
};

id FayeCppDelegateWrapper::objcObject(const Variant & variant)
{
	switch (variant.type())
	{
		case Variant::TypeInteger:
			return [NSNumber numberWithLongLong:variant.toInt64()];
			break;
		case Variant::TypeUnsignedInteger:
			return [NSNumber numberWithUnsignedLongLong:variant.toUInt64()];
			break;
		case Variant::TypeReal:
			return [NSNumber numberWithDouble:variant.toDouble()];
			break;
		case Variant::TypeBool:
			return [NSNumber numberWithBool:(BOOL)variant.toBool()];
			break;
		case Variant::TypeString:
			return FayeCppDelegateWrapper::objcString(variant.toString());
			break;
		case Variant::TypeMap:
			return FayeCppDelegateWrapper::objcDict(variant.toMap());
			break;
		case Variant::TypeList:
			return FayeCppDelegateWrapper::objcArray(variant.toList());
			break;
		default:
			break;
	}
	return [NSNull null];
}

NSMutableArray * FayeCppDelegateWrapper::objcArray(const VariantList & list)
{
	NSMutableArray * arr = [NSMutableArray array];
	VariantList::Iterator i = list.iterator();
	while (i.next())
	{
		[arr addObject:FayeCppDelegateWrapper::objcObject(i.value())];
	}
	return arr;
}

NSMutableDictionary * FayeCppDelegateWrapper::objcDict(const VariantMap & map)
{
	NSMutableDictionary * dict = [NSMutableDictionary dictionary];
	VariantMap::Iterator i = map.iterator();
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

Variant FayeCppDelegateWrapper::objcObjectToVariant(id object)
{
	if ([object isKindOfClass:[NSDictionary class]])
	{
		VariantMap m;
		FayeCppDelegateWrapper::objcDictToMap((NSDictionary *)object, m);
		return Variant(m);
	}
	else if ([object isKindOfClass:[NSArray class]])
	{
		VariantList l;
		for (id obj in (NSArray*)object)
		{
			l.add(FayeCppDelegateWrapper::objcObjectToVariant(obj));
		}
		return Variant(l);
	}
	else if ([object isKindOfClass:[NSString class]])
	{
		return Variant([(NSString*)object UTF8String]);
	}
	else if ([object isKindOfClass:[NSNumber class]])
	{
		NSNumber * num = (NSNumber *)object;
		if (strcmp([num objCType], @encode(BOOL)) == 0) return Variant((bool)[num boolValue]);
		else
		{
			switch (CFNumberGetType((CFNumberRef)num)) {
				case kCFNumberFloatType:
				case kCFNumberDoubleType:
				case kCFNumberCGFloatType:
				case kCFNumberFloat32Type:
				case kCFNumberFloat64Type:
					return Variant((double)[num doubleValue]);
					break;
					
				default:
					return Variant((long long)[num longLongValue]);
					break;
			}
		}
	}
	return Variant();
}

void FayeCppDelegateWrapper::objcDictToMap(NSDictionary * dict, VariantMap & map)
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
	
#if !__has_feature(objc_arc)
	id<FayeCppClientDelegate> _delegateObject;
	NSString * _urlStringObject;
#endif
}

#if __has_feature(objc_arc)
@property (nonatomic, weak) id<FayeCppClientDelegate> delegateObject;
@property (nonatomic, strong) NSString * urlStringObject;
#endif

@end


@implementation FayeCppClient

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
		VariantMap map;
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
	SAFE_RETAIN(_urlStringObject, url);
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
		
		_cppClient = new Client();
		if (!_cppClient)
		{
			delete _cppDelegate;
#if !__has_feature(objc_arc)
			[self release];
#endif
			return nil;
		}
		
		_cppClient->setDelegate(_cppDelegate);
	}
	return self;
}

- (void) dealloc
{
	_cppDelegate->objcClient = nil;
	delete _cppDelegate;
	
	
	if (_cppClient)
	{
		_cppClient->setDelegate(NULL);
		delete _cppClient;
	}

#if !__has_feature(objc_arc)
	if (_urlStringObject) [_urlStringObject release];
	[super dealloc];
#endif
}

@end

