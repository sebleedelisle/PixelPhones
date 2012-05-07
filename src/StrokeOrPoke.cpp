/*
 *  StrokeOrPoke.cpp
 *  PixelPhone05
 *
 *  Created by Seb Lee-Delisle on 04/09/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "StrokeOrPoke.h"

void StrokeOrPoke :: draw(ofRectangle* drawRect) { 
	if(!running) return;
	
	numPokes = 0; 
	numStrokes = 0; 
	
	map<int,ConnectedPhone>::iterator phoneit=connectedPhones->begin();
	//cout << "strokeOrPoke::draw()\n";
	while( phoneit!=connectedPhones->end()){
		
		ConnectedPhone	* connectedPhone = &phoneit->second; 

		if(sopData.find(connectedPhone) == sopData.end() ) {
			
			if(connectedPhone->isConnected()) {
				/// make a new SOPData!
				cout << "making a new SOPDatum for id : " << connectedPhone->ID <<"\n";
				sopData[connectedPhone];
				SOPData * sopdatum = &(sopData[connectedPhone]); 
				sopdatum->phone = connectedPhone; 
				
				connectedPhone->sendMsg("p:sop|start"); 
				
				
			}
			
		}
				
		SOPData * sopdatum = &(sopData[connectedPhone]); 
		
		while(connectedPhone->programMessages.size()>0) {
			// we have program messages! 
			
			string msg = connectedPhone->programMessages[0]; 
			//cout << msg <<"\n";
			
			
			if(ofGetElapsedTimeMillis() - sopdatum->lastMessageTime > 1000) {
				
				if(msg.find("stroke")!=string::npos) {
					sopdatum->strokeCount++; 
					sopdatum->lastActionWasStroke = true;
					numStrokes++; 
				} else if(msg.find("poke")!=string::npos) {
					sopdatum->pokeCount++; 
					sopdatum->lastActionWasStroke = false;
					numPokes++; 
				}
				
				sopdatum->lastMessageTime = ofGetElapsedTimeMillis(); 
			//	cout << sopdatum->strokeCount << " " << sopdatum->pokeCount << "\n";
			}
		
			connectedPhone->programMessages.erase(connectedPhone->programMessages.begin());
		}
		
		
		if(ofGetElapsedTimeMillis() - sopdatum->lastMessageTime < 10000) { 
			
			(sopdatum->lastActionWasStroke ? numStrokes : numPokes)++;
			
		}
				
		phoneit++;
		
		
	}
	
	map<ConnectedPhone *, SOPData>::iterator sopit = sopData.begin();

	while(sopit!=sopData.end()) {
		
		ConnectedPhone * phone = sopit->first; 
		
		if(connectedPhones->find(phone->ID) == connectedPhones->end()) { 	
			sopData.erase(sopit); 
		}
		
		
		sopit++;
	}
	
	//cout << "strokes/pokes :  " << numStrokes << " " << numPokes << " " << sopData.size() << "\n";

	float targetStroke = (float)numStrokes / (float)(( sopData.size()>10) ? sopData.size() : 10); 
	float targetPoke = (float)numPokes / (float)(( sopData.size()>10) ? sopData.size() : 10); 
	
	
	currentStrokeRating += ((targetStroke - currentStrokeRating)* ((targetStroke>currentStrokeRating)? 0.5 : 0.01)); 
	currentPokeRating+= ((targetPoke - currentPokeRating)* ((targetPoke>currentPokeRating)? 0.5 : 0.01));
	
	fbo->begin(); 
	ofClear(0, 0, 0, 0);
	ofFill(); 
	ofSetColor(255, 0, 0);
	ofRect(0, 0, currentPokeRating * 300 , 20);
	
	ofSetColor(0, 255, 0);
	ofRect(0, 30, currentStrokeRating* 300, 20);
	
	
	
	fbo->end();
	
	
	if(notifyTopStrokerFlag) { 
		notifyTopStroker(); 
	} 
	if(notifyTopPokerFlag) { 
		notifyTopPoker(); 
	}
	

}

void StrokeOrPoke::notifyTopPoker() { 
	
	notifyTopPokerFlag = false; 
	
	if(sopData.size()==0) return; 
	
	map<ConnectedPhone *, SOPData>::iterator sopit = sopData.begin();
	
	int pokeCount = -1; 
	ConnectedPhone * phone; 
	
	while(sopit!=sopData.end()) {
		
		SOPData * sopdata = &(sopit->second); 
		
		if(sopdata->pokeCount > pokeCount) { 
			pokeCount = sopdata->pokeCount; 
			phone = sopdata->phone;
		}
		
		sopit++;
	}
	
	if(phone!=NULL) {		
		phone->sendMsg("p:sop|flash");
	}
}
void StrokeOrPoke::notifyTopStroker() { 
	
	notifyTopStrokerFlag = false; 
	
	if(sopData.size()==0) return; 
	
	map<ConnectedPhone *, SOPData>::iterator sopit = sopData.begin();
	
	int strokeCount = -1; 
	ConnectedPhone * phone; 
	
	while(sopit!=sopData.end()) {
		
		SOPData * sopdata = &(sopit->second); 
		
		if(sopdata->strokeCount > strokeCount) { 
			strokeCount = sopdata->strokeCount; 
			phone = sopdata->phone;
		}
		
		sopit++;
	}
	
	if(phone!=NULL) {
		phone->sendMsg("p:sop|flash");
	}
}


void StrokeOrPoke :: start() {
	
	running = true; 
	
	map<int,ConnectedPhone>::iterator phoneit=connectedPhones->begin();
	
	while( phoneit!=connectedPhones->end()){
		
		ConnectedPhone	* connectedPhone = &phoneit->second; 
		
		connectedPhone->sendMsg("p:sop|start"); 

		
		if(sopData.find(connectedPhone) == sopData.end() ) {
			
			if(connectedPhone->isConnected()) {
				/// make a new SOPDatum!
				cout << "making a new SOPDatum for id : " << connectedPhone->ID <<"\n";
				sopData[connectedPhone];
				SOPData * sopdatum = &(sopData[connectedPhone]); 
				sopdatum->phone = connectedPhone; 
				
				
			}
			
		}
		phoneit++; 
	}
	
	
	
}



void StrokeOrPoke::stop() {
	if(!running) return; 
	running = false; 
	if((OutputEffect::connectedPhones!=NULL) && (!connectedPhones->empty())) {
		map<int,ConnectedPhone>::iterator phoneit=connectedPhones->begin();
		//cout << "strokeOrPoke::draw()\n";
		while( phoneit!=connectedPhones->end()){
			
			ConnectedPhone	* connectedPhone = &phoneit->second; 
			connectedPhone->sendMsg("p:sop|stop");
			phoneit++; 
		}
	}
}