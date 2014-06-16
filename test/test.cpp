
#include <stdio.h>

#include <fayecpp.h>

class FayeClientDelegate : public FayeCpp::Delegate
{
public:
	virtual void onFayeTransportConnected(FayeCpp::Client * client)
	{
		
	}
	
	virtual void onFayeTransportDisconnected(FayeCpp::Client * client)
	{
		
	}
	
	virtual void onFayeClientConnected(FayeCpp::Client * client)
	{
		
	}
	
	virtual void onFayeClientDisconnected(FayeCpp::Client * client)
	{
		
	}
	
	virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client, 
												 const FayeCpp::REString & channel)
	{
		
	}
	
	virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client, 
													 const FayeCpp::REString & channel)
	{
		
	}
	
	virtual void onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client, 
														const FayeCpp::VariantMap & message, 
														const FayeCpp::REString & channel)
	{
		
	}
	
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::VariantMap & message)
	{
		
	}
	
	virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client, 
												FayeCpp::VariantMap & message)
	{
		
	}
	
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{
		
	}
	
	
	FayeClientDelegate() { }
	virtual ~FayeClientDelegate() { }
};

static FayeCpp::Client * _client = NULL;
static FayeClientDelegate * _delegate = NULL;


int testInit()
{
	_client = new FayeCpp::Client();
	_delegate = new FayeClientDelegate();
	if (!_client || !_delegate) return 1;
	
	_client->setUrl("ws://host:80/path");
	_client->setDelegate(_delegate);
	_client->connect();
	_client->subscribeToChannel("/channel");
	
	return 0;
}



