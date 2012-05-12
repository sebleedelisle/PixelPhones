/*
 *  CameraManager.h
 *  PixelPhones
 *
 *  Created by Seb Lee-Delisle on 15/03/2012.
 *  Copyright 2012 seb.ly. All rights reserved.
 *
 */

#pragma once

#include "ofxLibdc.h"
#include "CameraWrapper.h"
#include "CameraVidGrabber.h"
#include "CameraFirewire.h"
#include "CameraVidPlayer.h"

class CameraManager { 
	public : 
	
	CameraManager(); 
	void init(); 
	bool update(); 
	void draw(float x, float y); 

	ofPixelsRef getPixelsRef();
	
	int getWidth(); 
	int getHeight(); 
	
	void videoSettings(); 
	
	void close(); 

	//ofImage* cameraImage; 
	//ofxLibdc::Camera cameraFirewire; 
	CameraWrapper* camera; 
	
	int gain; 
	int shutter; 
	
	//vector <CameraWrapper> cameras; 
	
	
};
