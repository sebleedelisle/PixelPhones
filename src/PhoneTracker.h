/*
 *  PhoneTracker.h
 *  PixelPhones03
 *
 *  Created by Seb Lee-Delisle on 06/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once
#include "ofxOpenCv.h"
#include "TrackedBlob.h"
#include "FoundPhone.h"

class PhoneTracker { 
	
	public : 
	
	void setupCamera(int width, int height); 
	vector <FoundPhone *> update(bool broadcastings); 
	void draw();
	
	bool doRectanglesIntersect(ofRectangle * rect1, ofRectangle * rect2); 
	
	//need to add vector of found phone ids
	
	
	// we don't actually use these
	// just checking to see if they
	// all work in the same place :)
	
	ofxCvColorImage cvColour; 
	ofxCvGrayscaleImage cvGrey;
	ofxCvGrayscaleImage cvGreyPrevious; 
	ofxCvGrayscaleImage cvDiff; 
	ofxCvContourFinder 	contourFinder;
	
	ofVideoGrabber vidGrabber; 
	
	int differenceThreshold;
	int minBlobSize; 
	int maxBlobSize; 
	float trackDistance;
	
	int vidWidth; 
	int vidHeight; 
	int vidScale; 
	
	
	float lastVideoFrameMils; 
	float videoFrameRate; 
	int phoneFrameRate; 
	int numBits; 
	float gapNumFrames; 
	
	vector <TrackedBlob*> trackedBlobs; 
	vector <TrackedBlob*> spareTrackedBlobs; 
	
	bool vidReset; 
	
	bool updateVideo; 
	bool updateDiff;
	bool broadcasting; 
	
	bool flipX; 
	bool flipY; 
	
	float bwthreshold; 
	
	
	

	
};