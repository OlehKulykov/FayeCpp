

#import "AppDelegate.h"
#include "../../../../fayecpp.h"
#include "../../../../src/FCJsonUtils.h"

#include <signal.h>
#include <stdlib.h>
#include <thread>

#define OBJC_CLIENT 1

class FayeDelegate;
static NSString * const _clientURL = @"https://localhost:6001/faye";
static NSString * const _clientChannel1 = @"/xxxxxxxx/xxxxxxxxxxxxx";
static NSString * const _clientChannel2 = @"/xxxxxx";


#if defined(OBJC_CLIENT)
#import "FayeCppClient.h"
#else
FayeCpp::Client * _client = NULL;
FayeDelegate * _delegate = NULL;

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

		FayeCpp::RELog::log("Try reconnect.");
		client->connect();
		client->subscribeToChannel([_clientChannel1 UTF8String]);
		client->subscribeToChannel([_clientChannel2 UTF8String]);
		FayeCpp::RELog::log("Done reconnect.");
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
														const FayeCpp::REVariantMap & message, 
														const FayeCpp::REString & channel)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientReceivedMessageFromChannel");
	}
	
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::REVariantMap & message)
	{
		FayeCpp::RELog::log("DELEGATE onFayeClientWillSendMessage");
	}
	
	virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client, 
												FayeCpp::REVariantMap & message)
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
#endif

using namespace FayeCpp;

@interface AppDelegate()
#if defined(OBJC_CLIENT)
<FayeCppClientDelegate, FayeCppClientSSLDataSource>

@property (nonatomic, strong) FayeCppClient * client;
#endif
@end

@implementation AppDelegate

- (IBAction) onSendText:(id)sender
{
#if defined(OBJC_CLIENT)
	if (_client) 
	{
		[_client sendMessage:@{@"text" : [_textField stringValue]} 
				   toChannel:_clientChannel1];
	}
#else
	if (_client) 
	{
		REVariantMap message;
				
		RELog::log("Try send text: %s", [[_textField stringValue] UTF8String]);
		
		message["text"] = [[_textField stringValue] UTF8String];
		
		_client->sendMessageToChannel(message, [_clientChannel1 UTF8String]);
	}
#endif
	NSLog(@"Done");
}

- (IBAction) onCreate:(id)sender
{
	NSLog(@"SSL: %@", FayeCpp::Client::isSupportsSSLConnection() ? @"YES" : @"NO");
	
#if defined(OBJC_CLIENT)
	FayeCppClient * client = self.client;
	self.client = nil;
	if (client) 
	{
		[client setDelegate:nil];
		[client setSSLDataSource:nil];
	}

	client = [[FayeCppClient alloc] init];
	client.usingAdviceReconnect = YES;
	[client setDelegate:self];
	[client setSSLDataSource:self];
	[client setUrlString:_clientURL];
	self.client = client;
#else
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

	_client->setExtValue(22222);
	REVariant v = _client->extValue();

	_client->setExtValue(REVariant());
	v = _client->extValue();

	_client->setUsingIPV6(false);
	_client->setUrl([_clientURL UTF8String]);
	_client->setDelegate(_delegate);
//	_client->setSSLDataSource(new FayeSSLDataSource());
#endif
	NSLog(@"Done");
}

- (IBAction) onConnect:(id)sender
{
#if defined(OBJC_CLIENT)
	[_client connect];
	[_client subscribeToChannel:_clientChannel1];
	[_client subscribeToChannel:_clientChannel2];
#else
	_client->connect();
	_client->subscribeToChannel([_clientChannel1 UTF8String]);
	_client->subscribeToChannel([_clientChannel2 UTF8String]);
#endif
	NSLog(@"Done");
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application
	NSLog(@"Client info: %s", FayeCpp::Client::info());
}

#if defined(OBJC_CLIENT)
#pragma mark - FayeCppClientDelegate & FayeCppClientSSLDataSource
- (void) onFayeTransportConnected:(FayeCppClient *) client
{
	NSLog(@"onFayeTransportConnected");
}

- (void) onFayeTransportDisconnected:(FayeCppClient *) client
{
	NSLog(@"onFayeTransportDisconnected");
}

- (void) onFayeClientConnected:(FayeCppClient *) client
{
	NSLog(@"onFayeClientConnected");
}

- (void) onFayeClientDisconnected:(FayeCppClient *) client
{
	NSLog(@"onFayeClientDisconnected");
}

- (void) onFayeClient:(FayeCppClient *) client
  subscribedToChannel:(NSString *) channel
{
	NSLog(@"onFayeClient subscribedToChannel: %@", channel);
}

- (void) onFayeClient:(FayeCppClient *) client
unsubscribedFromChannel:(NSString *) channel
{
	NSLog(@"onFayeClient unsubscribedFromChannel: %@", channel);
}

- (void) onFayeClient:(FayeCppClient *) client
	  receivedMessage:(NSDictionary *) message
		  fromChannel:(NSString *) channel
{
	NSLog(@"onFayeClient receivedMessage fromChannel: %@ \n%@", channel, message);
}

- (void) onFayeClient:(FayeCppClient *) client
				error:(NSError *) error
{
	NSLog(@"onFayeClient error: %@", error);
}
#endif

@end
