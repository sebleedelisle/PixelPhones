/*
 *  ConnectedPhone.h
 *  PixelPhones02
 *
 *  Created by Seb Lee-Delisle on 23/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */
#pragma once
#include "WebSocketClient.h"
#include "ofMain.h"
#include <string.h>
#include <ctime>

#define NOT_SYNCED -1
#define SYNC_IN_PROGRESS 0
#define SYNCED 1

class ConnectedPhone  {
	
	public : 
	void setup(WebSocketClient * webclient, int id);
	void update(); 
	void draw(int brightness); 
	
	void sendColour(ofColor col, int latency = 0, float fadeUp = 0, float fadeDown = 0) ;
	void sendFrameRate(int framerate); 
	void sendNumBits(int numbits); 
	
	void sendMsg(string msg) ;
	void broadcastID() ; 
	void stopBroadcastingID() ; 
	bool isConnected(); 
	void close(); 
	void sendQueue(); 
	void startSync(); 
	
	bool hitTest(int x, int y);
	void startDragging(int x, int y); 
	void stopDragging(); 
	
	bool isDragging; 
	ofVec2f dragOffset; 
	
	
	ofxTCPServer * tcp; 
	
	void reset(int id); // test for trying different ids
	
	WebSocketClient * webClient;
	
	bool found; 
	bool connectionReady; 
	ofVec2f pixelPosition;
	ofVec2f unitPosition;
	int ID; 
	
	int counter; 
	
	int syncStatus; 
	int latency; 
	
	bool broadcastingID; 
	vector <string> queuedMessages; 
	vector <string> programMessages; 
	ofColor currentColour; 
	
	ofTrueTypeFont* labelFont; 
	
	
};
