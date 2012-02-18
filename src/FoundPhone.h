/*
 *  FoundPhone.h
 *  PixelPhones03
 *
 *  Created by Seb Lee-Delisle on 25/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */
#pragma once
#include "ofMain.h"

class FoundPhone { 
	
	public : 
	
	FoundPhone(int id, float x, float y); 
	
	int ID; 
	ofVec2f	position;
	ofColor currentColour; 
		
	
	
};
