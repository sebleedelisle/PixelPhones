/*
 *  CameraWrapper.h
 *  PixelPhones
 *
 *  Created by Seb Lee-Delisle on 16/03/2012.
 *  Copyright 2012 seb.ly. All rights reserved.
 *
 */

#pragma once

//#include "ofMain.h"

class CameraWrapper { 

	public : 
	
	virtual void setup(string _name, int width = 640, int height = 480, int framerate =30){};
	virtual bool update(){};  // returns true if frame is new
	
	virtual ofPixelsRef getPixelsRef(){};	
	virtual void draw(float x, float y){}; 
	virtual int getWidth(){};	
	virtual int getHeight(){};
	virtual bool videoSettings(){}; 
	virtual void close(){}; 
    
    int frameNum; 
   
	string name; 


};