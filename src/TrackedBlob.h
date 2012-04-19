/*
 *  TrackedBlob.h
 *  PixelPhonesV1
 *
 *  Created by Seb Lee-Delisle on 05/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */
#pragma once


#include "ofMain.h"
#include "IDPulseData.h"
#include "TrackedBlobData.h"
#include "ofxOpenCv.h"


class TrackedBlob {

	public : 
	TrackedBlob() ;
	void update(ofColor col, int gapNumFrames, int numbits, int milsBetweenFrames, bool updateTrails, vector<ofPoint *> * otherPoints = NULL); 
	void updatePosition(ofxCvBlob * cvBlob, int cvWidth, int cvHeight, float vidScale); 
	void reset() ; 
	void draw(int vidwidth, int vidheight, bool showTrails) ; 
	int getID(float threshold = 0.5); 
//	int decodeManchesterString(string * code);
//	bool vectorContainsInt(vector <int> * vec, int num);
//	void updateGreys();
	
	ofPoint labelCentre; 

	//int counter; 
	int lastUpdated; 
	int lifeExpectancy; 
	
	
	bool enabled; 
	
	//bool blackGaps; 
	//int numBits; 

	ofColor colour; 
	int pixelCount; 
//	vector<ofColor> colours; 
//	vector<float> greys; 
//	vector<float> times; 
	ofPoint	pixelPosition; 
//	ofRectangle boundingRect; 

	TrackedBlobData* data; 
	
	ofImage trackedPixels; 


};