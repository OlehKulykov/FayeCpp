//
//  AppDelegate.m
//  test_mac
//
//  Created by Resident Evil on 29.07.14.
//  Copyright (c) 2014 re. All rights reserved.
//

#import "AppDelegate.h"
#include "../../../../fayecpp.h"
#include "../../../../src/jsonutils.h"

#include <signal.h>

#include <stdlib.h>
#include <thread>

class FayeDelegate;

FayeCpp::Client * _client = NULL;
FayeDelegate * _delegate = NULL;

class FayeDelegate : public FayeCpp::Delegate
{
public:
	virtual void onFayeTransportConnected(FayeCpp::Client * client)
	{
//		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeTransportConnected");
//		});
	}
	
	virtual void onFayeTransportDisconnected(FayeCpp::Client * client)
	{
//		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeTransportDisconnected");
			
			NSLog(@"Try reconnect.");
			client->connect();
			client->subscribeToChannel("/seminars/5322e93d8ee60a422400008f");
			client->subscribeToChannel("/seminars_service/5322e93d8ee60a422400008f");
			NSLog(@"Done reconnect.");
//		});
	}
	
	virtual void onFayeClientConnected(FayeCpp::Client * client)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeClientConnected");
		});
	}
	
	virtual void onFayeClientDisconnected(FayeCpp::Client * client)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeClientDisconnected");
		});
	}
	
	virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client, 
												 const FayeCpp::REString & channel)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeClientSubscribedToChannel");
		});
	}
	
	virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client, 
													 const FayeCpp::REString & channel)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeClientUnsubscribedFromChannel");
		});
	}
	
	virtual void onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client, 
														const FayeCpp::REVariantMap & message, 
														const FayeCpp::REString & channel)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeClientReceivedMessageFromChannel");
		});
	}
	
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::REVariantMap & message)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeClientWillSendMessage");
		});
	}
	
	virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client, 
												FayeCpp::REVariantMap & message)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE onFayeClientWillReceiveMessage");
		});
	}
	
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{
		dispatch_async(dispatch_get_main_queue(), ^{
			FayeCpp::RELog::log("DELEGATE ERROR: %s", errorString.UTF8String());
		});
	}
	
	
	FayeDelegate() { }
	virtual ~FayeDelegate() { }
};

class FayeSSLDataSource : public FayeCpp::SSLDataSource
{
public:
	virtual FayeCpp::REString clientLocalCertificateFilePath() const
	{
		return FayeCpp::REString("/Volumes/Data/faye_server/node/client1.crt");
	}
	
	virtual FayeCpp::REString clientPrivateKeyFilePath() const
	{
		return FayeCpp::REString("/Volumes/Data/faye_server/node/client1.key");
	}
	
	virtual FayeCpp::REString clientPrivateKeyPassPhrase() const
	{
		return FayeCpp::REString("Q1w2E3");
	}

	virtual FayeCpp::REString clientCACertificateFilePath() const
	{
		return FayeCpp::REString("/Volumes/Data/faye_server/node/ca.crt");
	}
	
	virtual ~FayeSSLDataSource() { }
};

using namespace FayeCpp;

@implementation AppDelegate

- (IBAction) onSendText:(id)sender
{
	if (_client) 
	{
		REVariantMap message;
				
		RELog::log("Try send text: %s", [[_textField stringValue] UTF8String]);
		
		message["text"] = [[_textField stringValue] UTF8String];
		
		_client->sendMessageToChannel(message, "/seminars/5322e93d8ee60a422400008f");
	}
	NSLog(@"Done");
}

- (IBAction) onCreate:(id)sender
{
	NSLog(@"SSL: %@", FayeCpp::Client::isSupportsSSLConnection() ? @"YES" : @"NO");
	
	if (_client) 
	{
		delete _client;
		_client = NULL;
	}
	
	if (_delegate)
	{
		delete _delegate;
		_delegate = NULL;
	}
	
	_delegate = new FayeDelegate();
	
	_client = new FayeCpp::Client();
	
	_client->setUsingIPV6(false);
	_client->setUrl("http://messages.presentain.com:80/faye");
	//_client->setUrl("https://localhost:6001/faye");
	_client->setDelegate(_delegate);
//	_client->setSSLDataSource(new FayeSSLDataSource());
	NSLog(@"Done");
}

- (IBAction) onConnect:(id)sender
{
	_client->connect();
	_client->subscribeToChannel("/seminars/5322e93d8ee60a422400008f");
	_client->subscribeToChannel("/seminars_service/5322e93d8ee60a422400008f");
	NSLog(@"Done");
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{	
	// Insert code here to initialize your application
	NSLog([NSString stringWithUTF8String:FayeCpp::Client::info()]);
}

@end
