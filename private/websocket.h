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


#ifndef __FAYECPP_FAYE_CLIENT_H__
#define __FAYECPP_FAYE_CLIENT_H__


#include "../common.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

#if !defined(HAVE_SUITABLE_QT_VERSION) && defined(HAVE_LIBWEBSOCKETS_H)

#include "../delegate.h"
#include "classmethodwrapper.h"
#include "transport.h"
#include "REThread.h"
#include "REMutex.h"
#include "REBuffer.h"

namespace FayeCpp {
	
	class WebSocket : protected REThread, public Transport
	{
	private:
		std::list<REBuffer *> _writeBuffers;
		struct lws_context_creation_info _info;
		struct libwebsocket_context * _context;
		struct libwebsocket * _connection;
		
		REMutex * _writeMutex;
		
		#define MAX_ECHO_PAYLOAD 4096
		typedef struct echoSessionData
		{
			unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + MAX_ECHO_PAYLOAD + LWS_SEND_BUFFER_POST_PADDING];
			unsigned int len;
			unsigned int index;
		} EchoSessionData;
		
		void writeLock();
		void writeUnlock();
		
		static struct libwebsocket_protocols protocols[];
		static int callbackEcho(struct libwebsocket_context * context,
								struct libwebsocket * wsi,
								enum libwebsocket_callback_reasons reason,
								void * user,
								void * input,
								size_t len);

		void onCallbackEstablished();
		void onCallbackConnectionError();
		int onCallbackWritable(struct libwebsocket_context * context,
							   struct libwebsocket * connection,
							   EchoSessionData * pss);
		void onCallbackReceive(void * input, size_t len);
		
		void addWriteBufferData(const unsigned char * data, const size_t dataSize, const enum libwebsocket_write_protocol type);
		void cleanup();
		
	protected:
		virtual void threadBody();
		
	public:
		virtual const std::string name() const;
		
		virtual void sendData(const std::vector<unsigned char> & data);
		virtual void sendText(const std::string & text);
		virtual void connectToServer();
		virtual void disconnectFromServer();
		WebSocket(ClassMethodWrapper<Client, void(Client::*)(Message*), Message> * processMethod);
		virtual ~WebSocket();
		
		static std::string transportName();
	};
	
}

#endif /* HAVE_SUITABLE_QT_VERSION */

#endif /* __FAYECPP_FAYE_CLIENT_H__ */
