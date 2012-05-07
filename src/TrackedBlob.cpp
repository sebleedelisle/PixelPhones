/*
 *  TrackedBlob.cpp
 *  PixelPhonesV1
 *
 *  Created by Seb Lee-Delisle on 05/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "TrackedBlob.h"


TrackedBlob::TrackedBlob() {
	
	trackedPixels.allocate(256, 1, OF_IMAGE_COLOR_ALPHA);
	//cout << "trackedPixels allocated " << trackedPixels.width << "\n"; 
	
	lifeExpectancy = 10; 
	colour.setHsb(ofRandom(150, 180), ofRandom(255), 255, 255);
	data = new TrackedBlobData(); 
	reset(); 
	
}
void TrackedBlob::update(ofColor col, int gapNumFrames, int numbits, int milsBetweenFrames, bool updateTrails, vector<ofPoint *> * otherPoints) {
	if(!enabled) return; 
	lifeExpectancy = gapNumFrames; 
		data->numBits = numbits; 
	
	
	if(pixelCount-lastUpdated>lifeExpectancy) {
		// the phone has stopped flashing
        enabled = false; 
		
	} else if(pixelCount > (gapNumFrames * numbits * 2)) {
        // means that we have way too much data - invalid so destroy!
		enabled = false; 
		data->reset(); 
		
		
	} else {
		data->colours.push_back(col); 
		data->times.push_back(ofMap(milsBetweenFrames, 0, 30, 0, 1)); 
	}
	
	labelCentre.set(pixelPosition);
	
	if(pixelCount<trackedPixels.width) {
		trackedPixels.setColor(pixelCount, 0, col);
		
		if(updateTrails) trackedPixels.update(); 
		
		
	}
	pixelCount ++; 
	
}

void TrackedBlob::draw(ofRectangle* drawRect, bool showTrails) { 
	
    if(!enabled) return; 
	
	ofNoFill(); 
	ofSetLineWidth(1);
	ofSetColor(colour);
	
	ofPushMatrix(); 
 	ofScale(drawRect->width, drawRect->height); 
	ofRect(data->boundingRect); 
	ofPopMatrix(); 
    
    if(showTrails) {
        ofSetColor(255); 
        // TODO - we need to know the vidWidth and height! 
        trackedPixels.draw( (labelCentre.x *  (float)drawRect->width / (float)cvWidth)-pixelCount, (labelCentre.y * (float)drawRect->height/(float)cvHeight));
    }
 	
}

void TrackedBlob::updatePosition(ofxCvBlob * cvBlob, int cvwidth, int cvheight, float vidScale) { 
	
    cvWidth = cvwidth; 
    cvHeight = cvheight;
    
	// unitise all values... 
	pixelPosition.set(cvBlob->centroid);
	
	data->unitPosition.set(cvBlob->centroid); 
	data->unitPosition.x/=(float)cvWidth; 
	data->unitPosition.y/=(float)cvHeight; 
	data->boundingRect.set(cvBlob->boundingRect); 	
	data->boundingRect.x/=(float)cvWidth; 
	data->boundingRect.y/=(float)cvHeight; 
	data->boundingRect.width/=(float)cvWidth; 
	data->boundingRect.height/=(float)cvHeight; 
	//pixelPosition*=vidScale;
    //pixelPosition.x+=1; 
    //pixelPosition.y+=1; 
	
	lastUpdated = pixelCount;
	
	
}

void TrackedBlob::reset() {
	
	enabled = true; 
	//counter =0; 
	lastUpdated = 0; 
	pixelCount = 0; 
	
	//colours.clear(); 
//	greys.clear(); 
//	times.clear(); 
	
	//data = new TrackedBlobData(); 
	data->reset(); 	
	unsigned char * pixels = trackedPixels.getPixels();
	
	memset(pixels, 0, trackedPixels.width*4);
	trackedPixels.update(); 
	
}


int TrackedBlob::getID(float bwthreshold) {
	return data->getID(bwthreshold); 
	
}

