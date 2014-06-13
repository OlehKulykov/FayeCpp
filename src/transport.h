/*
 *   Copyright 2014 Kulykov Oleh
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */


#ifndef __FAYECPP_TRANSPORT_H__
#define __FAYECPP_TRANSPORT_H__

#include "../fayecpp.h"
#include "classmethodwrapper.h"

namespace FayeCpp {
	
	class Transport
	{
	private:
		REString _url;
		REString _host;
		REString _path;
		ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * _processMethod;
		int _port;
		bool _isUseSSL;
		bool _isConnected;
		
		Transport();
		
		Client * client();
		Delegate * delegate();
		
	protected:
		void onConnected();
		void onDisconnected();
		void onTextReceived(const char * text);
		void onDataReceived(const unsigned char * data, const size_t dataSize);
		void onError(const REString & error);
		
	public:
		bool isConnected() const;
		void setUrl(const char * url);
		
		const REString & url() const;
		const REString & host() const;
		
		/**
		 @brief Should start with '/'
		 */
		const REString & path() const;
		int port() const;
		bool isUseSSL() const;
		
		virtual const REString name() const = 0;

		virtual void sendData(const unsigned char * data, const size_t dataSize) = 0;
		virtual void sendText(const char * text, const size_t textSize) = 0;

		virtual void connectToServer() = 0;
		virtual void disconnectFromServer() = 0;
		
		Transport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod);
		
		virtual ~Transport();

        static Transport * createNewTransport(ClassMethodWrapper<Client, void(Client::*)(Responce*), Responce> * processMethod);
        static REStringList availableConnectionTypes();
	};
	
}

#endif /* __FAYECPP_TRANSPORT_H__ */
