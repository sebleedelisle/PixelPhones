/*
 *  PixelPhonesUtils.h
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 12/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once
#include "ofUtils.h"

int appStartTime = ofGetSystemTime(); 

int getRelativeSystemTime() { 
	return getRelativeSystemTime() - appStartTime; 
	
}