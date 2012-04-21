//
//  PlasmaEffect.cpp
//  PixelPhones
//
//  Created by Seb Lee-Delisle on 21/04/2012.
//  Copyright (c) 2012 seb.ly. All rights reserved.
//

#include "PlasmaEffect.h"



void PlasmaEffect::update() {
	
	if(!started) return; 
	//fbo->
	fbo->begin(); 
    
    //ofPixels = fbo->getPix
	 
    time += speed;///10.0; 
    float w = fbo->getWidth(); 
    float h = fbo->getHeight(); 
    ofColor col; 
    
    ofFill(); 
    for(float x = 0; x<w; x+=4) { 
        for(float y = 0; y<h; y+=4) { 
            
            
            float v = sin(ofDist(x + time*gen1, y, 128.0, 128.0) / gen3)
            + sin(ofDist(x, y, 0.0, 0.0) / gen3)
            + sin(ofDist(x, y  , 192.0, 64) / gen4)
            + sin(ofDist(x, y+ time*gen2, 192.0, 64) / gen4);

            v= (v/8.0) + 0.5; // (now a range between 0 and 1)
            v = round(steps*v) / steps;
            
            col.setBrightness(ofClamp(brightnessOffset + v*brightnessRange, 0, 255));
            col.setHue(fmodf(hueOffset + (v * hueRange), 255.0));
            col.setSaturation(saturation);
            ofSetColor(col); 
            
            
//            int r = 127 + 127 * sin(1*(ofDist(x,y,0,0)+time)); 
//            int g = 127 + 127 * sin(3*(x+y)+time); 
//            int b = 127 + 127 * sin(5*(x+y)+time); 
//            ofSetColor(r,g,b);                         
            
            ofRect(x,y,4,4); 
            
        }
    }
    
    //colour = int((4 + v)) * 32;
    
    
	fbo->end(); 
	
	
	
	
}

