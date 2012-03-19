/*
 *  TrackedBlobData.h
 *  PixelPhones
 *
 *  Created by Seb Lee-Delisle on 14/03/2012.
 *  Copyright 2012 seb.ly. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "IDPulseData.h"

#define BLACKGAPS true 
#define USECHECKSUM

class TrackedBlobData { 
	
	public: 
		
	//ofColor colour; 
	vector<ofColor> colours; 
	vector<float> greys; 
	vector<float> times; 
	vector <IDPulseData *> pulseLengths; 
	
	
	ofPoint	unitPosition; 
	ofRectangle boundingRect; 
	int numBits; 
	
	int getID(float threshold = 0.5); 
	int decodeManchesterString(string * code);
	//bool vectorContainsInt(vector <int> * vec, int num);
	void updateGreys();
	void reset(); 
	

};