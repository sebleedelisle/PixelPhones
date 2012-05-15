/*
 *  CommsManager.cpp
 *  PixelPhones02
 *
 *  Created by Seb Lee-Delisle on 31/07/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "CommsManager.h"
#include "homography.h"

CommsManager::CommsManager() { 
	
	broadcastingIDs = false; 
	labelFbo.allocate(20, 20, GL_RGBA, 0);

	destPoints[0].set(0, 0, 0);
	destPoints[1].set(1, 0, 0);
	destPoints[2].set(1, 1, 0);
	destPoints[3].set(0, 1, 0);
	
	calibrating = false; 

}



bool CommsManager::setup(int portnum){

	syncing = false; 
	broadcastingIDs = false; 
	bool connected = false; 
	
	portNum = portnum; 
	connected = server.setup(portNum);
	
	cout << "SERVER connected : " << (connected?"SUCCESS \n" : "FAILED \n");
	labelFont.loadFont("Andale Mono.ttf",8, false); 

	return connected; 
	
	
}


void CommsManager::update(){


    // Check for phones that have disconnected and set up 
    // new phones that connect. 
    
	map<int,WebSocketClient>::iterator it;

	for(it=server.webSocketClients.begin(); it!=server.webSocketClients.end(); it++){
		
		// index number for this connection 
		int i = it->first; 
		
		if((!server.isClientSetup(i)) || (server.getClientPort(i)==0)) {
			// then it's a fake client! Or something :/ Or actually I 
			// think it's disconnected
            
			map<int,ConnectedPhone*>::iterator phoneit = connectedPhones.find(i);
            
			// should probably find relevant connectedPhone and destroy it. 
			if(phoneit!=connectedPhones.end()) { 
				// phoneit->second->close();
				cout << "we have a connectedPhone object for a phone that has been disconnected:" << i <<" \n";
			}
            
		} else {
			
			// go through each tcp client and check 
			// whether we've set them up with a phone communicator object yet - if we haven't then 
			// we do the handshake 
			
			if(connectedPhones.find(i)==connectedPhones.end()){
				
				//WebSocketClient * client = &it->second; 
				
				//WebSocketClient * websocketclient = new WebSocketClient(); 
				//websocketclient->portNum = portNum;
				
				//TODO check if this is the best way to distribute ids
				//websocketclient->setup(i,client); 
				int id = i;//floorf(ofRandom(0,16));
				//websocketclient->setup(id,client); 
				
				// this makes a new one !
				ConnectedPhone * phone = new ConnectedPhone(&labelFbo); 
				phone->labelFont = &labelFont; 
				connectedPhones[i] = phone; 
				phone->tcp = &server; 
				
				phone->setup(id); 
				phone->sendNumBits(numBits);
				phone->sendFrameRate(phoneFrameRate, doubleToSingleRatio, blackTimeOffset);
				// use this if you want start positions to be arranged
                //phone->unitPosition.set(ofMap(i%10, 0,10,0.2,0.8), ofMap(floor(i/10.0),0,10,0.2,0.8));
                phone->unitPosition.set(0,0); 
				phone->updateWarpedPosition(warpMatrix);
				
				cout << "pos "<< phone->unitPosition <<"\n";
				
				if(broadcastingIDs) phone->broadcastID(); 

				
			} 
		}
	}
	
    // this part destroys phones that are no longer connected and updates 
    // the number of phones that are currently syncing and broadcasting
    int numBroadcasting = 0; 
    
	map<int,ConnectedPhone *>::iterator phoneit=connectedPhones.begin();
	int numSyncing = 0; 
    while( phoneit!=connectedPhones.end()){
		
		ConnectedPhone	* connectedPhone = phoneit->second; 
		connectedPhone->update(); 
		
		if(!connectedPhone->isConnected()){
			server.disconnectClient(connectedPhone->ID);
			cout << "disconnecting phone " << phoneit->first <<" " << connectedPhone->ID << "\n";

			connectedPhones.erase(phoneit++);
			//delete connectedPhone;
		} else {
			if(connectedPhone->syncStatus == SYNC_IN_PROGRESS) numSyncing++; 
            if(connectedPhone->isBroadcasting)  numBroadcasting++; 
            phoneit++;
        }

	}
	
    // if we're syncing then find new phones to sync
	if(syncing) { 
		phoneit=connectedPhones.begin();
		while(phoneit!=connectedPhones.end() && numSyncing<numConcurrentSyncs) { 
			if(phoneit->second->syncStatus == NOT_SYNCED) {
				phoneit->second->startSync(); 
				numSyncing++; 
			}
			phoneit++; 
		}
	}
    
    
    if(calibrating) {
        if((numBroadcasting==0) || (ofGetElapsedTimeMillis() - calibrationStart >8000)) { 
            
            phoneit=connectedPhones.begin();
            while(phoneit!=connectedPhones.end()) { 
                ConnectedPhone * phone = phoneit->second;
                
                phone->calibrationID+=500;
                phone->sendMsg("i"+ofToString(phone->calibrationID));
                phone->broadcastID(); 
                
                phoneit++; 
            }
            
            calibrationStart = ofGetElapsedTimeMillis();
            
            // should check to see if we've finished! 
            
        }
        
        
        
    }
	
}

void CommsManager::draw(ofRectangle* drawRect) { 
	
	ofSetHexColor(0xDDDDDD);
	ofDrawBitmapString("Socket server connect on port: "+ofToString(server.getPort()) + " num:"+ofToString(server.getNumClients())+" "+ofToString(server.webSocketClients.size()), 10, 20);

	//for each connected client lets get the data being sent and lets print it to the screen
	map<int,WebSocketClient>::iterator it;

	int counter = 0; 
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);

	ofPushMatrix(); 
	ofTranslate( drawRect->x, drawRect->y );
	if(showPositions && (posBrightness>0)) {
		map<int,ConnectedPhone*>::iterator phoneit;
		
		for(phoneit=connectedPhones.begin(); phoneit!=connectedPhones.end(); phoneit++){

			ConnectedPhone * phone = phoneit->second; 
			
			//if(!phone->found) continue; 
			
			phone->draw(posBrightness, drawRect->width, drawRect->height);
			
		}
	}	
	
	ofPopMatrix(); 
	ofDisableBlendMode(); 
	
	//
//	for(map<int,ConnectedPhone>::iterator it2=connectedPhones.begin(); it2!=connectedPhones.end(); it2++){
//		
//		ConnectedPhone	* pixelOutput = &it2->second; 
//		
//		ofSetColor(255,255,255); 
//		ofEllipse(pixelOutput->origLocation.x, pixelOutput->origLocation.y, 5, 5);
//		ofDrawBitmapString(ofToString(pixelOutput->ID), pixelOutput->origLocation.x, pixelOutput->origLocation.y);
//		if(pixelOutput->origLocation.x!=0) cout << pixelOutput->origLocation.x <<" "<<"\n";
//		if((broadcastingIDs)&&(!pixelOutput->broadcastingID)) pixelOutput->broadcastID(); 
//		§
//	}
	
	
}

void CommsManager::startBroadcastingIDs() { 
	//if(broadcastingIDs) return; 
	
	for(map<int, ConnectedPhone*>::iterator it=connectedPhones.begin(); it!=connectedPhones.end(); it++){
		ConnectedPhone	* connectedPhone = it->second; 
		if(!connectedPhone->found)  connectedPhone->broadcastID(); 
		
	}
	broadcastingIDs = true; 
}

void CommsManager::stopBroadcastingIDs(ConnectedPhone * exceptThisOne) { 
	//if(!broadcastingIDs) return; 
	
	for(map<int, ConnectedPhone*>::iterator it=connectedPhones.begin(); it!=connectedPhones.end(); it++){
		ConnectedPhone	* connectedPhone = it->second; 
		if(connectedPhone!=exceptThisOne) connectedPhone->stopBroadcastingID(); 
		
	}
	broadcastingIDs = false; 
}

void CommsManager::startCalibrating() { 
 
    calibrating = true; 
    calibrationStart = ofGetElapsedTimeMillis(); 
    
    int newNumBits = 0; 
    while ((pow(2.0f, (float)newNumBits) <= calibrationCount) || (pow(2.0f, (float)newNumBits) <= server.getLastID()))  { 
		newNumBits++;
    }
    setNumBits(newNumBits); 
    cout << "numBits : "<< newNumBits << "\n"; 
    
    for(map<int, ConnectedPhone*>::iterator it=connectedPhones.begin(); it!=connectedPhones.end(); it++){
		ConnectedPhone	* connectedPhone = it->second; 
        connectedPhone->calibrationID = connectedPhone->ID; 
		
	}

    sendAllPhones("b1");

    
}

void CommsManager::stopCalibrating() { 
    
    calibrating = false;   
    
    for(map<int, ConnectedPhone*>::iterator it=connectedPhones.begin(); it!=connectedPhones.end(); it++){
        ConnectedPhone * phone = it->second;
        
        phone->sendMsg("i"+ofToString(phone->ID));
        phone->stopBroadcastingID(); 
    
    }
    
    
}


void CommsManager::sendAllPhones(string msg) { 
	
	for(map<int, ConnectedPhone*>::iterator it=connectedPhones.begin(); it!=connectedPhones.end(); it++){
		ConnectedPhone* connectedPhone = it->second; 
		connectedPhone->sendMsg(msg); 
		
	}
}

void CommsManager::sendAllPhonesColour(ofColor col) { 
	for(map<int, ConnectedPhone *>::iterator it=connectedPhones.begin(); it!=connectedPhones.end(); it++){
		ConnectedPhone * connectedPhone = it->second; 
		connectedPhone->sendColour(col); 
		
	}
}

void CommsManager::setFrameRate(int rate, float doubletosingleratio, int blacktimeoffset) {
	
	phoneFrameRate = rate; 
    doubleToSingleRatio = doubletosingleratio; 
    blackTimeOffset = blacktimeoffset; 
	
	for(map<int, ConnectedPhone *>::iterator it=connectedPhones.begin(); it!=connectedPhones.end(); it++){
		ConnectedPhone	* connectedPhone = it->second; 
		connectedPhone->sendFrameRate(rate, doubleToSingleRatio, blackTimeOffset); 
		
	}		
	
}

void CommsManager::setNumBits(int numbits) {
	numBits = numbits; 
	for(map<int, ConnectedPhone *>::iterator it=connectedPhones.begin(); it!=connectedPhones.end(); it++){
		ConnectedPhone	* connectedPhone = it->second; 
		connectedPhone->sendNumBits(numbits);
		
	}		
	
}


void CommsManager:: resetPhones() { 
	
	for(map<int, ConnectedPhone *>::iterator phoneit=connectedPhones.begin(); phoneit!=connectedPhones.end(); phoneit++){
		
		ConnectedPhone * phone = phoneit->second; 
		phone->found = false; 
	
	}		
	
}


void CommsManager::foundPhones(vector <FoundPhone *> phones, int vidWidth, int vidHeight){
	for(int i =0; i<phones.size();i++) {
		FoundPhone * phone = phones[i]; 
		
		
		map<int,ConnectedPhone * >::iterator phoneit;
		
		for(phoneit=connectedPhones.begin(); phoneit!=connectedPhones.end(); phoneit++){
			
			ConnectedPhone * cphone = phoneit->second; 
			
			if(cphone->found) continue; 
			
			//cout << "Looking for " << phone->ID << " found " << cphone->ID << "\n";
			if(cphone->ID == phone->ID) {
				cphone->unitPosition.set(phone->unitPosition);
				//cphone->pixelPosition.set(cphone->unitPosition.x * ofGetWidth(), cphone->unitPosition.y * ofGetHeight()); 
				cphone->updateWarpedPosition(warpMatrix);
				
				cphone->stopBroadcastingID(); 
				cphone->found = true; 
		
				//cout<< "Found phone "<< cphone->ID << " " << phone->ID << " " << cphone->unitPosition.x << " " << cphone->unitPosition.y << "\n"; 
								
				
			}
		}
		
		
	}
	
}



void CommsManager :: updateWarpPoints( ofPoint points[4], int w, int h){
	
    
	for(int i=0; i<4;i++) { 
		sourcePoints[i] = points[i]; 
		//sourcePoints[i].z = 0; 
		unitSourcePoints[i].x = points[i].x/(float)w; 
		unitSourcePoints[i].y = points[i].y/(float)h; 
		unitSourcePoints[i].z = 0; 
        cout << "s: "<< unitSourcePoints[i] << " d: " << destPoints[i] << "\n ";
    
        
    }
	// NOW UPDATE MATRIX
	
   warpMatrix = findHomography(unitSourcePoints, destPoints);
	
    if(warpMatrix.isNaN()) {
        cout << "warpMatrix all fucked up :( \n" <<  unitSourcePoints << "\n" << destPoints << "\n";
        warpMatrix.makeIdentityMatrix();
        
    }
		
	
	map<int, ConnectedPhone*> :: iterator it = connectedPhones.begin();
	
	while(it!=connectedPhones.end()) { 
		
		ConnectedPhone * phone = it->second; 

		phone->updateWarpedPosition(warpMatrix);
        
		it++;
	}
		
}
		


void CommsManager::mousePressed(int x, int y, int button) { 
	if(button != 0) return; 
	cout << "left mouse "<< ofGetElapsedTimeMillis() << "\n";
	
	map<int, ConnectedPhone*> :: iterator it = connectedPhones.begin();
	
	while(it!=connectedPhones.end()) { 
		
		ConnectedPhone * phone = it->second; 
		
		if(phone->hitTest(x, y)) { 
			phone->startDragging(x, y); 
			it = connectedPhones.end(); 
			if(broadcastingIDs) {	
				stopBroadcastingIDs(phone);
				broadcastingIDs = true; // ouch!
			}
			//phone->found = false; 
			if(!phone->isBroadcasting) phone->broadcastID(); 
			
		} else { 	
			it++;
		}
	}
		
		
		
	
	
}
void CommsManager::mouseReleased(int x, int y, int button) { 
	
	map<int, ConnectedPhone *> :: iterator it = connectedPhones.begin();
	
	while(it!=connectedPhones.end()) { 
		
		ConnectedPhone * phone = it->second; 
		
		if(phone->isDragging) { 
			phone->stopDragging(); 
			if(broadcastingIDs) startBroadcastingIDs();

		}
		
		it++;
		
	}
		
}
