/*
 *  PacmanEffect.h
 *  PixelPhone04
 *
 *  Created by Seb Lee-Delisle on 23/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once
#include "OutputEffect.h"

class PacmanEffect : public OutputEffect {

public:
	
	PacmanEffect(int w, int h) : OutputEffect(w, h) {}
	
	virtual void update(); 
	



};