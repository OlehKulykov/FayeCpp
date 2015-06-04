/*
 *   Copyright (c) 2014 - 2015 Kulykov Oleh <info@resident.name>
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


#include <stdlib.h>
#include <assert.h>

#if defined(CMAKE_BUILD)
#undef CMAKE_BUILD
#endif

#if defined(__BUILDING_RECORE_DYNAMIC_LIBRARY__)
#undef __BUILDING_RECORE_DYNAMIC_LIBRARY__
#endif


#include <fayecpp.h>

#ifndef CLIENT_URL_WS
#define CLIENT_URL_WS "ws://localhost:6001/faye"
#endif

#ifndef CLIENT_URL_WSS
#define CLIENT_URL_WSS "wss://localhost:6001/faye"
#endif

#ifndef CLIENT_CHANNEL
#define CLIENT_CHANNEL "/channel1"
#endif

#ifndef LOCAL_SERTIFICATE_FILE_PATH
#define LOCAL_SERTIFICATE_FILE_PATH "client.crt"
#endif

#ifndef PRIVATE_KEY_FILE_PATH
#define PRIVATE_KEY_FILE_PATH "client.key"
#endif

#ifndef PRIVATE_KEY_PASSPHASE
#define PRIVATE_KEY_PASSPHASE "password"
#endif

#ifndef CA_SERTIFICATE_FILE_PATH
#define CA_SERTIFICATE_FILE_PATH "ca.crt"
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(o) if(o){delete o;o=NULL;}
#endif

using namespace FayeCpp;

class FayeDelegate;
class FayeSSLDataSource;
class FayeDelegateAndSSLDataSource;

static Client * _client = NULL;
static FayeDelegate * _delegate = NULL;
static FayeSSLDataSource * _SSLDataSource = NULL;
static FayeDelegateAndSSLDataSource * _delegateAndSSLDataSource = NULL;

class FayeDelegate : public Delegate
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
														const FayeCpp::REVariantMap & message, 
														const FayeCpp::REString & channel)
	{

	}
	
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::REVariantMap & message)
	{

	}
	
	virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client, 
												FayeCpp::REVariantMap & message)
	{

	}
	
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{

	}
	
	
	FayeDelegate() { }
	virtual ~FayeDelegate() { }
};

class FayeSSLDataSource : public SSLDataSource
{
public:
	virtual FayeCpp::REString clientLocalCertificateFilePath() const
	{
		return REString(LOCAL_SERTIFICATE_FILE_PATH);
	}
	
	virtual FayeCpp::REString clientPrivateKeyFilePath() const
	{
		return REString(PRIVATE_KEY_FILE_PATH);
	}
	
	virtual FayeCpp::REString clientPrivateKeyPassPhrase() const
	{
		return REString(PRIVATE_KEY_PASSPHASE);
	}
	
	virtual FayeCpp::REString clientCACertificateFilePath() const
	{
		return REString(CA_SERTIFICATE_FILE_PATH);
	}
	
	FayeSSLDataSource() { }
	virtual ~FayeSSLDataSource() { }
};

class FayeDelegateAndSSLDataSource : public Delegate, public SSLDataSource
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
														const FayeCpp::REVariantMap & message, 
														const FayeCpp::REString & channel)
	{
		
	}
	
	virtual void onFayeClientWillSendMessage(FayeCpp::Client * client,
											 FayeCpp::REVariantMap & message)
	{
		
	}
	
	virtual void onFayeClientWillReceiveMessage(FayeCpp::Client * client, 
												FayeCpp::REVariantMap & message)
	{
		
	}
	
	virtual void onFayeErrorString(FayeCpp::Client * client,
								   const FayeCpp::REString & errorString)
	{
		
	}
	
	virtual FayeCpp::REString clientLocalCertificateFilePath() const
	{
		return REString();
	}
	
	virtual FayeCpp::REString clientPrivateKeyFilePath() const
	{
		return REString();
	}
	
	virtual FayeCpp::REString clientPrivateKeyPassPhrase() const
	{
		return REString();
	}
	
	virtual FayeCpp::REString clientCACertificateFilePath() const
	{
		return REString();
	}
	
	FayeDelegateAndSSLDataSource() { }
	virtual ~FayeDelegateAndSSLDataSource() { }
};


int testCreate()
{
	_client = new Client();
	if (!_client) return 1;
	
	_delegate = new FayeDelegate();
	if (!_delegate) return 2;
	
	_SSLDataSource = new FayeSSLDataSource();
	if (!_SSLDataSource) return 3;
	
	_client->setDelegate(_delegate);
	_client->setSSLDataSource(_SSLDataSource);
	
	if (_client->delegate() != _delegate) return 4;
	if (_client->sslDataSource() != _SSLDataSource) return 5;
	
	return EXIT_SUCCESS;
}

int testCreate2()
{
	_client = new Client();
	if (!_client) return 1;
	
	_delegateAndSSLDataSource = new FayeDelegateAndSSLDataSource();
	if (!_delegateAndSSLDataSource) return 2;
		
	_client->setDelegate(_delegateAndSSLDataSource);
	_client->setSSLDataSource(_delegateAndSSLDataSource);
	
	if (_client->delegate() != _delegateAndSSLDataSource) return 3;
	if (_client->sslDataSource() != _delegateAndSSLDataSource) return 4;
	
	return EXIT_SUCCESS;
}

int testSSLDataSourcePaths()
{
	if (!_client) return 1;
	if (!_client->sslDataSource()) return 2;
	
	if (!_client->sslDataSource()->clientLocalCertificateFilePath().isEqual(LOCAL_SERTIFICATE_FILE_PATH)) 
		return 3;
	
	if (!_client->sslDataSource()->clientPrivateKeyFilePath().isEqual(PRIVATE_KEY_FILE_PATH)) 
		return 4;
	
	if (!_client->sslDataSource()->clientPrivateKeyPassPhrase().isEqual(PRIVATE_KEY_PASSPHASE)) 
		return 5;
	
	if (!_client->sslDataSource()->clientCACertificateFilePath().isEqual(CA_SERTIFICATE_FILE_PATH)) 
		return 6;
	
	return EXIT_SUCCESS;
}

int testClientConnectionSettings()
{
	if (!_client) return 1;
	
	_client->setUrl(CLIENT_URL_WS);
	if (_client->isUseSSL()) return 2;
	if (!_client->url().isEqual(CLIENT_URL_WS)) return 3;
	
	_client->setUrl(CLIENT_URL_WSS);
	if (!_client->isUseSSL()) return 4;
	if (!_client->url().isEqual(CLIENT_URL_WSS)) return 5;
	
	return EXIT_SUCCESS;
}

int testDelete2()
{
	_client->setDelegate(NULL);
	_client->setSSLDataSource(NULL);

	delete _delegateAndSSLDataSource;
	delete _client;
	
	_delegateAndSSLDataSource = NULL;
	_client = NULL;
	
	return EXIT_SUCCESS;
}

int testDelete()
{
	_client->setDelegate(NULL);
	_client->setSSLDataSource(NULL);
	
	delete _delegate;
	delete _SSLDataSource;
	delete _client;
	
	_delegate = NULL;
	_SSLDataSource = NULL;
	_client = NULL;
	
	return EXIT_SUCCESS;
}

int testExternUserInfo()
{
	REVariantMap info;

	info[kErrorLocalizedDescriptionKey] = kErrorLocalizedDescriptionKey;
	info[kErrorPlaceInTheCodeKey] = kErrorPlaceInTheCodeKey;
	info[kErrorURLKey] = kErrorURLKey;
	info[kErrorChannelKey] = kErrorChannelKey;

	REVariant * result = info.findTypedValue(kErrorLocalizedDescriptionKey, REVariant::TypeString);
	if (!result) return 1;

	result = info.findTypedValue(kErrorPlaceInTheCodeKey, REVariant::TypeString);
	if (!result) return 2;

	result = info.findTypedValue(kErrorURLKey, REVariant::TypeString);
	if (!result) return 3;

	result = info.findTypedValue(kErrorChannelKey, REVariant::TypeString);
	if (!result) return 4;

	return EXIT_SUCCESS;
}

int testExterns()
{
	Error * error = new Error(kErrorDomainClient, 0, REVariantMap());
	if (error && !error->domain().isEqual(kErrorDomainClient))
	{
		return 1;
	}

	SAFE_DELETE(error)
	error = new Error(kErrorDomainTransport, 0, REVariantMap());
	if (error && !error->domain().isEqual(kErrorDomainTransport))
	{
		return 2;
	}

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) 
{
	RELog::log("Client info: %s", Client::info());
	RELog::log("Start test");

	RELog::log("Test create ...");
	int test = testCreate();
	assert(test == EXIT_SUCCESS);
	test = testDelete();
	assert(test == EXIT_SUCCESS);
	RELog::log("Test create OK");
	
	RELog::log("Test client connection settings ...");
	test = testCreate();
	assert(test == EXIT_SUCCESS);
	test = testClientConnectionSettings();
	assert(test == EXIT_SUCCESS);
	test = testDelete();
	assert(test == EXIT_SUCCESS);
	RELog::log("Test client connection settings OK");
	
	RELog::log("Test SSL data source paths ...");
	test = testCreate();
	assert(test == EXIT_SUCCESS);
	test = testSSLDataSourcePaths();
	assert(test == EXIT_SUCCESS);
	test = testDelete();
	assert(test == EXIT_SUCCESS);
	RELog::log("Test SSL data source paths OK");
	
	RELog::log("Test create2 ...");
	test = testCreate2();
	assert(test == EXIT_SUCCESS);
	test = testDelete2();
	assert(test == EXIT_SUCCESS);
	RELog::log("Test create2 OK");

	RELog::log("Test extern user info ...");
	test = testExternUserInfo();
	assert(test == EXIT_SUCCESS);
	RELog::log("Test extern user info OK");

	RELog::log("Test externs ...");
	test = testExterns();
	assert(test == EXIT_SUCCESS);
	RELog::log("Test externs OK");

	RELog::log("All done.");
	return EXIT_SUCCESS;
}
