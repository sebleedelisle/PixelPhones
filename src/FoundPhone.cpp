/*
 *  FoundPhone.cpp
 *  PixelPhones03
 *
 *  Created by Seb Lee-Delisle on 25/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */
#include "FoundPhone.h"


FoundPhone::FoundPhone(int phoneid, TrackedBlobData* data) {
	unitPosition.set(data->unitPosition);
	ID = phoneid; 
	//cout << "FoundPhone::FoundPhone " << ID << " " << unitPosition.x << " " << unitPosition.y << "\n";
	
	
}

