#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){
	
	
	guiSetup(); 
	
	phoneTracker.setupCamera(640,480);
	if(!commsManager.setup(portNum)) {
		portNum++;
		if(portNum>12000) portNum = 11995; 
	}
	// this default value should match the JS really... 
	//numBits = 4; 
	phoneRenderer.setup(&commsManager);
	
	ofBackground(0,0,0);
	toggleBroadcastIDs = false; 
	
}

//--------------------------------------------------------------
void testApp::update(){

	//if(vidReset) {
//		phoneTracker.setupCamera(160*vidScale,120*vidScale);
//		vidReset = false;
//	}	
	//bit nasty - we pass through a boolean if we're currently broadcasting ids
	vector <FoundPhone *> foundPhones = phoneTracker.update(commsManager.broadcastingIDs);
	if(foundPhones.size()>0) {
		commsManager.foundPhones(foundPhones, phoneTracker.vidWidth, phoneTracker.vidHeight); 
	
	}
	
	if(toggleBroadcastIDs!=commsManager.broadcastingIDs) { 
		if(toggleBroadcastIDs) commsManager.startBroadcastingIDs(); 
		else commsManager.stopBroadcastingIDs();
	}
	commsManager.update(); 	
	
	// if we have so many phones that we need more bits, 
	// let's change the number!
	//cout << pow(2.0f, (float)numBits) << "\n";
	numBits = numBitsMin; 
	
	while (pow(2.0f, (float)numBits) <= commsManager.TCP.getLastID()) { 
		numBits++;

	}
	
	if(commsManager.numBits!=numBits) { 
		commsManager.setNumBits(numBits);
		phoneTracker.numBits = numBits; 
	}
	if(phoneFrameRate!=commsManager.frameRate){
		commsManager.setFrameRate(phoneFrameRate);
		phoneTracker.phoneFrameRate = phoneFrameRate; 
	}
	
	phoneRenderer.update(); 
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	phoneTracker.draw();
	commsManager.draw(); 	
	phoneRenderer.draw(); 
	gui.draw(); 
	
	
}


void testApp::guiSetup() {
	gui.addSlider("Port number", portNum, 11995, 12000); 
	gui.addFPSCounter(); 
	
	//gui.addSlider("videoScale", vidScale, 1,4);
	//gui.addButton("resetVideo", vidReset);
	gui.addSlider("numBitsMin", numBitsMin, 4, 12);
	gui.addSlider("numBits", numBits, 4, 12);
	gui.addToggle("updateVideo", phoneTracker.updateVideo);
	gui.addToggle("updateDiff", phoneTracker.updateDiff);
	
	gui.addPage("PhoneTracking");
	
	gui.addContent("Difference feed", phoneTracker.cvDiff,200);
	gui.addSlider("differenceThreshold", phoneTracker.differenceThreshold, 0, 255).setNewColumn(true).setSize(200,30);
	gui.addSlider("position brightness", commsManager.posBrightness,0,255).setSize(200,30); 
	gui.addToggle("BROADCAST IDs", toggleBroadcastIDs).setNewColumn(true).setSize(200,30);
		
	//ofxSimpleGuiContent * cam = (ofxSimpleGuiContent * )&
	//gui.addContent("Camera feed", phoneTracker.cvColour, 220).setNewColumn(true);
		
	gui.addToggle("Flip X", phoneTracker.flipX).setSize(200,30); 
	gui.addToggle("Flip Y", phoneTracker.flipY).setSize(200,30); 
	
	gui.addSlider("phoneFrameRate", phoneFrameRate, 1, 30).setNewColumn(true).setSize(200,30);
	gui.addSlider("minBlobSize", phoneTracker.minBlobSize, 0, 255).setSize(200,30);
	gui.addSlider("maxBlobSize", phoneTracker.maxBlobSize, 1, 500).setSize(200,30);
	gui.addSlider("videoFrameRate", phoneTracker.videoFrameRate, 0, 100).setSize(200,30);
	gui.addSlider("b w threshold", phoneTracker.bwthreshold, 0, 1).setSize(200,30);
	gui.addSlider("track distance", phoneTracker.trackDistance, 0, 40).setSize(200,30);
	
	gui.addPage("PhoneRenderer"); 

	gui.addToggle("syncing", commsManager.syncing).setSize(200,30);
	gui.addSlider("concurrent syncs", commsManager.numConcurrentSyncs, 1, 200).setSize(200,30);
	
	phoneRenderer.initGui(&gui); 
	
	gui.loadFromXML();

	phoneRenderer.nyanCatch->gameStartSwitch = false; 
	phoneRenderer.startProgramSwitch = false; 
	
	gui.show();

}






//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if(key>='0' && key<='9') {
		gui.setPage(key - '0');
		gui.show();
	} else {
		switch(key) {
			case ' ': gui.toggleDraw(); break;
			case '[': gui.prevPage(); break;
			case ']': gui.nextPage(); break;
			case 'p': gui.nextPageWithBlank(); break;
			case 'i':
				toggleBroadcastIDs = !toggleBroadcastIDs; 
				break; 
			case 'v': 
				phoneTracker.vidGrabber.videoSettings(); 
				break;
				
			case 'r': 
				commsManager.resetPhones(); 
				break;
//				
			//default:
//				ofColor col = ofColor();
//				col.setHsb(ofRandom(255), 255, 255);
//				commsManager.sendAllPhonesColour(col);
//				break; 	
				
		}
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	if(!gui.isOn()) commsManager.mousePressed(x,y,button); 
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
	commsManager.mouseReleased(x,y,button); 
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::exit() { 
	commsManager.TCP.close(); 
	phoneTracker.vidGrabber.close(); 


}