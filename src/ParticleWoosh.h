/*
 *  ParticleWoosh.h
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 13/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once
#include "OutputEffect.h"

class ParticleWoosh : public OutputEffect { 
	
public:
	ParticleWoosh(int w, int h) : OutputEffect(w, h) {
		size = 20; 
		speed = 3; 
		particleNum = 4; 
		
		
	}
	
	
	virtual void update(); 
	
	
	vector <ofVec2f> particlePos; 
	vector <ofVec2f> particleVels; 
	vector <ofColor> particleColours;
	float speed; 
	float currentSpeed; 
	float particleSize; 
	int particleNum;
	float size; 
	float fade;
	
};