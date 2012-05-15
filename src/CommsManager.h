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
#include "WebSocketClient.h"
#include "WebSocketServer.h"
#include "ConnectedPhone.h"
#include "FoundPhone.h"

class CommsManager { 

	public : 
	CommsManager();
	
	void update(); 
	bool setup(int portNum); 
	void draw(ofRectangle* drawRect);
	
	void sendAllPhones(string msg); 
	void sendAllPhonesColour(ofColor col); 
	void startBroadcastingIDs(); 
	void stopBroadcastingIDs(ConnectedPhone * exceptThisOne = NULL); 
	void setFrameRate(int rate, float doubleToSingleRatio, int blackTimeOffset); 
	void setNumBits(int numbits); 
	void foundPhones(vector <FoundPhone *> phones, int vidWidth, int vidHeight); 
	
	void resetPhones(); 
	void updateWarpPoints( ofPoint points[4], int w, int h); 
    
    void startCalibrating(); 
    void stopCalibrating(); 
    
	
	void mousePressed(int x, int y, int button); 
	void mouseReleased(int x, int y, int button); 

	
	WebSocketServer server;
	
	map<int, ConnectedPhone *> connectedPhones;
	vector <ConnectedPhone *>  disconnectedPhones;
	bool broadcastingIDs; 
	bool syncing; 
    bool showPositions; 
	int numConcurrentSyncs; 
	int portNum; 
	int numBits; 
	int phoneFrameRate; 
    float doubleToSingleRatio; 
    int blackTimeOffset; 
    bool calibrating; 
    int calibrationCount; 
    int calibrationStart; 
    
	int posBrightness; 
	ofTrueTypeFont labelFont; 
	
	ofFbo labelFbo; 

	ofPoint			sourcePoints[4];
	ofPoint			unitSourcePoints[4];
	ofPoint			destPoints[4];
	ofMatrix4x4     warpMatrix; 
	



};