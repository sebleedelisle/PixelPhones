/*
 *  StripesEffect.h
 *  PixelPhone04
 *
 *  Created by Seb Lee-Delisle on 23/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once
#include "OutputEffect.h"

class StripesEffect : public OutputEffect { 

public:
	StripesEffect(int w, int h) : OutputEffect(w, h) {
		
		colour1.setHsb(0, 255, 255);
		colour2.setHsb(0, 0, 0);
		speed = 30; 
		fadeDown = 0.5;
		fadeUp = 0.25;
		barWidth = 40; 
		
		

	}
	
	
	virtual void update(); 
	
	ofColor colour1; 
	ofColor colour2; 
	
	int speed; 
	int barWidth; 

};