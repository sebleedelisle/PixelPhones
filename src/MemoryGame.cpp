//
//  MemoryGame.cpp
//  PixelPhones
//
//  Created by Seb Lee-Delisle on 07/05/2012.
//  Copyright (c) 2012 seb.ly. All rights reserved.
//

#include "MemoryGame.h"

void MemoryGame::update() { 
	
	timeNow = ofGetElapsedTimeMillis(); 
	
	// NOTE We'll need to also check to see if it's game over
    
	
	if((gameState == MEM_GAME_STATE_WAITING)||(gameState == MEM_GAME_STATE_OVER)) { 
		
		updatePhoneList(); 
		
		if(gameStartSwitch) { 
			cout << "MEMORY GAME START\n";
			startGame(); 
		}
		
	} else if(gameState == MEM_GAME_STATE_BETWEEN_LEVELS) { 
		// we'll wait between levels. Until a button is pressed. 
		// And then call newLevel(); 
		if((newLevelSwitch)||(ofGetElapsedTimeMillis()>nextLevelStartTime)){ 
			newLevel(); 
			//gameStartSwitch = false; 
		}
		
	} else if(gameState == MEM_GAME_STATE_SHOWING_LEVEL) { 
		// we need a start time and a last colour shown time
		// how do we sequence the level colours? 

		if((currentShowColourIndex<colours.size()) && (timeNow>times[currentShowColourIndex])) { 
			
			currentShowColour = colours[currentShowColourIndex]; 
			tones[currentShowColour].play();
			cout << "CHANGING COLOUR : " <<  colours[currentShowColourIndex] << "\n";
			currentShowColourIndex++; 
		} else if(timeNow>times[currentShowColourIndex-1]+(timePerColourShow*0.7)) {
			currentShowColour = -1; 
			
		}

		// if we've finished playing out the colours, then 
		// start receiving input
		
		if(timeNow>stopShowingTime) { 
			startReceivingPresses();
		}
		
	} else if(gameState == MEM_GAME_STATE_RECEIVING_PRESSES) {
		// then check the messages from the phones! 
		
		updatePhoneCounts(); 
		
		if((numPhonesPlayingLevel==0) || ( timeNow  > stopReceivingTime + 2000)) { 
			// TIME UP
			stopReceivingPresses();
			updatePhoneCounts();
			if(numPhonesInPlay==0) { 
				// END GAME!
				gameState = MEM_GAME_STATE_OVER;
				updateWinners(); 
				
				// NOTIFY WINNERS HERE! 
				
			} else { 
				nextLevelStartTime = ofGetElapsedTimeMillis()+1500; 
				
			}
		}

	}
	
	getPhoneMessages(); 
	
    gameStartSwitch = ((gameState != MEM_GAME_STATE_WAITING) && (gameState != MEM_GAME_STATE_OVER)); 
    newLevelSwitch = (gameState != MEM_GAME_STATE_BETWEEN_LEVELS); 
    
    lastUpdateTime = timeNow; 

}

void MemoryGame::startReceivingPresses() {
	
	gameState = MEM_GAME_STATE_RECEIVING_PRESSES;
	cout << "RECEIVING PRESSES \n";
	
}


void MemoryGame::stopReceivingPresses() {
	
	gameState = MEM_GAME_STATE_BETWEEN_LEVELS;
	cout << "RECEIVING PRESSES END\n";	
	
	map <int, ConnectedPhone *> ::iterator it =
	connectedPhones->begin();
	
	while(it!=connectedPhones->end()) { 
		ConnectedPhone * cphone = it->second; 
		if((cphone!=NULL) && (cphone->isConnected()) && (phoneGameData.find(cphone) != phoneGameData.end())) { 
			
			MemoryGamePhoneData * data = phoneGameData[cphone]; 
			if((data->inPlay) && (!data->levelSuccess)) {
				cphone->sendMsg("p:mem|fail");
				data->inPlay = false; 
			}
		}
		it++; 
	}
	
	
}


void MemoryGame::updatePhoneCounts() { 
	
	numPhonesInPlay = 0; 
	numPhonesPlayingLevel = 0; 
	
	map <int, ConnectedPhone *> ::iterator it =
	connectedPhones->begin();
	
	while(it!=connectedPhones->end()) { 
		
		ConnectedPhone * cphone = it->second; 
		
		if((cphone!=NULL) && (cphone->isConnected()) && (phoneGameData.find(cphone) != phoneGameData.end())) { 
			
			MemoryGamePhoneData * data = phoneGameData[cphone]; 
			if(data->inPlay) {
				numPhonesInPlay++; 
				if(!data->levelSuccess) {
					numPhonesPlayingLevel++; 
				}
			}
			
		}
	
		it++;
	}
	
}


bool sortWinnersFunction(MemoryGamePhoneData * first, MemoryGamePhoneData * second) { 
    
    if((first==NULL )|| (second == NULL)) return false; 
	
    return ((first->levels.size() > second->levels.size())|| (first->averagePressTime<second->averagePressTime));
}



void MemoryGame::updateWinners() { 
	
	map <int, ConnectedPhone *> ::iterator it =
	connectedPhones->begin();
	
	winnersList.clear(); 
	
	while(it!=connectedPhones->end()) { 
		
		ConnectedPhone * cphone = it->second; 
		
		if((cphone!=NULL) && (cphone->isConnected()) && (phoneGameData.find(cphone) != phoneGameData.end())) { 
			
			MemoryGamePhoneData * data = phoneGameData[cphone]; 
			
			winnersList.push_back(data); 
			data->updateAveragePressTime();
	
		}
		it++;
	}
	
	sort(winnersList.begin(), winnersList.end(), sortWinnersFunction); 
		
}



void MemoryGame::getPhoneMessages() { 
	
	map <int, ConnectedPhone *> ::iterator it =
	connectedPhones->begin();
	
	while(it!=connectedPhones->end()) { 
		
		ConnectedPhone * cphone = it->second; 

		if((cphone!=NULL) && (cphone->isConnected()) && (phoneGameData.find(cphone) != phoneGameData.end())) { 
				
			MemoryGamePhoneData * data = phoneGameData[cphone]; 
			
			
			
			while(cphone->programMessages.size()>0) {
				// we have program messages! 
				
				string msg = cphone->programMessages[0]; 
				cout << msg <<"\n";
				
				if(msg.find("press")!=string::npos) {
		
					vector <string> values = ofSplitString(msg, "|");
					if(values.size()>3) { 
						int button = ofToInt(values[2]); 
						int time = ofToInt(values[3]); 
						
						if(data->inPlay && !data->levelSuccess) {
							
							
							cout << startReceivingTime << " " << stopReceivingTime << "\n";
							cout << "button press " << button << " at time "<< time << "\n";
							
							if((time>startReceivingTime) && (time<stopReceivingTime)) { 
								data->addButtonPress(levelNum, button, time-startReceivingTime);
							} 	
							
							int numCorrect = data->checkButtonPresses(colours, levelNum);
							
							if((time>stopReceivingTime) || (time<startReceivingTime) || (numCorrect<0)) {
								
								data->inPlay = false; 
								data->levelSuccess = false; 
								cphone->sendMsg("p:mem|fail");
								
							}
							
							if(numCorrect == colours.size()) { 
								cphone->sendMsg("p:mem|win");
								data->levelSuccess = true; 
							}
						}
						
					}
				} 
				
				
				cphone->programMessages.erase(cphone->programMessages.begin());
			}
			
			
			
			
			
			
			
		}
		
		
		
		it++; 
	}
	
	
	
}




void MemoryGame::draw(ofRectangle* drawRect) { 

	
	if(gameState == MEM_GAME_STATE_SHOWING_LEVEL) {
		if(currentShowColour>=0) { 
			ofFill(); 
			ofSetColor(colourRefs[currentShowColour]);
			
			ofRectangle displayRect = *drawRect;		
			
			displayRect.width*=0.5; 
			displayRect.height*=0.5; 
			
			if(currentShowColour%2==1) displayRect.x += displayRect.width;
			if(currentShowColour>1) displayRect.y += displayRect.height;
			
			
			ofRect(displayRect); 
		}
		
		
	} else if (gameState == MEM_GAME_STATE_RECEIVING_PRESSES ) { 
		
		float progress = ofMap(ofGetElapsedTimeMillis(), startReceivingTime, stopReceivingTime, 0, 1, true); 
		
		
		ofFill(); 
		ofSetColor(255); 
		ofRect(drawRect->x, drawRect->y, progress * drawRect->width, 20); 
		
		
	} 
	
	map <int, ConnectedPhone *> ::iterator it =
	connectedPhones->begin();
	
	while(it!=connectedPhones->end()) { 
		
		ConnectedPhone * cphone = it->second; 
		
		if((cphone!=NULL) && (cphone->isConnected())  && (phoneGameData.find(cphone) != phoneGameData.end())) { 
			
			MemoryGamePhoneData * data = phoneGameData[cphone]; 
		
			ofPoint pos = cphone->unitPosition; 
			pos.x* drawRect->width; 
			pos.y* drawRect->height; 
			pos.x+=drawRect->x; 
			pos.y+=drawRect->y;
			
			
			if (data->inPlay) { 
			
				ofPushMatrix(); 
				ofTranslate(pos.x, pos.y); 
				if(data->currentLevel!=NULL) { 
					for(int i = data->currentLevel->pressButtons.size()-1; i>=0 ; i--) { 
						ofSetColor(colourRefs[data->currentLevel->pressButtons[i]]); 
						ofFill(); 
						ofRect(-2,-2,4,4); 					  
						ofTranslate(0,-5); 					
					}
				}
				ofPopMatrix(); 
		
			} else { 
				
				ofNoFill(); 
				ofSetColor(255,0,0); 
				ofLine(pos.x-10, pos.y-10, pos.x+10, pos.y+10); 
				ofLine(pos.x+10, pos.y-10, pos.x-10, pos.y+10); 
				ofFill(); 
				//ofCircle(pos.x, pos.y, 30); 
				
			}
			
			// otherwise draw ticks and colours... 
		
		
		
		}
		
		it++; 
	}
	
	ofFill(); 
	ofSetColor(255); 
	if (gameState == MEM_GAME_STATE_OVER ) { 
	
		ofPushMatrix(); 
		ofTranslate(drawRect->x, drawRect->y); 
		for(int i =0; i<winnersList.size(); i++) { 
			ofDrawBitmapString(ofToString(winnersList[i]->ID) +" "+ofToString(winnersList[i]->levels.size())+" "+ofToString(winnersList[i]->averagePressTime) , 30,30*(i+1)); 
			
			
		}
							   
		
		ofPopMatrix(); 
	}
		
}

void MemoryGame::startGame() { 

	updatePhoneList(); 
    gameState = MEM_GAME_STATE_BETWEEN_LEVELS;
	colours.clear(); 
	times.clear(); 
	levelNum = -1; 
	sendAllPhones("p:mem|reset"); 
	
	map <ConnectedPhone *, MemoryGamePhoneData *> ::iterator it = phoneGameData.begin();
	
	while(it!=phoneGameData.end()) { 
		MemoryGamePhoneData * data = it->second; 
		data->reset(); 
		
		it++;
	}
	nextLevelStartTime = ofGetElapsedTimeMillis(); 
	
    
}

void MemoryGame::newLevel(){ 
	
	//timePerColourShow = 300; 
	levelNum ++; 
	
	currentShowColour = -1; 
	
	colours.push_back(floor(ofRandom(0,4))); 
	times.push_back(1); 
	
	startShowingTime = ofGetElapsedTimeMillis(); 

	
	for(int i =0; i<times.size(); i++) { 
		times[i] = (i * timePerColourShow) + startShowingTime; 
		
		
		sendAllPhones("c"+ofToHex(colourRefs[colours[i]].getHex()).substr(2,6)+"|t"+ofToString(times[i]));
		
		sendAllPhones("c000000|t"+ofToString(times[i]+timePerColourShow*0.7));

	}
	
	gameState = MEM_GAME_STATE_SHOWING_LEVEL;
	
	stopShowingTime = startShowingTime + (timePerColourShow * times.size()); 
	
	currentShowColourIndex = 0; 
	
	
	startReceivingTime = stopShowingTime + 500; 
	stopReceivingTime = startReceivingTime + ((times.size()+2) * timePerPress); 
	
	//sendAllPhones("p:mem|showButtons|" + ofToString(startReceivingTime)+'|'+ofToString(stopReceivingTime)); 
	
	map <int, ConnectedPhone *> ::iterator it = connectedPhones->begin();
	
	while(it!=connectedPhones->end()) { 
		ConnectedPhone * cphone = it->second; 
		if((cphone!=NULL) && (cphone->isConnected()) && (phoneGameData.find(cphone) != phoneGameData.end())) { 
			
			MemoryGamePhoneData * data = phoneGameData[cphone]; 
			if(data->inPlay) { 
				data->levelSuccess = false; 
				cphone->sendMsg("p:mem|showButtons|" + ofToString(startReceivingTime)+'|'+ofToString(stopReceivingTime));
				
			}
			
			
		}
		it++;
	}

	
	
	
	cout << "NEW LEVEL \n";
	
}

// start and stop are automatically called by the phoneRenderer when you 
// change program. 

void MemoryGame::start() { 
    started = true; 
    gameState = MEM_GAME_STATE_WAITING;
	gameStartSwitch = false; 
}


void MemoryGame::stop() { 
    started = false; 
    gameState = MEM_GAME_STATE_WAITING;
	sendAllPhones("p:mem|stop"); 
    
}

void MemoryGame::updatePhoneList() { 
    
	// get the definitive list of phones that are playing
    
	map <int, ConnectedPhone *> ::iterator it = connectedPhones->begin();
	while(it!=connectedPhones->end()) { 
        
		ConnectedPhone * phone = it->second; 
		if((phone!=NULL) && ((!onlyUseFoundPhones) || phone->found)){
			
			if(phoneGameData.find(phone) == phoneGameData.end()) { 
				phoneGameData[phone] = new MemoryGamePhoneData(); 
				
				phoneGameData[phone]->ID = phone->ID; 
				// make sure this phone is playing this game! 
				phone->sendMsg("p:mem|start"); 
				
			}
			phoneGameData[phone]->inPlay = true;
		}
		
		it++;
	}	
	
	
	
}


void MemoryGame::sendAllPhones(string msg) { 
	
	if((connectedPhones!=NULL) && (!connectedPhones->empty())) {
		map<int,ConnectedPhone * >::iterator phoneit=connectedPhones->begin();
		
		while( phoneit!=connectedPhones->end()){
			
			ConnectedPhone	* connectedPhone = phoneit->second; 
			connectedPhone->sendMsg(msg);
			phoneit++; 
		}
	}
	
	
}

void MemoryGame::initGui(ofxSimpleGuiToo * gui) { 
	
	gui->addPage("MemoryGame"); 
	 
    gui->addToggle("Start game", gameStartSwitch).setSize(300,20);
    gui->addToggle("Start level", newLevelSwitch).setSize(300,20);
	gui->addSlider("Show speed", timePerColourShow, 100,1000).setSize(200,30); 
	gui->addSlider("Play speed", timePerPress, 500,2000).setSize(200,30); 
	gui->addSlider("In play", numPhonesInPlay, 0,500).setSize(200,30); 
	gui->addSlider("Playing level", numPhonesPlayingLevel, 0,500).setSize(200,30); 
	gui->addSlider("state", gameState, 0, 5).setSize(200,30); 
	
}


