//
//  PlasmaEffect.h
//  PixelPhones
//
//  Created by Seb Lee-Delisle on 21/04/2012.
//  Copyright (c) 2012 seb.ly. All rights reserved.
//

#pragma once
#include "OutputEffect.h"

class PlasmaEffect : public OutputEffect { 
    
public:
	PlasmaEffect(int w, int h) : OutputEffect(w, h) {
		
		time = 0; 
        speed = 1; 
        gen1 = 0.1; 
        gen2 = 0.1; 
        gen3 = 0.1; 
        gen4 = 0.1; 
        steps = 4; 
        hueOffset = 0; 
        hueRange = 30; 
        brightnessOffset = 255; 
        brightnessRange = 0; 
        saturation = 255; 
        
		
        
	}
	
	
	virtual void update(); 
	
    float time;
    float speed; 
    float gen1; 
    float gen2; 
    float gen3;
    float gen4;
    
    int steps; 
    float hueOffset; 
    float hueRange; 
    float brightnessOffset; 
    float brightnessRange; 
    float saturation; 
};