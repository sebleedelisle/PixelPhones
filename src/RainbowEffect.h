/*
 *  RainbowEffect.h
 *  PixelPhone04
 *
 *  Created by Seb Lee-Delisle on 26/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once 

#include "OutputEffect.h"


class RainbowEffect : public OutputEffect { 
	
	public: 
	
	RainbowEffect(int w, int h) : OutputEffect(w, h) {}
	

	virtual void update() { 
		
		if(!started) return; 
		
		fbo->begin(); 
		
		ofColor col; 
		
		for(int i = 0; i<width; i+=10) {
			col.setHsb((float)i/width*255, 255, 255); 
			
			ofSetColor(col); 
			ofFill();
			
			ofRect(i, 0, i+10, height); 
		}
		
		fbo->end(); 
		
		
	}





};