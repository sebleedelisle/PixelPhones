/*
 *  NyanCatch.cpp
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 09/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "NyanCatch.h"


void NyanCatch::update() {

	//cout << "NyanCatch::update() " << gameState << "\n";
	
	if(connectedPhones == NULL) return; 
	// by this point connectedPhones should be updated
	
	//cout << gameStartSwitch << " " << (gameState==NYAN_STATE_PLAYING) <<  " "<< gameState << "\n";
	if(gameStartSwitch && gameState!=NYAN_STATE_PLAYING) { 
		cout << "NYAN CATCH START!\n";
		startGame(); 
	}
	
	
	// if gamestate is waiting then we'd best check that we have the up to date list of phones
	if(gameState == NYAN_STATE_WAITING) {
		
		map <int, ConnectedPhone *> ::iterator it =
		connectedPhones->begin();
		
		while(it!=connectedPhones->end()) { 
			
			ConnectedPhone * phone = it->second; 
			
			if((phone!=NULL) && ((!onlyUseFoundPhones) || phone->found)){
					
				if(nyanCatchPhonesByConnectedPhone.find(phone) == nyanCatchPhonesByConnectedPhone.end()) { 
					
					NyanCatchPhone *  nyanPhone = new NyanCatchPhone();  
					nyanCatchPhonesByConnectedPhone[phone] = nyanPhone;
					
					nyanPhone->connectedPhone = phone; 
					nyanCatchPhones.push_back(nyanPhone); 
                    nyanPhone->screenPos.set(phone->unitPosition); 
					
					cout << "adding phone to list " << phone->ID <<"\n";
					phone->sendMsg("p:nyc|start"); 
					
				}
			}
			it++;
		}
		
		
		
	// else if we're playing 	
	} else if(gameState == NYAN_STATE_PLAYING) { 
		
		//best check the timer and tell some phones to catch the cat! 

		int timer = ofGetElapsedTimeMillis(); 
	
		// and then we'd best go through the messages to see who's caught the cat
		map <int, ConnectedPhone *> ::iterator it =
		connectedPhones->begin();
		
		while(it!=connectedPhones->end()) { 
			
			ConnectedPhone * cphone = it->second; 
			
			if(nyanCatchPhonesByConnectedPhone.find(cphone) != nyanCatchPhonesByConnectedPhone.end()) { 
				
				NyanCatchPhone * phone = nyanCatchPhonesByConnectedPhone[cphone]; 
				
				if((!phone->startMessageSent) && (phone->displayCatTime - timer < 2000)) {
					
					cphone->sendMsg("p:nyc|show|"+ofToString(phone->displayCatTime)+"|"+ofToString(speed));
					cphone->sendColour(ofColor(255,255,0), phone->displayCatTime-timer);
					cphone->sendColour(ofColor(0,255,255), phone->displayCatTime+speed-timer);
					
					phone->startMessageSent = true; 
					// NOTE - should make sure this matches resolution
					catTargetPos.set(phone->screenPos * ofPoint(640,480)+ofPoint((ofGetWidth()-640)/2,(ofGetHeight()-480)/2 )); 
				}
				
				
				// now see if we have any messages from that phone... 
				
				while(cphone->programMessages.size()>0) {
					// we have program messages! 
					
					string msg = cphone->programMessages[0]; 
					cout << msg <<"\n";
					
					
						
					if(msg.find("catch")!=string::npos) {
						int touchTime = ofToInt(msg.substr(msg.find("|")+1, msg.length()) ); 
						cout << "CAUGHT " << msg.substr(msg.find("|")+1, msg.length())  << " " << (touchTime - phone->displayCatTime) << "\n";
						
						// SHOULD ADD SECURITY MEASURES HERE :) 
						
						if(!phone->catCaught) {
							phone->touchTime = (touchTime - phone->displayCatTime);
							if((phone->touchTime>0) && (phone->touchTime<speed)) { 
								cphone->sendMsg("p:nyc|caught|"+ofToString(phone->touchTime)); 
								phone->catCaught = true;
							}
						}
					} 
					
					
					cphone->programMessages.erase(cphone->programMessages.begin());
				}
				
				
				
			}
				
			it++; 
		}
				
		// if we're past the end time then set state to game over.
		if(timer> gameEndTime ) {
			gameOver();
		}
		//cout << (gameState == NYAN_STATE_OVER) << " " << (timer> gameEndTime ) << "\n"; 
		
	}
	// if game over then display winners
	
		
	
	gameStartSwitch = (gameState==NYAN_STATE_PLAYING); 
	
}

void NyanCatch :: draw() { 
	
	bool updatePhysics = (ofGetElapsedTimeMillis()-lastUpdate >16) ;
		
	if(gameState == NYAN_STATE_OVER ) { 
		
		int y = 200; 
		ofFill(); 
		ofSetColor(255,255,255); 
		ofDrawBitmapString("NYAN CATCH HIGH SCORE!", 100,y);
		y+=50; 
		
		for(int i = 0; i<nyanCatchPhones.size(); i++) {
			
			NyanCatchPhone * phone = nyanCatchPhones[i]; 
			if(phone->catCaught) { 
				
				
				if(nyanCatchPhonesByConnectedPhone.find(phone->connectedPhone) != nyanCatchPhonesByConnectedPhone.end()) {
					string msg = ofToString(phone->connectedPhone->ID) + " " + ofToString(phone->touchTime);
				
					ofDrawBitmapString(msg, 100,y);
				
					y+=40; 
				}
			}
			
			
		}
		
		
		
		
	}
	if(updatePhysics) {
		catVel*=0.8; 
		
		catVel += (catTargetPos-catPos)*0.01; 
		catPos+=catVel; 
		lastUpdate = ofGetElapsedTimeMillis(); 
	}
	
	if(catImage!=NULL) {
		ofSetColor(255); 
		ofEnableAlphaBlending(); 
		ofPushMatrix(); 
		ofTranslate(catPos.x, catPos.y); 
		float scale = 1;//(catVel.length()*0.1)+1;
		ofScale(scale,scale); 
		ofTranslate(-catImage->width/2, -catImage->height); 
		catImage->draw(0, 0); 
		ofDisableAlphaBlending();
		ofPopMatrix(); 
	}
}


bool nyanCatchSortFunction(NyanCatchPhone * first, NyanCatchPhone * second) { 
    
    if((first==NULL )|| (second == NULL)) return false; 

    int rowfirst = floor(first->screenPos.y*10);
    int rowsecond = floor(second->screenPos.y*10); 
    float index1 = rowfirst + ((rowfirst%2==0) ? first->screenPos.x : 1-first->screenPos.x);
    float index2 = rowsecond + ((rowsecond%2==0) ? second->screenPos.x : 1-second->screenPos.x);
    
    return index1<index2 ;
}

void NyanCatch::startGame() { 

	
   
    
    
	//if(!started) return; 
	// sort vector of phones in order that the cat should see them 
	
	// set the start time of the game to now!
	gameStartTime = ofGetElapsedTimeMillis();
	
	// set the time of each one 
	
	map <int, ConnectedPhone *> :: iterator it = connectedPhones->begin(); 
	
	//int counter = 0; 
    nyanCatchPhones.clear(); 
	
	while(it!=connectedPhones->end() ) {
		
		if((!onlyUseFoundPhones) || (it->second->found)) {
			
			if(nyanCatchPhonesByConnectedPhone.find(it->second)!=nyanCatchPhonesByConnectedPhone.end()) {
				   
				NyanCatchPhone* phone = nyanCatchPhonesByConnectedPhone[it->second]; 
	             
                nyanCatchPhones.push_back(phone); 
                phone->screenPos.set(it->second->unitPosition); 
				
			}
		}
		
		it++; 
	
	}
    
    sort(nyanCatchPhones.begin(), nyanCatchPhones.end(), nyanCatchSortFunction); 
    
	for(int i = 0; i<nyanCatchPhones.size(); i++) { 
        
        NyanCatchPhone* phone = nyanCatchPhones[i]; 
        
        phone->reset(); 
        
        phone->connectedPhone->sendMsg("p:nyc|reset"); 
        
        phone->displayCatTime = gameStartTime + ( i* catInterval);    
        
        
    }
	gameState = NYAN_STATE_PLAYING;

	// game ends 5 secs after all the cats have gawn. 
	gameEndTime = gameStartTime + (nyanCatchPhones.size()* catInterval) + 2000; 
	
	//cout << "gameEndTime = " << gameEndTime << " " << (gameEndTime - gameStartTime) << "\n";
	
	
	//nyanSound.setLoop(true);
	//nyanSound.play(); 
	
	catPos.set(0,0); 
	catTargetPos.set(catPos); 
}


bool myfunction (NyanCatchPhone * i,NyanCatchPhone* j) { return (i->touchTime < j->touchTime); }

void NyanCatch :: gameOver() {
	
	
	gameState = NYAN_STATE_OVER; 
	
	//bool myfunction (int i,int j) { return (i<j); }

	std::sort(nyanCatchPhones.begin(), nyanCatchPhones.end(), myfunction);
	cout << "GAME OVER \n";
	for(int i = 0; i<nyanCatchPhones.size(); i++) { 
		NyanCatchPhone * phone = nyanCatchPhones[i]; 
		
		if(nyanCatchPhonesByConnectedPhone.find(phone->connectedPhone)!=nyanCatchPhonesByConnectedPhone.end()) {
			
			cout << i << " " << phone->connectedPhone->ID << phone->touchTime << "\n";
			ConnectedPhone * cphone = phone->connectedPhone; 
			if(phone->catCaught) cphone->sendMsg("p:nyc|place|"+ofToString(i)); 
			else cphone->sendMsg("p:nyc|missed"); 

		}
		
	}
	
	
}




void NyanCatch :: start() {
	
	
	//map<int,ConnectedPhone>::iterator phoneit=connectedPhones->begin();
//	
//	while( phoneit!=connectedPhones->end()){
//		
//		ConnectedPhone	* connectedPhone = &phoneit->second; 
//		
//		connectedPhone->sendMsg("p:nyc|start"); 
//		
//		phoneit++; 
//	}
	
	
	cout << "NyanCatch::start\n";
	
	started = true; 
	gameState = NYAN_STATE_WAITING; 
	
	nyanSound.loadSound("nyanlooped.mp3");
	if(catImage==NULL) {
		
		
		catImage = new ofImage(); 
		catImage->loadImage("nyancat.png");
	
	}
}



void NyanCatch::stop() {
	
	cout << "NyanCatch::stop\n";
	
	
	if((OutputEffect::connectedPhones!=NULL) && (!connectedPhones->empty())) {
		map<int,ConnectedPhone * >::iterator phoneit=connectedPhones->begin();

		while( phoneit!=connectedPhones->end()){
			
			ConnectedPhone	* connectedPhone = phoneit->second; 
			connectedPhone->sendMsg("p:nyc|stop");
			phoneit++; 
		}
	}
	
	nyanCatchPhonesByConnectedPhone.clear(); 
	nyanCatchPhones.clear();
	started = false; 
	
	gameState = NYAN_STATE_WAITING; 
	gameStartSwitch = false; 
	
	nyanSound.stop();

}