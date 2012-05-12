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
	int w = 320; 
	int h = 240; 
	currentProgramFbo.allocate(w, h, GL_RGB, 0);
	
	addProgram(new RainbowEffect(w,h)); 
	//
	addProgram(stripesEffect = new StripesEffect(w,h)); 
    addProgram(particleWoosh = new ParticleWoosh(w,h)); 
	addProgram(pacmanEffect = new PacmanEffect(w,h)); 
	addProgram(nyanCatch = new NyanCatch()); 
    addProgram(plasmaEffect = new PlasmaEffect(w,h)); 
	addProgram(memoryGame = new MemoryGame()); 	
	

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
	currentProgram->onlyUseFoundPhones = onlyUseFoundPhones; 
	
	if(startProgramSwitch && !currentProgram->started) {	
		currentProgram->start(); 
		startProgramSwitch = true;
	} else if((!startProgramSwitch) && (currentProgram->started)) { 
		currentProgram->stop(); 
		startProgramSwitch = false;
		clearPhones();
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
		
		if(currentProgram->drawFBOToPhones) updatePhonesWithFBO(&currentProgramFbo); 
		
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
		
		if((!onlyUseFoundPhones) || phone->found) {
			
			
			//cout << "warpedPosition : " << phone->warpedPosition << "\n";
			ofColor col = pixels.getColor(ofMap(phone->warpedPosition.x,0,1,0, fbo->getWidth(),true), ofMap(phone->warpedPosition.y,0,1,0, fbo->getHeight(),true)); 
		
			// NOW BROADCAST COLOUR TO PHONE! 
		
			phone->sendColour(col, latency, currentProgram->fadeUp, currentProgram->fadeDown);
		}
		
		phoneit++; 
	}
	
	
	
	//ofColor col = pixels.getColor(10, 10);
	//ofDrawBitmapString("brt: " + ofToString(col.getBrightness()), ofGetWidth() - 100, 80);
	
	ofPopMatrix(); 
	
	
	
}


void PhoneRenderer::draw(ofRectangle* drawRect) {
	
	if((currentProgram == NULL) || (!currentProgram->started)) return; 

	currentProgram->draw(drawRect); 
	
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

void PhoneRenderer :: addProgram( OutputEffect * program) {
	
	outputPrograms.push_back(program); 

	
}



void PhoneRenderer :: changeProgram(int programnum) { 
	
	
	if((currentProgram!=NULL) && (currentProgram->started)){
		currentProgram->stop(); 
	}
	currentProgramIndex = programnum; 
		
	if(currentProgramIndex>=0 ) {
		currentProgram = outputPrograms[currentProgramIndex];
		
		currentProgram->fbo = &currentProgramFbo; 
		if(currentProgram->started) currentProgram->stop();
		
		
	} else {
		currentProgram = NULL; 	
		
		//currentProgramFbo = NULL; 
		
	}
	//startProgramSwitch = false; 
	
	clearPhones(); 
	
}

void PhoneRenderer :: clearPhones() { 
	
	map <int,ConnectedPhone *> * phones = &(commsManager->connectedPhones); 
	
	map<int,ConnectedPhone *>::iterator phoneit = phones->begin();
	
	while(phoneit!=phones->end()) {
		
		ConnectedPhone * phone = phoneit->second; 
		
		// NOW BROADCAST COLOUR TO PHONE! 
			
		phone->sendColour(0,0,0,0);
		
		
		phoneit++; 
	}
}

void PhoneRenderer :: initGui(ofxSimpleGuiToo * gui){
	
	gui->addContent("Program FBO", currentProgramFbo);
	
	gui->addToggle("Program Running", startProgramSwitch );	
	gui->addSlider("Current Program", currentProgramIndex, 0, outputPrograms.size()-1);
	gui->addSlider("latency", latency, 0, 3000).setSize(200,30);
	gui->addSlider("framerate", frameRate, 0, 100).setSize(200,30);
	gui->addToggle("Only use found phones", onlyUseFoundPhones).setSize(200,30); 
	
	gui->addSlider("stripeSpeed", stripesEffect->speed, 1, 100).setNewColumn(true).setSize(200,30);
	gui->addSlider("fadeUp", stripesEffect->fadeUp, 0, 3).setSize(200,30);
	gui->addSlider("fadeDown", stripesEffect->fadeDown, 0, 3).setSize(200,30);
	gui->addSlider("barWidth", stripesEffect->barWidth, 1, 400).setSize(200,30);
	
	gui->addSlider("particleNum", particleWoosh->particleNum, 0, 100).setNewColumn(true).setSize(200,30);
	gui->addSlider("speed", particleWoosh->speed, 0.1, 20).setSize(200,30);
	gui->addSlider("size", particleWoosh->size, 0, 30).setSize(200,30);
	gui->addSlider("fade", particleWoosh->fade, 0, 255).setSize(200,30);

	gui->addSlider("plasmaSpeed", plasmaEffect->speed, 0, 2).setNewColumn(true).setSize(200,30);
	gui->addSlider("gen1", plasmaEffect->gen1, 1, 50).setSize(200,30);
	gui->addSlider("gen2", plasmaEffect->gen2, 1, 50).setSize(200,30);
	gui->addSlider("gen3", plasmaEffect->gen3, 1, 50).setSize(200,30);
	gui->addSlider("gen4", plasmaEffect->gen4, 1, 50).setSize(200,30);
	gui->addSlider("steps", plasmaEffect->steps, 0, 20).setSize(200,30);
	gui->addSlider("hueOffset", plasmaEffect->hueOffset, 0, 255).setSize(200,30);
	gui->addSlider("hueRange", plasmaEffect->hueRange, 0, 255).setSize(200,30);
	gui->addSlider("brightnessOffset", plasmaEffect->brightnessOffset, 0, 255).setSize(200,30);
	gui->addSlider("brightnessRange", plasmaEffect->brightnessRange, 0, 255).setSize(200,30);
	gui->addSlider("saturation", plasmaEffect->saturation, 0, 255).setSize(200,30);

    gui->addPage("PhoneRenderer2");
    
    gui->addToggle("Program Running", startProgramSwitch );	
	gui->addSlider("Current Program", currentProgramIndex, 0, outputPrograms.size()-1);
	gui->addSlider("latency", latency, 0, 3000).setSize(200,30);
	gui->addToggle("Only use found phones", onlyUseFoundPhones).setSize(200,30); 

	gui->addToggle("Start Nyan", nyanCatch->gameStartSwitch).setNewColumn(true).setSize(200,30); 
	gui->addSlider("Nyan Speed", nyanCatch->speed, 200,3000).setSize(200,30); 
	gui->addSlider("Cat Interval", nyanCatch->catInterval, 1,3000).setSize(200,30); 
	//gui->addToggle("Only found phones", nyanCatch->onlyUseFoundPhones).setSize(200,30); 
	
	//gui->addPage("Nyan Catch");
	
	
	//gui->addPage("Stroke or Poke");
	//gui->addButton("Top Stroker", strokeOrPoke->notifyTopStrokerFlag );
	//gui->addButton("Top Poker", strokeOrPoke->notifyTopPokerFlag );
	//gui->addSlider("Poker num", strokeOrPoke->topPokerPos, 0, 20 );
	//gui->addSlider("Stroker num", strokeOrPoke->topStrokerPos, 0, 20);
	
	for(int i=0; i<outputPrograms.size(); i++) { 
		outputPrograms[i]->initGui(gui); 
		
	}
	
	
}



