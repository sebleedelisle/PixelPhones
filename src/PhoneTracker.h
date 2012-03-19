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
#include "CameraManager.h"
#include <vector>


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
	ofxCvColorImage cvVideo; 
	ofxCvGrayscaleImage cvGrey;
	ofxCvGrayscaleImage cvGreyPrevious; 
	ofxCvGrayscaleImage cvDiff; 
	ofxCvContourFinder 	contourFinder;
	
	CameraManager cameraManager; 
	
//#ifdef USE_LIBDC
//	ofxLibdc::Camera libdcCamera; 
//	ofImage libdcCameraImage; 
//#else
//	ofVideoGrabber vidGrabber; 
//#endif
	
	int differenceThreshold;
	int minBlobSize; 
	int maxBlobSize; 
	float trackDistance;
	
	int vidWidth; 
	int vidHeight; 
	int cvWidth; 
	int cvHeight;
	float vidScale; 
	
	
	float lastVideoFrameMils; 
	float videoFrameRate; 
	int phoneFrameRate; 
	int numBits; 
	float gapNumFrames; 
	
	list <TrackedBlob*> trackedBlobs; 
	vector <TrackedBlob*> spareTrackedBlobs; 
	deque <TrackedBlob*> trackedBlobsToCheck; 
	
	bool vidReset; 
	
	bool updateVideo; 
	bool updateDiff;
	bool broadcasting; 
	
	bool flipX; 
	bool flipY; 
	
	bool recording; 
	
	float bwthreshold; 
	
	ofFbo trackingDebugData; 
	int trackingDebugDataLine; 
	
	
	
	

	
};