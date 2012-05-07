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
    
    PhoneTracker();
	
	void setupCamera(int width, int height); 
	vector <FoundPhone *> update(bool broadcastings); 
	void draw(ofRectangle * drawRect);
	
	bool doRectanglesIntersect(ofRectangle * rect1, ofRectangle * rect2); 
	
	ofxCvColorImage cvColour; 
	ofxCvColorImage cvVideo; 
    //ofxCvShortImage cvThreshold; 
	ofxCvGrayscaleImage cvGrey;
	ofxCvGrayscaleImage cvGreyPrevious; 
	ofxCvGrayscaleImage cvDiff; 
    ofxCvGrayscaleImage cvDiffBlended; 
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
    float smoothVideoFrameRate; 
	int phoneFrameRate; 
	int numBits; 
	float gapNumFrames; 

	
	list <TrackedBlob*> trackedBlobs; 
	vector <TrackedBlob*> spareTrackedBlobs; 
	deque <TrackedBlob*> trackedBlobsToCheck; 
	
	bool vidReset; 
	
	bool updateVideo; 
	bool updateDiff;
    int diffSkipFrames; 
    float doubleToSingleRatio;
    int frameCounter;
	bool broadcasting; 
    bool calibrating; 
	bool showTrails;
    
	bool flipX; 
	bool flipY; 
	
    string recordingFolder; 
	bool recording; 
	
	float bwthreshold; 
	
	ofFbo trackingDebugData; 
	int trackingDebugDataLine; 
	
	
	
	

	
};