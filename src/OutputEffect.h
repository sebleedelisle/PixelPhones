/*
 *  OutputEffect.h
 *  outputSpike
 *
 *  Created by Seb Lee-Delisle on 22/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once 


#include "ofMain.h"
#include "FoundPhone.h"
#include "ConnectedPhone.h"

class OutputEffect { 

	public : 
	OutputEffect(int w, int h);
	
	virtual void update() {}; 
	virtual void draw(ofRectangle* drawRect) {};
	virtual void start() {
		started = true;
	}; 
	virtual void stop() {
		started = false; 
	}; 
	
	
	bool onlyUseFoundPhones; 
	ofFbo* fbo;	
	ofPixels pixels; 
	int frame; 
	int width; 
	int height; 
	float fadeUp; 
	float fadeDown; 
	bool drawFBOToPhones; 
	bool started; 
	
	vector <FoundPhone>* points; 
	map <int, ConnectedPhone *> * connectedPhones;
	
	

};