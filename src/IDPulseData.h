/*
 *  IDColourData.h
 *  PixelPhone04
 *
 *  Created by Seb Lee-Delisle on 24/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once 

class IDPulseData { 

	public : 
	
	IDPulseData() { 
		bit = 0; 
		length = 0; 
		bitCount =  0; 
	//	nextSameColour = NULL; 
	}
	
	int getBitCount(float shortest, float longest, float threshold) { 
		// fix for if there's no variation. 
		if(longest-shortest<=0) longest=shortest+0.1; 
		
		float actualthreshold = ((longest-shortest) * threshold) + shortest; 
		return (length>=actualthreshold) ? 2 : 1; 
		
		
	}
	
	
	/*int getBitCount(){
		if (bitCount == 0) {
			
			if(nextSameColour == NULL) {
				bitCount = (ratioToPrevious>1.5) ? 2 : 1; 
			} else {
				
				int nextBitCount = nextSameColour->getBitCount(); 
				
				bitCount = nextBitCount;
				
				if((nextBitCount==1) && (nextSameColour->ratioToPrevious < 0.75)) bitCount=2;
				else if((nextBitCount==2) && (nextSameColour->ratioToPrevious > 1.5)) bitCount=1; 
				
			}

		} 
		
		return bitCount; 
	}*/

	bool isBlack() { return bit==0; }
	
	int bit;				//0 for black, true for white? 
	float length;			// length of the pulse in frames 
	float ratioToPrevious;	// length of previous over this length
	int bitCount;			// whether this is a single or double
	//IDPulseData * nextSameColour; // linked list of same colours; 

};