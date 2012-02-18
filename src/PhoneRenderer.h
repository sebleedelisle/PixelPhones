/*
 *  PhoneRenderer.h
 *  PixelPhone04
 *
 *  Created by Seb Lee-Delisle on 23/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */


#pragma once
#include "CommsManager.h"
#include "PacmanEffect.h"
#include "StripesEffect.h"
#include "RainbowEffect.h"
#include "OutputEffect.h"
#include "StrokeOrPoke.h"
#include "NyanCatch.h"
#include "ofxSimpleGuiToo.h"
#include "ParticleWoosh.h"

class PhoneRenderer { 

	public : 
	
	PhoneRenderer() ;
	void setup(CommsManager * cm); 
	
	void update(); 
	void draw(); 
	void updatePhonesWithFBO( ofFbo * fbo) ;
	
	void changeProgram(int programnum); 
	
	void initGui(ofxSimpleGuiToo * gui); 
	
	int frameRate; 
	int lastUpdateTime; 
	int elapsedFrames; 
	
	int latency; 
	
	ofPixels pixels; // temp object for storing fbo pixel data in 
	
	CommsManager * commsManager; 

	OutputEffect * currentProgram; 
	int currentProgramIndex; 
	vector <OutputEffect *> outputPrograms;
	PacmanEffect * pacmanEffect; 
	StripesEffect * stripesEffect; 
	//StrokeOrPoke * strokeOrPoke; 
	NyanCatch * nyanCatch; 
	ParticleWoosh * particleWoosh; 
	
	bool startProgramSwitch;
	//bool programRunning; 
	
	ofFbo 	currentProgramFbo;
	

};