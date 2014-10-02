
#include <jni.h>
#include "TestFayeCpp.h"

#include <fayecpp.h>

class FayeCppDelegateWrapper;

static FayeCpp::Client * _client = NULL;
static FayeCppDelegateWrapper * _delegate = NULL;
static JavaVM * _javaVM = NULL;

using namespace FayeCpp;

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


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM * vm, void * reserved)
{
	RELog::log("JNI_OnLoad()");

	_javaVM = vm;

	/// Look for minimum versions in all Application.mk
	/// which is used in the whole app
	return JNI_VERSION_1_4;
}


JNIEXPORT void JNICALL JNI_OnUnload(JavaVM * vm, void * reserved)
{
	RELog::log("JNI_OnUnload()");
}


JNIEXPORT jint JNICALL Java_com_testfayecpp_MainActivity_initializeFayeClient
  (JNIEnv * env, jobject obj)
{
	RELog::log("Initialize Faye Client...");
	if (_client)
	{
		RELog::log("Delete prev client");
		delete _client;
		_client = NULL;
	}

	if (_delegate) 
	{
		delete _delegate;
		_delegate = NULL;
	}
	
	RELog::log("Try create new FayeCppDelegateWrapper...");
	_delegate = new FayeCppDelegateWrapper();
	if (!_delegate) 
	{
		RELog::log("FayeCppDelegateWrapper creation ERROR");
		return 0;
	}

	RELog::log("Try create new client");
	Client * newClient = new Client();
	if (newClient)
	{
		RELog::log("Client created");
		newClient->setDelegate(_delegate);
		_client = newClient;
		
		_client->setUrl("ws://messages.presentain.com:80/faye");
		
		return 1;
	}

	RELog::log("Client creation ERROR");
	return 0;
}

/*
 * Class:     com_testfayecpp_MainActivity
 * Method:    connectFayeClient
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_testfayecpp_MainActivity_connectFayeClient
  (JNIEnv * env, jobject obj)
{
	RELog::log("Try to connect...");
	
	if (!_client)
	{
		RELog::log("Client not created");
	}
	
	_client->connect();
	
	_client->subscribeToChannel("/seminars/5322e93d8ee60a422400008f");
	_client->subscribeToChannel("/seminars_service/5322e93d8ee60a422400008f");
	
	return 1;
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

