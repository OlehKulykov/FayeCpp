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

FayeCpp::Client * client = NULL;

class FayeDelegate : public FayeCpp::Delegate
{
public:
	virtual void onFayeTransportConnected(FayeCpp::Client * client)
	{
		FayeCpp::RELog::log("DELEGATE onFayeTransportConnected");
	}
	
	virtual void onFayeTransportDisconnected(FayeCpp::Client * client)
	{
		FayeCpp::RELog::log("DELEGATE onFayeTransportDisconnected");
		delete client;
//		client->connect();
	}
	
	virtual void onFayeClientConnected(FayeCpp::Client * client)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientConnected");
	}
	
	virtual void onFayeClientDisconnected(FayeCpp::Client * client)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientDisconnected");
	}
	
	virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client, 
												 const FayeCpp::REString & channel)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientSubscribedToChannel");
	}
	
	virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client, 
													 const FayeCpp::REString & channel)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientUnsubscribedFromChannel");
	}
	
	virtual void onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client, 
														const FayeCpp::VariantMap & message, 
														const FayeCpp::REString & channel)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientReceivedMessageFromChannel");
	}
	
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::VariantMap & message)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientWillSendMessage");
	}
	
	virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client, 
												FayeCpp::VariantMap & message)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientWillReceiveMessage");
	}
	
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{
		FayeCpp::RELog::log("DELEGATE ERROR: %s", errorString.UTF8String());
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
	if (client) 
	{
		VariantMap message;
				
		RELog::log("Try send text: %s", [[_textField stringValue] UTF8String]);
		
		message["text"] = [[_textField stringValue] UTF8String];
		
		client->sendMessageToChannel(message, "/seminars/5322e93d8ee60a422400008f");
	}
	NSLog(@"Done");
}

- (IBAction) onCreate:(id)sender
{
	NSLog(@"SSL: %@", FayeCpp::Client::isSupportsSSLConnection() ? @"YES" : @"NO");
	
	if (client) 
	{
		delete client;
		client = NULL;
	}
	
	client = new FayeCpp::Client();
	client->setUsingIPV6(false);
	client->setUrl("http://messages.presentain.com:80/faye");
	//client->setUrl("https://localhost:6001/faye");
	client->setDelegate(new FayeDelegate());
//	client->setSSLDataSource(new FayeSSLDataSource());
	NSLog(@"Done");
}

- (IBAction) onConnect:(id)sender
{
	client->connect();
	client->subscribeToChannel("/seminars/5322e93d8ee60a422400008f");
	client->subscribeToChannel("/seminars_service/5322e93d8ee60a422400008f");
	NSLog(@"Done");
}

static void doSomeWork(void)
{
	
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{	
	
	std::thread t(doSomeWork);
	t.join();
	
	// Insert code here to initialize your application
}

@end
