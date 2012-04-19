/*
 *  OutputEffect.cpp
 *  PixelPhones
 *
 *  Created by Seb Lee-Delisle on 04/04/2012.
 *  Copyright 2012 seb.ly. All rights reserved.
 *
 */
#include "OutputEffect.h"

OutputEffect:: OutputEffect(int w, int h){
	
	frame = 0; 
	width = w; 
	height = h;
	fadeUp = fadeDown = 0; 
	drawFBOToPhones = true;
	
	started = false; 
}