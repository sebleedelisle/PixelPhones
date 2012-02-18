/*
 *  StripesEffect.cpp
 *  PixelPhone04
 *
 *  Created by Seb Lee-Delisle on 23/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "StripesEffect.h"


void StripesEffect::update() {
	
	if(!started) return; 
	
	fbo.begin(); 
	frame ++; 
	if(frame>speed+1) {
		//ofColor temp = colour1; 
		//colour1 = colour2; 
		//colour2 = temp; 
		colour1.setHue(((int)colour1.getHue()+10)%255); 
		frame = 0; 
		//cout << "switched " << colour1.getHue() << " " << colour2.getHue() << "\n";
	}
	
	int offset = ((width+barWidth)/speed * frame) - barWidth; 
	
	
	
	ofSetColor(colour2); 
	ofFill(); 
	ofRect(0, 0, width, height);
	

	ofSetColor(colour1); 
	
	ofRect(offset, 0, barWidth, height);
	
	
	
	
	fbo.end(); 
	
	
	
	
}