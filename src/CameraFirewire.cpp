/*
 *  CameraFirewire.cpp
 *  PixelPhones
 *
 *  Created by Seb Lee-Delisle on 16/03/2012.
 *  Copyright 2012 seb.ly. All rights reserved.
 *
 */

#include "CameraFirewire.h"

void CameraFirewire::setup(string _name, int width, int height, int framerate){
	
    //camera.setBayerMode(DC1394_COLOR_FILTER_GBRG);
//    camera.set1394b(true); 

    
	// setup() will pick the first camera. Or if there are multiple cameras you
	// can use the number, or the GUID of the camera.
    camera.setup();
    camera.setBayerMode(DC1394_COLOR_FILTER_GBRG);
    
    //camera.setFeatureAbs(<#dc1394feature_t feature#>, <#float value#>)tBayerMode(DC1394_COLOR_FILTER_GBRG);
    camera.setGainRaw(10); 
    camera.setShutterRaw(130); 
      name = _name; 
    frameNum = 0; 
	
}
bool CameraFirewire::update() {
	if(camera.grabVideo(curFrame)) {
		curFrame.update();
        
        cout << camera.getFrameRate() << " " << 
                camera.getGainRaw() << " " << 
                camera.getShutterRaw() << " " << 
                camera.getBrightnessRaw() << " " << 
                camera.getExposureRaw() << " " << 
                camera.getGammaRaw() << "\n"; 
                
        return true;
	}	
    return false; 
}

void CameraFirewire::draw(float x, float y) {
	curFrame.draw(x, y); 
}
ofPixelsRef CameraFirewire::getPixelsRef(){
	return curFrame.getPixelsRef();
    
}
int CameraFirewire::getWidth(){
	return camera.getWidth(); 
    
}
int CameraFirewire::getHeight(){
	return camera.getHeight(); 
    
}
bool CameraFirewire::videoSettings(){
	//camera.videoSettings(); 
	
}
void CameraFirewire::close() { 
	
	
	
}
