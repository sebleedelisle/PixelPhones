//
//  CameraVidPlayer.cpp
//  PixelPhones
//
//  Created by Seb Lee-Delisle on 01/05/2012.
//  Copyright (c) 2012 seb.ly. All rights reserved.
//

#include "CameraVidPlayer.h"

void CameraVidPlayer::setup(string _name, int width, int height, int framerate){
    
    
	//vidPlayer.initGrabber(width, height);
	//vidPlayer.setDesiredFrameRate(framerate); 
	
    vidPlayer.loadMovie("/Users/seb/Desktop/BTPlay/BTPlayFootage.mov");
    
    name = _name; 
    frameNum = 0; 
    vidPlayer.play();
    
}
bool CameraVidPlayer::update() {
	vidPlayer.update(); 
    bool newFrame = vidPlayer.isFrameNew();
    
   	return newFrame; 
	
}

void CameraVidPlayer::draw(float x, float y) {
	vidPlayer.draw(x, y); 
}
ofPixelsRef CameraVidPlayer::getPixelsRef(){
	return vidPlayer.getPixelsRef();
    
}
int CameraVidPlayer::getWidth(){
	return vidPlayer.getWidth(); 
    
}
int CameraVidPlayer::getHeight(){
	return vidPlayer.getHeight(); 
    
}
bool CameraVidPlayer::videoSettings(){
	//vidPlayer.videoSettings(); 
    vidPlayer.setPosition(0); 
	
}
void CameraVidPlayer::close() { 
	vidPlayer.close(); 
	
	
}
