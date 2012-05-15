/*
 *  ConnectedPhone.h
 *  PixelPhones02
 *
 *  Created by Seb Lee-Delisle on 23/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */
#pragma once
#include "WebSocketServer.h"
#include "ofMain.h"
#include <string.h>
#include <ctime>

#define NOT_SYNCED -1
#define SYNC_IN_PROGRESS 0
#define SYNCED 1

class ConnectedPhone  {
	
	public : 
	ConnectedPhone(ofFbo * labelFbo);
	void setup(int id);
	void update(); 
	void draw(int brightness, int vidWidth, int vidHeight); 
	
	void sendColour(ofColor col, int latency = 0, float fadeUp = 0, float fadeDown = 0) ;
	void sendFrameRate(int framerate, float doubleToSingleRatio, int blackTimeOffset); 
	void sendNumBits(int numbits); 
    void updateWarpedPosition(ofMatrix4x4 warpMatrix);
	
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
	
	
	WebSocketServer * tcp; 
	
	void reset(int id); // test for trying different ids

	bool found; 
	
	bool connectionReady; 
	
	
	// the unit position of the phone relative to the camera view
	ofPoint unitPosition;
	// the unit position after it's been warped to fix the perspective 
    // in the room
    ofPoint warpedPosition; 
	int ID; 
    int calibrationID; 
    // true if the phone is currently flashing. 
	bool isBroadcasting; 
	
	int counter; 
	
	int syncStatus; 
	int latency; 
	
	vector <string> queuedMessages; 
	vector <string> programMessages; 
	ofColor currentColour; 
	
	ofTrueTypeFont* labelFont; 
	ofImage labelImage; 
	
	ofFbo* labelFbo; 
	
	
	
};
