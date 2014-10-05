
#include <jni.h>
#include "TestFayeCpp.h"

#include <fayecpp.h>

class FayeCppDelegateWrapper;

FayeCpp::Client * _client = NULL;
FayeCppDelegateWrapper * _delegate = NULL;

using namespace FayeCpp;

static int initializeClient();
static int connectClient();

class FayeCppDelegateWrapper : public Delegate
{
public:
	virtual void onFayeTransportConnected(FayeCpp::Client * client)
	{
		RELog::log("FayeCppDelegate: onFayeTransportConnected");
	}
	
	virtual void onFayeTransportDisconnected(FayeCpp::Client * client)
	{
		RELog::log("FayeCppDelegate: onFayeTransportDisconnected");
	}
	
	virtual void onFayeClientConnected(FayeCpp::Client * client)
	{
		RELog::log("FayeCppDelegate: onFayeClientConnected");
	}
	
	virtual void onFayeClientDisconnected(FayeCpp::Client * client)
	{
		RELog::log("FayeCppDelegate: onFayeClientDisconnected");
	}
	
	virtual void onFayeClientSubscribedToChannel(FayeCpp::Client * client,
												 const FayeCpp::REString & channel)
	{
		RELog::log("FayeCppDelegate: onFayeClientSubscribedToChannel: %s", channel.UTF8String());
	}
	
	virtual void onFayeClientUnsubscribedFromChannel(FayeCpp::Client * client,
													 const FayeCpp::REString & channel)
	{
		RELog::log("FayeCppDelegate: onFayeClientUnsubscribedFromChannel");
	}
	
	virtual void onFayeClientReceivedMessageFromChannel(FayeCpp::Client * client,
														const FayeCpp::VariantMap & message,
														const FayeCpp::REString & channel)
	{
		RELog::log("FayeCppDelegate: onFayeClientReceivedMessageFromChannel: %s", channel.UTF8String());
	}
	
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::VariantMap & message)
	{
		RELog::log("FayeCppDelegate: onFayeClientWillSendMessage");
	}
	
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{
		RELog::log("FayeCppDelegate: onFayeErrorString: %s", errorString.UTF8String());
	}
	
	FayeCppDelegateWrapper()
	{
		RELog::log("FayeCppDelegate: FayeCppDelegateWrapper()");
	}
	
	virtual ~FayeCppDelegateWrapper()
	{
		RELog::log("FayeCppDelegate: ~FayeCppDelegateWrapper()");
	}
};

static int initializeClient()
{
	RELog::log("Initialize Faye Client...");
	if (_client)
	{
		RELog::log("Delete prev client");
		_client->setDelegate(NULL);
		delete _client;
		_client = NULL;
	}
	
	
	if (!_delegate) 
	{
		RELog::log("Try create new FayeCppDelegateWrapper...");
		_delegate = new FayeCppDelegateWrapper();
		if (!_delegate) 
		{
			RELog::log("FayeCppDelegateWrapper creation ERROR");
			return 0;
		}
	}
	
	
	RELog::log("Try create new client");
	Client * newClient = new Client();
	if (newClient)
	{
		RELog::log("Client created");
		newClient->setDelegate(_delegate);
		_client = newClient;
		
		_client->setUrl("xx://xxxxxxxx.xxxxxxx:80/faye");
		
		return 1;
	}
	
	
	RELog::log("Client creation ERROR");
	return 0;
}

static int connectClient()
{
	RELog::log("Try to connect...");
	
	if (!_client)
	{
		RELog::log("Client not created");
		return 0;
	}
	
	_client->connect();
	
	_client->subscribeToChannel("/xxxxxxxx/xxxxxxxxxxxxxxxxx");
	_client->subscribeToChannel("/xxxxxxxxxxx/xxxxxxxxxxxxxxxxxxxxx");
	
	return 1;
}


jint JNICALL Java_com_testfayecpp_MainActivity_initializeFayeClient
  (JNIEnv * env, jobject obj)
{
	return initializeClient();
}

/*
 * Class:     com_testfayecpp_MainActivity
 * Method:    connectFayeClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_testfayecpp_MainActivity_connectFayeClient
  (JNIEnv * env, jobject obj)
{
	return connectClient();
}

/*
 * Class:     com_testfayecpp_MainActivity
 * Method:    sentTestMessageFayeClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_testfayecpp_MainActivity_sentTestMessageFayeClient
  (JNIEnv * env, jobject obj)
{
	return 1;
}

/*
 * Class:     com_testfayecpp_MainActivity
 * Method:    sentTextMessageFayeClient
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_testfayecpp_MainActivity_sentTextMessageFayeClient
  (JNIEnv * env, jobject obj, jstring str)
{	
	VariantMap message;
	
	const char * nativeString = env->GetStringUTFChars(str, NULL);
	
	RELog::log("Try send text: %s", nativeString);
	
	message["text"] = nativeString;
	env->ReleaseStringUTFChars(str, nativeString);
	
	_client->sendMessageToChannel(message, "/xxxxxxxxxx/xxxxxxxxxxxxxxxxxx");
	
	RELog::log("Send text OK");
	
	return 1;
}

/*
 * Class:     com_testfayecpp_MainActivity
 * Method:    disconnectFayeClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_testfayecpp_MainActivity_disconnectFayeClient
  (JNIEnv * env, jobject obj)
{
	return 1;
}

