/*
 *  SOPDatum.h
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 04/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */
#pragma once
#include "ConnectedPhone.h"

class SOPDatum { 

	public : 
	
	SOPDatum() { 
		strokeCount = 0; 
		pokeCount = 0; 
		lastMessageTime = 0;
	}
	
	int strokeCount; 
	int pokeCount; 
	int lastMessageTime; 
	bool lastActionWasStroke; 

	ConnectedPhone * phone; 


}; 