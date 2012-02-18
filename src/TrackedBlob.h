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

class TrackedBlob {

	public : 
	TrackedBlob() ;
	void update(ofColor col, int gapNumFrames, int numbits, vector<ofPoint *> * otherPoints = NULL) ; 
	void reset() ; 
	void draw(float bwthreshold) ; 
	int getID(float threshold = 0.5); 
	int decodeManchesterString(string * code);
	bool vectorContainsInt(vector <int> * vec, int num);
	void updateGreys();
	
	ofPoint	centroid; 
	ofPoint labelCentre; 
	ofRectangle boundingRect; 

	int counter; 
	int lastUpdated; 
	int lifeExpectancy; 
	
	
	bool enabled; 
	bool blackGaps; 
	int numBits; 

	ofColor colour; 
	vector<ofColor> colours; 
	vector<float> greys; 


};