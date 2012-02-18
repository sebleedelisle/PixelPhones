/*
 *  WebSocketClient.h
 *  SocketServer
 *
 *  Created by Seb Lee-Delisle on 08/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"; 
#include "md5.h"
#include "ofxNetwork.h"

class WebSocketClient { 
	
	public : 
	WebSocketClient();
	void setup(int id, ofxTCPClient * client); 
	void update(); 
	
	string procClientHeader(string header, int port);
	long getKeyValue(string str);
	bool isConnected(); 
	//void longToByteArray(long value); 
	
	void trim(string* s);
	ofxTCPClient * tcpClient;
	
	bool handShaked;
	int portNum; 

	string receiveString;
	
	


}; 
