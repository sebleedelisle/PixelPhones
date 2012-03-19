/*
 *  CommsManager.h
 *  PixelPhones02
 *
 *  Created by Seb Lee-Delisle on 31/07/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once 

#include "ofMain.h"
#include "ofxNetwork.h"
#include "WebSocketClient.h"
#include "ConnectedPhone.h"
#include "FoundPhone.h"

class CommsManager { 

	public : 
	CommsManager();
	
	void update(); 
	bool setup(int portNum); 
	void draw(int vidWidth, int vidHeight);
	
	void sendAllPhones(string msg); 
	void sendAllPhonesColour(ofColor col); 
	void startBroadcastingIDs(); 
	void stopBroadcastingIDs(ConnectedPhone * exceptThisOne = NULL); 
	void setFrameRate(int rate); 
	void setNumBits(int numbits); 
	void foundPhones(vector <FoundPhone *> phones, int vidWidth, int vidHeight); 
	
	void resetPhones(); 
	
	void mousePressed(int x, int y, int button); 
	void mouseReleased(int x, int y, int button); 

	
	ofxTCPServer TCP;
	map<int, ConnectedPhone *> connectedPhones;
	vector <ConnectedPhone *>  disconnectedPhones;
	bool broadcastingIDs; 
	bool syncing; 
	int numConcurrentSyncs; 
	int portNum; 
	int numBits; 
	int frameRate; 
	int posBrightness; 
	ofTrueTypeFont labelFont; 
	
	ofFbo labelFbo; 
};