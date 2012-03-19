/*
 *  ParticleWoosh.cpp
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 13/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "ParticleWoosh.h"

void ParticleWoosh::update() {
	
	if(!started) return; 
	
	while(particlePos.size() <= particleNum) { 
		
		ofVec2f pos; 
		pos.set(ofRandom(width), ofRandom(height)); 
		particlePos.push_back(pos);
		ofVec2f vel; 
		vel.set(speed,0); 
		vel.rotate(ofRandom(360));
		particleVels.push_back(vel); 
		ofColor col; 
		col.setHsb((particleColours.size()*5)%255, 255, 255);
		particleColours.push_back(col); 
		
	}
	
	while(particlePos.size()>particleNum) { 
		particlePos.pop_back(); 
		particleVels.pop_back(); 
		particleColours.pop_back(); 
		
	}
	
	fbo->begin(); 

	ofEnableAlphaBlending();
	ofSetColor(0,0,0, fade); 
	ofFill(); 
	ofRect(0, 0, width, height);
	ofDisableAlphaBlending();
	for(int i = 0; i<particleNum; i++) {
		
		particlePos[i]+= particleVels[i];
		
		ofVec2f * pos = &particlePos[i]; 
		ofVec2f * vel = &particleVels[i];
		
		if(currentSpeed!=speed) { 
			particleVels[i]*=(speed/currentSpeed); 
			
			
		}
		if(pos->x-size<0) {
			pos->x = size; 
			vel->x*=-1; 
			
		} else if (pos->x+size>width) {
			pos->x = width-size; 
			vel->x*=-1; 
			
		}
		if(pos->y-size<0) {
			pos->y = size; 
			vel->y*=-1; 
			
		} else if (pos->y+size>height) {
			pos->y = height-size; 
			vel->y*=-1; 
			
		}
		ofSetColor(particleColours[i]); 
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofCircle(pos->x, pos->y, size); 
		ofDisableBlendMode();
		
	}
	
	
	
	fbo->end(); 
	
	currentSpeed = speed; 
	
	
}