/*
 *  NyanCatch.h
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 09/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once

#include "OutputEffect.h"
#include "NyanCatchPhone.h"
#include "ConnectedPhone.h"


#define NYAN_STATE_WAITING 0
#define NYAN_STATE_PLAYING 1
#define NYAN_STATE_OVER 2


class NyanCatch : public OutputEffect {
	
	public:
	NyanCatch() : OutputEffect(320,240) {
		gameStartSwitch = false; 
		drawFBOToPhones = false; 
		speed = 100; 
		catImage = NULL;
	};
	void start(); 
	void stop(); 
	
	void update(); 
	void draw(ofRectangle* drawRect); 
	
	void startGame(); 
	void gameOver();
	
	int gameState; 
	int gameStartTime; 
	int gameEndTime; 
	
	bool gameStartSwitch; 

	
	vector <NyanCatchPhone *> nyanCatchPhones; 
	map <ConnectedPhone *, NyanCatchPhone *> nyanCatchPhonesByConnectedPhone; 
	
	ofSoundPlayer nyanSound; 
	
	int speed; 
	int catInterval; 
	//bool onlyUseFoundPhones;
	
	ofPoint catPos; 
	ofPoint catTargetPos; 
	ofPoint catVel; 
	
	ofImage * catImage;
	int lastUpdate; 
	

}; 
