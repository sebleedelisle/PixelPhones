/*
 *  PhoneTracker.cpp
 *  PixelPhones03
 *
 *  Created by Seb Lee-Delisle on 06/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "PhoneTracker.h"

void PhoneTracker::setupCamera(int camwidth, int camheight){
	
	lastVideoFrameMils = (float)ofGetElapsedTimeMillis(); 
	phoneFrameRate = 7; 
	numBits = 4;
	trackDistance = 4; 
	
	vidGrabber.close(); 
	
	vidGrabber.setDesiredFrameRate(60);
	vidGrabber.initGrabber(camwidth, camwidth);
	
	vidWidth = vidGrabber.getWidth(); 
	vidHeight = vidGrabber.getHeight(); 
	
	cout << vidWidth << " " << vidHeight << "\n bwthreshold : " << bwthreshold << "\n";
	
	vidReset = false; 

	cvColour.allocate(vidWidth,vidHeight); 
	cvGrey.allocate(vidWidth,vidHeight); 
	cvGreyPrevious.allocate(vidWidth,vidHeight); 
	cvDiff.allocate(vidWidth,vidHeight); 
	
}


vector <FoundPhone *>  PhoneTracker::update(bool isBroadcasting){
	
	broadcasting = isBroadcasting; 
	vector <FoundPhone *> foundPhones; 
	
	if(updateDiff && !updateVideo) updateVideo = true; 
	
	if(updateVideo) {
		
		vidGrabber.update();
	
		if(maxBlobSize<minBlobSize) maxBlobSize = minBlobSize;
		int minBlobArea = minBlobSize * minBlobSize ;
		int maxBlobArea = maxBlobSize * maxBlobSize ;
		
		
		if(vidGrabber.isFrameNew()) {
			
			
			float timeNow = (float)ofGetElapsedTimeMillis(); 
			videoFrameRate+= ((( 1000.0f / (timeNow - lastVideoFrameMils)) - videoFrameRate) * 0.01) ;
			
			lastVideoFrameMils = timeNow; 

			// gapNumFrames is the number of missing frames before we reset the colour reading. 
			// the phones will leave 6 blank frames in, so any more than four should be safe as a
			// trigger. 
			gapNumFrames = ((float)videoFrameRate / (float)phoneFrameRate)*5;

			unsigned char * vidPix = vidGrabber.getPixels();
			
			//ofPixels pix = new ofPixels
			cvColour.setFromPixels(vidPix, vidWidth, vidHeight);	
			cvColour.mirror(flipY, flipX);
			

			if(updateDiff) { 

				cvGreyPrevious = cvGrey; 
				cvGrey= cvColour;
				//cvGrey.mirror(true, true); 
				cvDiff = cvGrey; 
				
				// take the abs value of the difference between background and incoming and then threshold:
				cvDiff.absDiff(cvGreyPrevious, cvGrey);
				cvDiff.threshold(differenceThreshold);
			
				
				if(broadcasting) { 
					//cout << trackedBlobs.size() << " "<< spareTrackedBlobs.size()<<"\n"; 
					
					
					// find contours which are between the size of minBlobSize and maxBlobSize (in area).
					// also, find holes is set to true so we will get interior contours as well....
					contourFinder.findContours(cvDiff, minBlobArea, maxBlobArea, 500, true); 	// find hole
					
					//cout << contourFinder.blobs.size() << "\n"; 
					
					// go through every found blob... 
					for(int i = 0; i<contourFinder.nBlobs; i++) { 
						
						ofxCvBlob * cvBlob = &contourFinder.blobs[i];
						
						// and compare them to all the blobs we're tracking
						
						TrackedBlob * trackedBlobHit = NULL; 
						
						for(int j=0; j<trackedBlobs.size(); j++) { 
							
							TrackedBlob * trackedBlob = trackedBlobs[j]; 
							// ignore old tracked blobs
							
							if(!trackedBlob->enabled) continue; 
							
							
							// if the trackedblob is inside the bounding rectangle of the 
							// found blob then update the trackedBlob info with this blob info
							
							// NOTE : all tracked blobs that intersect with this cvBlob
							// will be updated. If there are multiple overlapping cvBlobs this 
							// may get weird.
							
							// IMPROVEMENT : if we're thinking about having pretty small phones
							// we may want to check distance between the cvBlob and the trackedBlob
							// as they may not overlap. 
							
							//if(doRectanglesIntersect(&(cvBlob->boundingRect), &(trackedBlob->boundingRect))){
							if(cvBlob->centroid.distance(trackedBlob->centroid) < trackDistance) {
							//if(trackedBlob->boundingRect.inside(cvBlob->centroid)) {
								trackedBlobHit = trackedBlob; 
								
								// could probably put this in a method of the trackedBlob. 
								trackedBlobHit->centroid = cvBlob->centroid; 
								trackedBlobHit->boundingRect = cvBlob->boundingRect; 				
								trackedBlobHit->lastUpdated = trackedBlobHit->counter; 
								
							}
						}
						
						// if none of the currently tracked blobs match up, then make a new one
						if(trackedBlobHit == NULL) { 
							if(spareTrackedBlobs.size()>0) {
								// primitive object pooling 
								trackedBlobHit = spareTrackedBlobs.back();
								spareTrackedBlobs.pop_back(); 
								trackedBlobHit->reset(); 
								
							} else {
								trackedBlobHit = new TrackedBlob(); 
								trackedBlobs.push_back(trackedBlobHit);
							}
							
							// could probably put this in a method of the trackedBlob. 
							trackedBlobHit->centroid = cvBlob->centroid; 
							trackedBlobHit->boundingRect = cvBlob->boundingRect; 				
							trackedBlobHit->lastUpdated = trackedBlobHit->counter; 
						}
									
					}
					
					// now we update all the tracked blobs
					vector<ofPoint *> labelPoints;
					//vidPix = cvGreyPrevious.getPixels(); 
					for(int i=0; i<trackedBlobs.size(); i++) { 
						
						TrackedBlob * tb = trackedBlobs[i];
						if(!tb->enabled) continue; 
						
						// figure out the pixel index of the centre of the blob
						int index = flipY ? (vidHeight - (int)tb->centroid.y) : (int)tb->centroid.y *vidWidth;
						index += flipX ? (vidWidth-(int)tb->centroid.x) : (int)tb->centroid.x;
						index*=3; 
									 
						
						
						
						// and pull out the colour
						//ofColor col;
						//col.set(vidPix[index], vidPix[index+1], vidPix[index+2], 255);
						//col.set(vidPix[index], vidPix[index], vidPix[index], 255);
						
						int col = vidPix[index] + vidPix[index+1] + vidPix[index+2];
						
						
						
						// pass it into our tracked blob function. 10 - numbits 
						tb->update(col, gapNumFrames, numBits, &labelPoints); 
						
						if(!tb->enabled) {
							// tracked Blob has disappeared - so we check it was a valid id. 
							int id = tb->getID(bwthreshold);
							if(id>-1){
								cout <<  "FOUND! : "<< id << " " << tb->centroid.x << " " << tb->centroid.y << "\n";
								foundPhones.push_back(new FoundPhone(id, tb->centroid.x, tb->centroid.y)); 
								
							}; 
							spareTrackedBlobs.push_back(tb); 
						}
						else {
							
							
						}
						
					}
					
				}
			}
		}
	}
	return foundPhones; 
	
	
	
}

void PhoneTracker :: draw() {
	
	// set the colour to white and then draw the video
	ofSetColor(255,255,255); 
	if(updateVideo) cvColour.draw(0,0, 1024,768); 
	
	if(broadcasting) { 
		ofPushMatrix(); 
		// scale up dependent on video width vs the screen width
		ofScale(1024.0/vidWidth, 768.0/vidHeight); 
	
		// draw all the blobs! 
		//for (int i = 0; i < contourFinder.nBlobs; i++){
//			contourFinder.blobs[i].draw(0,0);
//			
//		}
	
		for(int i=0; i<trackedBlobs.size(); i++) { 
			trackedBlobs[i]->draw(bwthreshold);
		
		}
	
		ofPopMatrix();
		ofSetColor(255,255,255); 
	}
	//ofFill(); 
	//ofDrawBitmapString(ofToString(gapNumFrames), ofPoint(500,10));
	
}
	
bool PhoneTracker ::doRectanglesIntersect(ofRectangle * rect1, ofRectangle * rect2){
	
	if(!(( rect1->x> rect2->x+rect2->width) || (rect2->x > rect1->x+rect1->width))) {
		if(!(( rect1->y> rect2->y+rect2->height) || (rect2->y > rect1->y+rect1->height))) {
			
			return true; 
		} 
	}
	return false; 
		
	
}
