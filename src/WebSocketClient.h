
#pragma once

#include "ofxTCPClient.h"
#include "ofMain.h"
#include "md5.h"

class WebSocketClient : public ofxTCPClient {


	public:

		WebSocketClient();
		virtual bool setup(int _index, bool blocking);

		virtual bool send(string message);
		virtual void setMessageDelimiter(string delim){
			ofLog(OF_LOG_WARNING, "Attempt to set the message delimiter on a WebSocket - you shouldn't do this :) "); 
		};
		virtual string receive();
	
		string procClientHeader(string header, int port);
		long getKeyValue(string str);		
		void trim(string* s);

	
	
		string messagePrefix; 
		bool handshakeComplete; 
		int serverPortNum;
	bool verbose; 
	
};



