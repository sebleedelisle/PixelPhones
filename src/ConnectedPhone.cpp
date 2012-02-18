/*
 *  ConnectedPhone.cpp
 *  PixelPhones02
 *
 *  Created by Seb Lee-Delisle on 23/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "ConnectedPhone.h"

void ConnectedPhone::setup(WebSocketClient *webclient, int id) { 

	webClient = webclient; 
	webClient->tcpClient->setVerbose(true); 
	
	//TODO : Maybe ID shouldn't be set here? 
	ID = id; 
	connectionReady = false; 
	counter = 0; 
	broadcastingID = false;
	found = false; 
	
	syncStatus = NOT_SYNCED;
	isDragging = false; 
	
	
	//queuedMessages.append(ofToString((char)0) + "hellooo?" +(ofToString((char)0xff))); 
	
}

void ConnectedPhone::update() { 

	//bool connectionReady = webClient->connectionReady; 
	bool handShaked = webClient->handShaked; 
	// should probably see what we're getting from the phone ? for reconnection and stuff. 
	webClient->update(); 
	
	if((!handShaked) && (webClient->handShaked)){
		// just connected! So send messages... 
	
		cout << "CONNECTED!\n" ;
	
		//sendColour(ofColor(255,128,0)); 

	} else if(webClient->receiveString.length()>0) {
		
		// should really break this up into separate messages... 
		
		
		vector <string> msgs = ofSplitString(webClient->receiveString, ofToString((char)0xff));
		
		for(int i=0; i<msgs.size(); i++) {
			
			string msg = msgs[i];
			//cout << "msg : "<< msg <<"\n";
			if(!connectionReady) {
				
				if(msg.find("ready")!=string::npos) {
					cout << "connectionReady  "<<ID<<"\n";
					connectionReady = true; 
					// TODO SHould actually just strip out up to the 
					// 0xff; 
					sendMsg( "i"+ofToString(ID));
					sendColour(ofColor(0,255,0)); 
					// if there's a queue of stuff built up, send it
					
					
					
				}; 
			} else {
				// here's where we parse stuff
			
				if(msg.find("t")==1) { 
					//clock_t now;
					//now = clock(); 
					
					//cout << "time now "	<< getRelativeSystemTime() << " " << getRelativeSystemTime() <<"\n";
					sendMsg("t"+ofToString(ofGetElapsedTimeMillis())); 
					
				} else if (msg.find("p")==1) {
					// these are program messages! 
					programMessages.push_back(msg); 
					
				} else if(msg.find("sync")!=string::npos) { 
					//cout << msg << "\n";
					//cout << "sync " << msg.substr(5, msg.length()) << " " << ofToInt(msg.substr(5, msg.length()) ) << "\n";
					syncStatus = SYNCED; 
					latency = ofToInt(msg.substr(5, msg.length()) ); 
					//cout << "SYNCED! Latency : " << latency << "\n";
					
				}
				
				
			}
			// TODO - make sure we're not half way through a message! 
			webClient->receiveString = ""; 
		}
		//sendColour(ofColor());
		
		if(connectionReady) sendQueue(); 
		
		// and delete the stored string
		
	}
	
	if(isDragging) {
		pixelPosition.set(ofGetMouseX()-dragOffset.x, ofGetMouseY()-dragOffset.y);
		unitPosition.set( pixelPosition.x/(float)ofGetWidth(), pixelPosition.x/(float)ofGetHeight() );
		//cout << pixelPosition << unitPosition << "\n";
		
	}
	counter++; 
	
}

void ConnectedPhone::draw(int brightness) { 
	
	ofPushMatrix(); 
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofTranslate( pixelPosition.x, pixelPosition.y);
	if(found) ofSetColor(0,brightness, 0); 
	else ofSetColor((int)(brightness/2)); 
	
	ofNoFill(); 
	ofSetLineWidth(1);
	ofEllipse(0,0, 20, 20);
	ofFill();
	//ofSetColor(brightness); 

	string idstr = ofToString(ID); 
	
	labelFont->drawString(idstr,(-(float)idstr.size()*7.0f/2.0f),4);
	ofPopMatrix(); 
	//get the ip and port of the client
	
	//string info = "phone id "+ofToString(phone->ID)+" - pos "+ofToString(phone->unitPosition.x)+", "+ofToString(phone->unitPosition.x)+", "+((phone->broadcastingID)?"broadcasting":"") + ", "+((phone->found)?"found":"") ;
	
	
	//draw the info text and the received text bellow it
	//ofDrawBitmapString(info, xPos, yPos);
	
	ofDisableBlendMode();
	
}


void ConnectedPhone::sendColour(ofColor col, int latency, float fadeUp, float fadeDown) {

	
	if(col==currentColour) return;
	
	string msg = ofToString("c"+ ofToHex(col.getHex()).substr(2,6));
	
	bool fading = false; 
	float colourDifference = col.getBrightness() - currentColour.getBrightness() ; 
	
	if((colourDifference>0) && (fadeUp!=0)) {
		msg+="|f"+ofToString(fadeUp);
	} else if((colourDifference<0)  && (fadeDown!=0)) {
		msg+="|f"+ofToString(fadeDown);
	}
	
	if(latency>0) {
		msg+="|t" + ofToString(ofGetElapsedTimeMillis() + latency); 
		//cout << "latency = " << latency << " " <<getRelativeSystemTime() << " " << msg << "\n";
		
	}
	
	sendMsg(msg); 
	currentColour = col; 

}
void ConnectedPhone::sendFrameRate(int framerate) { 
	
	sendMsg("f"+ofToString(framerate));
	
}

void ConnectedPhone::sendNumBits(int numbits) { 
	
	sendMsg("n"+ofToString(numbits));
	
}

void ConnectedPhone::sendMsg(string msg) { 
	//cout << "queued "<< queuedMessages.empty() << "\n";
	if((!connectionReady) || (!tcp->isClientConnected(ID)))  //(!webClient->isConnected()))
		queuedMessages.push_back(msg); 
	else
		webClient->tcpClient->sendRaw(ofToString((char)0) + msg + ofToString((char)0xff)); 

}

void ConnectedPhone::broadcastID() { 
	if(!broadcastingID) {
		// make white flash that we can pick up in our camera!
		sendColour(ofColor(255,255,255));
		sendMsg("b1"); 
	}
	broadcastingID = true; 
}

void ConnectedPhone::stopBroadcastingID() { 
	//if(broadcastingID) {
		sendMsg("b0");
		//sendMsg("b0");
		broadcastingID = false; 
		//cout << "StopBroadcastingID " << ID << "\n"; 

	//}
}

void ConnectedPhone::sendQueue() { 
	if(!isConnected()) return; 
	if(queuedMessages.size()>0)  {
		for(int i = 0; i<queuedMessages.size(); i++) {
			webClient->tcpClient->sendRaw(ofToString((char)0) + queuedMessages[i] + ofToString((char)0xff)); 
		}
			
		queuedMessages.clear(); 
	}
}
	
bool ConnectedPhone::isConnected() {
	return tcp->isClientConnected(ID);
}

void ConnectedPhone::close() { 
	webClient->tcpClient->close(); 
}

void ConnectedPhone :: reset(int newid) { 
	ID = newid; 
	cout << "resetting id to " << ID;
	sendMsg("i"+ofToString(ID)); 
	found = false; 
	
}

void ConnectedPhone :: startSync() { 
	if(syncStatus!=NOT_SYNCED) return;
	sendMsg("sync"); 
	syncStatus = SYNC_IN_PROGRESS;
}

bool ConnectedPhone ::hitTest(int x,int y) { 
		
	ofVec2f mouseDistance(x,y);
	mouseDistance-=pixelPosition;
	
	//cout << "hittest "<< mouseDistance << "\n";
	
	if(mouseDistance.squareLength() < 20*20) return true;
	else return false; 
	
	
}

void ConnectedPhone :: startDragging(int x, int y) { 
	if(!isDragging); 
	isDragging = true; 
	dragOffset.set(x-pixelPosition.x, y-pixelPosition.y); 
	found = !found; 
	
}

void ConnectedPhone :: stopDragging() { 
	if(isDragging); 
	isDragging = false; 
	//found = true; 
	
}