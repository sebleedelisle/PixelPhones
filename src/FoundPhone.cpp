/*
 *  FoundPhone.cpp
 *  PixelPhones03
 *
 *  Created by Seb Lee-Delisle on 25/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */
#include "FoundPhone.h"


FoundPhone::FoundPhone(int phoneid, float x, float y) {
	position.set(x, y);
	ID = phoneid; 
	cout << "FoundPhone::FoundPhone " << ID << " " << x << " " << y << " " << position.x << " " << position.y << "\n";
	
	
}

