/*
 *  PacmanEffect.cpp
 *  PixelPhone04
 *
 *  Created by Seb Lee-Delisle on 23/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "PacmanEffect.h"


void PacmanEffect::update() {
	
	if(!started ) return; 
	fbo->begin(); 
	frame +=2; 
	float openness = (((frame%30)<15)? frame%30 : 30-(frame%30))*3;// abs(sinf(frame*0.15)) * 45; 
	
	ofPushMatrix(); 
	
	
	ofSetColor(0,0,200); 
	ofFill(); 
	ofRect(0, 0, width, height);
	
	
	
	ofVec2f outsidePoint; 
	outsidePoint.set(105,0); 
	
	ofTranslate(((frame*4) % (width+200) )- 100, (height/2) + (openness/4));//(abs(sinf(frame*0.1))*-20)+10);
	ofSetColor(0,0,0); 
	
	
	for(int i = 0; i< 360; i+=12){
		
		if((i< openness ) || (i>(360-openness))) {
			outsidePoint.rotate(12);
			continue;
		}
		ofBeginShape(); 
		ofVertex(-10, 0);
		ofVertex(outsidePoint.x, outsidePoint.y);
		outsidePoint.rotate(12);
		ofVertex(outsidePoint.x, outsidePoint.y);
		ofEndShape(true);
		
	}
	
	ofPopMatrix(); 
	
	outsidePoint.set(100,0); 
	
	ofTranslate(((frame*4) % (width+200) )- 100, (height/2) + (openness/4));//(abs(sinf(frame*0.1))*-20)+10);
	ofSetColor(255,220,0); 
	
	for(int i = 0; i< 360; i+=12){
		
		if((i< openness ) || (i>(360-openness))) {
			outsidePoint.rotate(12);
			continue;
		}
		ofBeginShape(); 
		ofVertex(-20, 0);
		ofVertex(outsidePoint.x, outsidePoint.y);
		outsidePoint.rotate(12);
		ofVertex(outsidePoint.x, outsidePoint.y);
		ofEndShape(true);
		
	}
	
	
	ofSetColor(0,0,0); 
	ofEllipse(-20, -46, 40, 45);
	ofPopMatrix(); 
	
	
	fbo->end(); 
	
	
	
	
}