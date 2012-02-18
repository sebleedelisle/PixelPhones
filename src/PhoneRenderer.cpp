/*
 *  PhoneRenderer.cpp
 *  PixelPhone04
 *
 *  Created by Seb Lee-Delisle on 23/08/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "PhoneRenderer.h"


PhoneRenderer:: PhoneRenderer() { 
	
	frameRate = 30; 
	
	outputPrograms.push_back(new RainbowEffect(320,240)); 
	//
	outputPrograms.push_back(stripesEffect = new StripesEffect(320,240)); 
	outputPrograms.push_back(pacmanEffect = new PacmanEffect(320,240)); 
	outputPrograms.push_back(nyanCatch = new NyanCatch()); 
	outputPrograms.push_back(particleWoosh = new ParticleWoosh(320,240)); 
	
	

	currentProgramIndex = 0; 
	currentProgram = NULL; 
	//currentProgram = outputEffects[currentProgramIndex]; 
	
	latency = 0; 
	
	
}


void PhoneRenderer::setup(CommsManager * cm) { 
	
	commsManager = cm; 
	lastUpdateTime = 0; 

	
}

void PhoneRenderer::update() { 
	
	if(outputPrograms[currentProgramIndex]!= currentProgram) {
		
		changeProgram(currentProgramIndex);
		
	}
	
	if(currentProgram == NULL) return; 
	
	if(startProgramSwitch && !currentProgram->started) {
			
		currentProgram->start(); 
		startProgramSwitch = true;
	} else if((!startProgramSwitch) && (currentProgram->started)) { 
		currentProgram->stop(); 
		startProgramSwitch = false;
	}
	//cout << "PR:update " << startProgramSwitch << " " << currentProgram->started << "\n";
	
	// = currentProgram->started; 
	
	
	currentProgram->connectedPhones = &(commsManager->connectedPhones);
	
	if(!currentProgram->started) return; 
	
	
	int time = ofGetElapsedTimeMillis();
	int elapsedTime = time - lastUpdateTime ; 
	
//	cout << "update! "<< elapsedTime << " " << (1000/frameRate) << "\n";
	
	if((frameRate>0) && (elapsedTime>(1000/frameRate))) { 
		// ready for update!
	
		currentProgram->update(); 
		
		if(currentProgram->drawFBOToPhones) updatePhonesWithFBO(&(currentProgram->fbo)); 
		
		lastUpdateTime = time; 
	
	}
	
}


void PhoneRenderer :: updatePhonesWithFBO( ofFbo * fbo) { 
	
	
	fbo->readToPixels(pixels);
	

	map <int,ConnectedPhone *> * phones = &(commsManager->connectedPhones); 
	
	ofPushMatrix();
	ofTranslate(0, fbo->getHeight());
		
	
	map<int,ConnectedPhone *>::iterator phoneit = phones->begin();
	
	while(phoneit!=phones->end()) {
		
		ConnectedPhone * phone = phoneit->second; 
		
	
	//for(int i = 0; i<phones->size(); i++) {
//		
//		ConnectedPhone * phone = &(phones->at(i)); 
		
		if(true) {//phone->found) {
			
			//SHOULD PROB CHANGE THIS TO UNIT POSITION
			ofPoint point = phone->unitPosition;
			
			ofColor col = pixels.getColor(point.x * fbo->getWidth(), point.y* fbo->getHeight()); 
			//col.a = 20;
//			ofSetColor(col); 
//			
//			ofCircle( point.x,  point.y,12); 
//			ofCircle( point.x, point.y,8); 
//			ofCircle( point.x, point.y,6); 
//			ofCircle(point.x, point.y,3);
//			ofCircle( point.x, point.y,2);
//			col.a = 255;
//			ofSetColor(col); 
//			ofCircle(point.x, point.y,1);
			
			// NOW BROADCAST COLOUR TO PHONE! 
		
			phone->sendColour(col, latency, currentProgram->fadeUp, currentProgram->fadeDown);
		}
		
		phoneit++; 
	}
	
	
	
	//ofColor col = pixels.getColor(10, 10);
	//ofDrawBitmapString("brt: " + ofToString(col.getBrightness()), ofGetWidth() - 100, 80);
	
	ofPopMatrix(); 
	
	
	
}


void PhoneRenderer::draw() {
	
	if(currentProgram == NULL) return; 

	currentProgram->draw(); 
	
	//	
//	
//	
//	//if(currentProgram == NULL) return; 
//	
//	
//		ofFbo * fbo = &(currentProgram->fbo); 
//		//ofEnableAlphaBlending();
//		ofSetColor(255,255,255); 
//		ofPushMatrix(); 
//		ofTranslate(x, y);
//		//ofScale((float)w/(float)fbo->getWidth(), (float)h/(float)fbo->getHeight()); 
//		fbo->draw(0,0);		
//		ofPopMatrix();
//		
//		//ofDisableAlphaBlending(); 
//		ofSetColor(255,255,255,255); 

	
}

void PhoneRenderer :: changeProgram(int programnum) { 
	
	
	if((currentProgram!=NULL) && (currentProgram->started)){
		currentProgram->stop(); 
	}
	currentProgramIndex = programnum; 
		
	if(currentProgramIndex>=0 ) {
		currentProgram = outputPrograms[currentProgramIndex];
		
		currentProgramFbo = currentProgram->fbo; 
		if(currentProgram->started) currentProgram->stop();
		
		
	} else {
		currentProgram = NULL; 	
		
		//currentProgramFbo = NULL; 
		
	}
	//startProgramSwitch = false; 
	
}

void PhoneRenderer :: initGui(ofxSimpleGuiToo * gui){
	
	gui->addContent("Program FBO", currentProgramFbo);
	
	gui->addToggle("Program Running", startProgramSwitch );	
	gui->addSlider("Current Program", currentProgramIndex, 0, outputPrograms.size()-1);
	gui->addSlider("latency", latency, 0, 3000).setSize(200,30);
	gui->addSlider("framerate", frameRate, 0, 100).setSize(200,30);
	
	
	gui->addSlider("stripeSpeed", stripesEffect->speed, 1, 100).setNewColumn(true).setSize(200,30);
	gui->addSlider("fadeUp", stripesEffect->fadeUp, 0, 3).setSize(200,30);
	gui->addSlider("fadeDown", stripesEffect->fadeDown, 0, 3).setSize(200,30);
	gui->addSlider("barWidth", stripesEffect->barWidth, 1, 400).setSize(200,30);
	
	gui->addSlider("particleNum", particleWoosh->particleNum, 0, 100).setNewColumn(true).setSize(200,30);
	gui->addSlider("speed", particleWoosh->speed, 0.1, 20).setSize(200,30);
	gui->addSlider("size", particleWoosh->size, 0, 30).setSize(200,30);
	gui->addSlider("fade", particleWoosh->fade, 0, 255).setSize(200,30);
	

	gui->addToggle("Start Nyan", nyanCatch->gameStartSwitch).setNewColumn(true).setSize(200,30); 
	gui->addSlider("Nyan Speed", nyanCatch->speed, 200,3000).setSize(200,30); 
	gui->addSlider("Cat Interval", nyanCatch->catInterval, 1,3000).setSize(200,30); 
	gui->addToggle("Only found phones", nyanCatch->onlyUseFoundPhones).setSize(200,30); 
	
	//gui->addPage("Nyan Catch");
	
	
	//gui->addPage("Stroke or Poke");
	//gui->addButton("Top Stroker", strokeOrPoke->notifyTopStrokerFlag );
	//gui->addButton("Top Poker", strokeOrPoke->notifyTopPokerFlag );
	//gui->addSlider("Poker num", strokeOrPoke->topPokerPos, 0, 20 );
	//gui->addSlider("Stroker num", strokeOrPoke->topStrokerPos, 0, 20);
	
		
}



