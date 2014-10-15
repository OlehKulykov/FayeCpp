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


#include <assert.h>
#include <fayecpp.h>

using namespace FayeCpp;

Client * _client = NULL;


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
	
	
	FayeDelegate() { }
	virtual ~FayeDelegate() { }
};

class FayeSSLDataSource : public SSLDataSource
{
public:
	virtual FayeCpp::REString clientLocalCertificateFilePath() const
	{
		return REString("client.crt");
	}
	
	virtual FayeCpp::REString clientPrivateKeyFilePath() const
	{
		return REString("client.key");
	}
	
	virtual FayeCpp::REString clientPrivateKeyPassPhrase() const
	{
		return REString("password");
	}
	
	virtual FayeCpp::REString clientCACertificateFilePath() const
	{
		return REString("ca.crt");
	}
	
	FayeSSLDataSource() { }
	virtual ~FayeSSLDataSource() { }
};

FayeDelegate * _delegate = NULL;
FayeSSLDataSource * _SSLDataSource = NULL;

int test_create()
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

int test_delete()
{
	_client->setDelegate(NULL);
	_client->setSSLDataSource(NULL);
	
	delete _delegate;
	delete _SSLDataSource;
	delete _client;
	
	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) 
{
	assert(test_create() == EXIT_SUCCESS);
	
	assert(test_delete() == EXIT_SUCCESS);
	
	return EXIT_SUCCESS;
}
