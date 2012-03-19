/*
 *  CameraVidGrabber.cpp
 *  PixelPhones
 *
 *  Created by Seb Lee-Delisle on 16/03/2012.
 *  Copyright 2012 seb.ly. All rights reserved.
 *
 */

#include "CameraVidGrabber.h"

void CameraVidGrabber::setup(string _name, int width, int height, int framerate){
	vidGrabber.initGrabber(width, height);
	vidGrabber.setDesiredFrameRate(framerate); 
	name = _name; 
	
}
bool CameraVidGrabber::update() {
	vidGrabber.update(); 
	return vidGrabber.isFrameNew(); 
	
}

void CameraVidGrabber::draw(float x, float y) {
	vidGrabber.draw(x, y); 
}
ofPixelsRef CameraVidGrabber::getPixelsRef(){
	return vidGrabber.getPixelsRef();

}
int CameraVidGrabber::getWidth(){
	return vidGrabber.getWidth(); 

}
int CameraVidGrabber::getHeight(){
	return vidGrabber.getHeight(); 

}
bool CameraVidGrabber::videoSettings(){
	vidGrabber.videoSettings(); 
	
}
void CameraVidGrabber::close() { 
	vidGrabber.close(); 
	
	
}
