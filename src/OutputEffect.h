/*
 *  OutputEffect.h
 *  outputSpike
 *
 *  Created by Seb Lee-Delisle on 22/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once 
#include "ofMain.h"; 
#include "FoundPhone.h";
#include "ConnectedPhone.h";

class OutputEffect { 

	public : 
	OutputEffect(int w, int h) {
		
		//fbo.allocate(w, h, GL_RGB, 0);
		
		frame = 0; 
		width = w; 
		height = h;
		fadeUp = fadeDown = 0; 
		drawFBOToPhones = true;
		//fbo.begin();
		//ofClear(0, 0, 0, 0);
		//fbo.end();
				
		started = false; 
	}
	
	virtual void update() {}; 
	virtual void draw() {};
	virtual void start() {
		started = true;
	}; 
	virtual void stop() {
		started = false; 
	}; 
	
	
	
	ofFbo*		fbo;	
	ofPixels	pixels ; 
	int			frame; 
	int			width; 
	int			height; 
	float		fadeUp; 
	float		fadeDown; 
	bool		drawFBOToPhones; 
	bool		started; 
	
	vector <FoundPhone>* points; 
	map <int, ConnectedPhone *> * connectedPhones;
	
	

};