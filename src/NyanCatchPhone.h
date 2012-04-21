/*
 *  NyanCatchPhone.h
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 09/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

class NyanCatchPhone {

	public : 
	NyanCatchPhone() { 
		reset();
        screenPos.set(0,0,0); 
	}
	
	void reset() { 
		displayCatTime = -1; 
		touchTime = -1; 
		catCaught = false; 
		startMessageSent = false;
		touchTime = 10000000;
       
	}
	
	
	
	int displayCatTime; 
	bool startMessageSent; 
	int touchTime; 
	bool catCaught; 
    ofPoint screenPos; 
	
	
	ConnectedPhone * connectedPhone; 


};