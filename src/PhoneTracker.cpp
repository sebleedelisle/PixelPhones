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
    diffSkipFrames = 1; 
	
//#ifdef USE_LIBDC
//	libdcCamera.setBayerMode(DC1394_COLOR_FILTER_GBRG);
//	libdcCamera.setup();
//	//vidGrabber.close(); 
//	vidScale = 4; 
//	vidWidth = camwidth = 1024; 
//	vidHeight = camheight = 768; 
//	
//	
//#else
//	//vidGrabber.close(); 
//	vidScale = 2; 
//	camwidth = 640; 
//	camheight = 480; 
//	
//	vidGrabber.initGrabber(camwidth, camheight);
//	vidGrabber.setDesiredFrameRate(60);
//	
//	vidWidth = vidGrabber.getWidth(); 
//	vidHeight = vidGrabber.getHeight(); 
//#endif	
//	
	
	cameraManager.init(); 
	vidWidth = cameraManager.getWidth(); 
	vidHeight = cameraManager.getHeight(); 
	
	cvWidth = 320; 
	cvHeight = 240; 
	
	vidScale = (float)vidWidth/(float)cvWidth; 
	
	vidReset = false; 

	cvVideo.allocate(vidWidth, vidHeight); 
	cvColour.allocate(cvWidth, cvHeight); 
	cvGrey.allocate(cvWidth, cvHeight); 
	cvGreyPrevious.allocate(cvWidth, cvHeight); 
	cvDiff.allocate(cvWidth, cvHeight); 	
    cvDiffBlended.allocate(cvWidth, cvHeight); 
  	
	trackingDebugData.allocate(384, 1024, GL_RGB, 0);
	trackingDebugData.begin(); 
	ofClear(0, 0, 0, 0); 
	trackingDebugData.end(); 
	trackingDebugDataLine = 0; 
	
}


vector <FoundPhone *>  PhoneTracker::update(bool isBroadcasting){
	
	if ((!isBroadcasting) && (broadcasting)) {
		//clear everything... 
		spareTrackedBlobs.insert(spareTrackedBlobs.end(), trackedBlobs.begin(), trackedBlobs.end());
		trackedBlobs.clear(); 
		
	}
	broadcasting = isBroadcasting;
	
	vector <FoundPhone *> foundPhones; 
	
	if(maxBlobSize<minBlobSize) maxBlobSize = minBlobSize;
	
	if(updateDiff && !updateVideo) updateVideo = true; 
	
	if(updateVideo) {
		
		int minBlobArea = minBlobSize * minBlobSize ;
		int maxBlobArea = maxBlobSize * maxBlobSize ;
		
		bool newframe = false; 
		
		if(cameraManager.update()) {
			
            frameCounter++; 
            
			ofPixelsRef vidpix = cameraManager.getPixelsRef();
						
			cvVideo.setFromPixels(vidpix);	
			cvColour.scaleIntoMe(cvVideo);
			cvColour.mirror(flipY, flipX);			
			
			
			float timeNow = (float)ofGetElapsedTimeMillis(); 
			int milsbetweenframes = timeNow - lastVideoFrameMils;
			videoFrameRate+= ((( 1000.0f / (milsbetweenframes)) - videoFrameRate) * 0.01) ;
			
			lastVideoFrameMils = timeNow; 

			// gapNumFrames is the number of missing frames before we reset the colour reading. 
			// the phones will leave 6 blank frames in, so any more than four should be safe as a
			// trigger. 
			gapNumFrames = ((float)videoFrameRate / (float)phoneFrameRate)*5*doubleToSingleRatio;

			//unsigned char * vidPix = vidGrabber.getPixels();
			

			if(updateDiff) { 

				cvGreyPrevious = cvGrey; 
				cvGrey= cvColour;
				//cvGrey.mirror(true, true); 
				cvDiff = cvGrey; 
				
				// take the abs value of the difference between background and incoming and then threshold:
				cvDiff.absDiff(cvGreyPrevious, cvGrey);
				cvDiffBlended+=cvDiff; 
                
                                
				
				if(broadcasting) { 
					
					
                    if(frameCounter%diffSkipFrames==0) {
                    
                        cvDiffBlended.threshold(differenceThreshold);
                                                                    
                        //cout << trackedBlobs.size() << " "<< spareTrackedBlobs.size()<<"\n"; 
                        
                        
                        // find contours which are between the size of minBlobSize and maxBlobSize (in area).
                        // also, find holes is set to true so we will get interior contours as well....
                        contourFinder.findContours(cvDiffBlended, minBlobArea, maxBlobArea, 300, true); 	// find hole
                        
                        //cout << contourFinder.blobs.size() << "\n"; 
                        
                        // go through every found blob... 
                        for(int i = 0; i<contourFinder.nBlobs; i++) { 
                            
                            ofxCvBlob * cvBlob = &contourFinder.blobs[i];
                                
                            // and compare them to all the blobs we're tracking
                            
                            TrackedBlob * trackedBlobHit = NULL; 
                            
                            list<TrackedBlob*>::iterator it;
                            
                            for(it = trackedBlobs.begin(); it!=trackedBlobs.end(); it++) { 
                                
                                TrackedBlob * trackedBlob = *it; 
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
                                if(cvBlob->centroid.distance(trackedBlob->pixelPosition /vidScale) < trackDistance) {
                                //if(trackedBlob->boundingRect.inside(cvBlob->centroid)) {
                                    trackedBlobHit = trackedBlob; 
                                    trackedBlobHit->updatePosition(cvBlob, cvWidth, cvHeight, vidScale); 
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
                                }
                                trackedBlobs.push_back(trackedBlobHit);
                                trackedBlobHit->updatePosition(cvBlob, cvWidth, cvHeight, vidScale); 

                            }
                                        
                        }
                        
                        cvDiffBlended.set(0); 
                        
                    }
                        
					// now we update all the tracked blobs
					vector<ofPoint *> labelPoints;
					
					list<TrackedBlob*>::iterator it = trackedBlobs.begin(); 
					while(it != trackedBlobs.end()) { 
						
						TrackedBlob * tb = *it;
						
						if(!tb->enabled) continue; 
						
						int cx = (int)(tb->pixelPosition.x);
						int cy = (int)(tb->pixelPosition.y);
						
						// figure out the pixel index of the centre of the blob
						int index = (flipY ? (vidHeight - cy) : cy) *vidWidth;
						index += flipX ? (vidWidth-cx) : cx;
						index*=3; 
									 
						// and pull out the colour - we should probably average the colour - perhaps by 
                        // shrinking the source image? Or better to take a few pixels around the 
                        // centroid. 

						ofColor col;
						col.set(vidpix[index], vidpix[index+1], vidpix[index+2], 255);
						
						
                        // pass it into our tracked blob function.  
						tb->update(col, gapNumFrames, numBits, milsbetweenframes, showTrails, &labelPoints); 
						
						if(!tb->enabled) {
							// tracked Blob has disappeared - so we check it was a valid id. 
														
							it = trackedBlobs.erase(it); 
							trackedBlobsToCheck.push_back(tb); 

						} else {
							it++; 
							
						}
					}
					
				}
			}
		}
		
		
	}
	
	if(trackedBlobsToCheck.size()>0) { 
		
		TrackedBlob * tb = *trackedBlobsToCheck.begin(); 
		trackedBlobsToCheck.pop_front();
		
		float thresh = 0.25;// bwthreshold; 
		int id =-1; 
		while((thresh<1) && (id==-1)) {
			id = tb->getID(thresh);
			thresh+=0.25; 
		} 
		if(id>-1){
			//cout <<  "FOUND! : "<< id << " " << tb->centroid.x << " " << tb->centroid.y << "\n";
			foundPhones.push_back(new FoundPhone(id, tb->data)); 
			
		}; 
		
		if(tb->pixelCount>tb->lifeExpectancy*2) { 
			
			// draw the data picked up in the debug bitmap
			trackingDebugData.begin(); 
			if(id>-1) ofSetColor(0,255,0); 
			else ofSetColor(255, 0, 0);
			ofLine(0,(float)trackingDebugDataLine+0.5f,1,(float)trackingDebugDataLine+0.5f); 
			ofSetColor(0,0,255);
			ofLine(0,(float)trackingDebugDataLine+1.5f,1,(float)trackingDebugDataLine+1.5f); 
			
			ofSetColor(255); 
			tb->trackedPixels.update();
			tb->trackedPixels.draw(1,trackingDebugDataLine); 
			
			trackingDebugDataLine++;
			ofEnableSmoothing(); 
			ofSetLineWidth(1); 
			
			TrackedBlobData* data = tb->data; 
			float x = 1; 
			for(int i=0; i<data->pulseLengths.size();i++) {
				IDPulseData* pulse = (data->pulseLengths[i]);
				ofSetColor(pulse->isBlack() ? 0 : 255);
				ofLine(x, (float)trackingDebugDataLine+0.5f, x+pulse->length, (float)trackingDebugDataLine+0.5f);
				x+=pulse->length; 
			}
			ofDisableSmoothing(); 
			trackingDebugDataLine++; 
			trackingDebugData.end(); 
		}
		
		spareTrackedBlobs.push_back(tb); 
		
	}
	
	
	return foundPhones; 
	
	
	
}

void PhoneTracker :: draw() {
	
	// set the colour to white and then draw the video
	ofSetColor(255,255,255); 
	
	
	if(updateVideo) {
		ofPushMatrix(); 
		ofTranslate(ofGetWidth()/2,ofGetHeight()/2); 
		ofScale(flipX ? -1 :1, flipY ? -1 : 1); 

		cameraManager.draw(-vidWidth/2,-vidHeight/2);
        
		ofPopMatrix(); 
	}
	if(broadcasting) { 
		ofPushMatrix(); 
		ofTranslate((ofGetWidth()/2) - (vidWidth/2), (ofGetHeight()/2) - (vidHeight/2)); 
		// scale up dependent on video width vs the screen width
		//ofScale(1024.0/vidWidth, 768.0/vidHeight); 
	
		// draw all the blobs! 
		//for (int i = 0; i < contourFinder.nBlobs; i++){
//			contourFinder.blobs[i].draw(0,0);
//			
//		}
		//ofEnableAlphaBlending(); 
        ofEnableBlendMode(OF_BLENDMODE_ADD);

		list<TrackedBlob*>::iterator it; 
		for(it=trackedBlobs.begin(); it!=trackedBlobs.end(); it++) { 
			(*it)->draw(vidWidth, vidHeight, showTrails);
		
		}
		ofDisableBlendMode(); 
		ofPopMatrix();
		ofSetColor(255,255,255); 
	}
	
}
	
bool PhoneTracker ::doRectanglesIntersect(ofRectangle * rect1, ofRectangle * rect2){
	
	if(!(( rect1->x> rect2->x+rect2->width) || (rect2->x > rect1->x+rect1->width))) {
		if(!(( rect1->y> rect2->y+rect2->height) || (rect2->y > rect1->y+rect1->height))) {
			
			return true; 
		} 
	}
	return false; 
		
	
}

