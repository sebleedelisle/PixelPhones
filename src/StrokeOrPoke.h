/*
 *  StrokeOrPoke.h
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 04/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#pragma once
#include "OutputEffect.h"
#include "SOPData.h"
#include "ConnectedPhone.h"

class StrokeOrPoke : public OutputEffect {
	public:
	StrokeOrPoke() : OutputEffect(300,100) {
		drawFBOToPhones = false;
		currentPokeRating = 0; 
		currentStrokeRating = 0; 
		numPokes = 0; 
		numStrokes = 0; 
		topStrokerPos = 1; 
		topPokerPos = 1; 
		notifyTopStrokerFlag = false;
		notifyTopPokerFlag = false;
	}	
	
	virtual void draw(); 	
	virtual void start(); 
	virtual void stop(); 
	
	void notifyTopStroker(); 
	void notifyTopPoker(); 
	
	map<ConnectedPhone * , SOPData> sopData;
	map <int, ConnectedPhone> * connectedPhones;

	int numPokes; 
	int numStrokes; 
	float currentPokeRating; 
	float currentStrokeRating; 
	
	bool notifyTopStrokerFlag; 
	bool notifyTopPokerFlag; 
	
	int topStrokerPos; 
	int topPokerPos; 
	
	bool running; 
	
	
};
