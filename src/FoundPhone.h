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
#include "TrackedBlobData.h"

class FoundPhone { 
	
	public : 
	
	FoundPhone(int id, TrackedBlobData* data); 
	
	int ID; 
	ofVec2f	unitPosition;
	ofColor currentColour; 
	
	
};
