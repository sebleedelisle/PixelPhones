#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){
	
	
	guiSetup(); 

	
	phoneTracker.setupCamera(640,480);
	gui.control("warp").setSize(phoneTracker.vidWidth/phoneTracker.vidScale, phoneTracker.vidHeight/phoneTracker.vidScale);
	
	if(!commsManager.setup(portNum)) {
		portNum++;
		if(portNum>12000) portNum = 11995; 
	}
	// this default value should match the JS really... 
	//numBits = 4; 
	phoneRenderer.setup(&commsManager);
	
	ofBackground(0,0,0);
	toggleBroadcastIDs = false; 
	

	//ofSetVerticalSync(true);
	//ofEnableSmoothing();
	
	
}

//--------------------------------------------------------------
void testApp::update(){

	for(int i =0; i<4; i++) {
		if(warpPoints[i]!=commsManager.sourcePoints[i]) { 
			//cout << "updating warp points \n";
			commsManager.updateWarpPoints(warpPoints, phoneTracker.cvColour.getWidth(), phoneTracker.cvColour.getHeight()); 
			 
			break; 
		}
	}
	
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
	if( (phoneFrameRate!=commsManager.phoneFrameRate) || 
        (doubleToSingleRatio!=commsManager.doubleToSingleRatio) || 
        (blackTimeOffset!=commsManager.blackTimeOffset)) {
        
		commsManager.setFrameRate(phoneFrameRate, doubleToSingleRatio, blackTimeOffset);
		phoneTracker.phoneFrameRate = phoneFrameRate; 
        phoneTracker.doubleToSingleRatio = doubleToSingleRatio;
	
    }
	
	phoneRenderer.update(); 
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	phoneTracker.draw();
	commsManager.draw(phoneTracker.vidWidth, phoneTracker.vidHeight); 	
	phoneRenderer.draw(); 
	
	int framerate = ofGetFrameRate();
	if(framerate<60) { 
		ofSetColor(255,ofMap(framerate, 30, 60, 0,255, true),0); 
		ofFill();
		ofRect(0,0,10,10); 
				   
	}
		
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
    gui.addContent("Difference blend", phoneTracker.cvDiffBlended,200);
	gui.addSlider("differenceThreshold", phoneTracker.differenceThreshold, 0, 255).setNewColumn(true).setSize(200,30);
	gui.addSlider("position brightness", commsManager.posBrightness,0,255).setSize(200,30); 
	gui.addToggle("showPositions P", commsManager.showPositions).setSize(200,30); 
	gui.addToggle("showTrails T", phoneTracker.showTrails).setSize(200,30); 
	gui.addToggle("broadcast IDs I", toggleBroadcastIDs).setNewColumn(true).setSize(200,30);
		
	//ofxSimpleGuiContent * cam = (ofxSimpleGuiContent * )&
	//gui.addContent("Camera feed", phoneTracker.cvColour, 220).setNewColumn(true);
	
	gui.addToggle("Recording Video", phoneTracker.recording).setSize(200,30); 
		
	gui.addToggle("Flip X", phoneTracker.flipX).setSize(200,30); 
	gui.addToggle("Flip Y", phoneTracker.flipY).setSize(200,30); 
	
	gui.addSlider("phoneFrameRate", phoneFrameRate, 1, 30).setNewColumn(true).setSize(200,30);
    gui.addSlider("blackToWhiteRatio", doubleToSingleRatio, 2, 6).setSize(200,30); 
    gui.addSlider("blackTimeOffset", blackTimeOffset, 0, 200).setSize(200,30); 
    gui.addSlider("diffSkipFrames", phoneTracker.diffSkipFrames, 1, 10).setSize(200,30); 
    
	gui.addSlider("minBlobSize", phoneTracker.minBlobSize, 0, 255).setSize(200,30);
	gui.addSlider("maxBlobSize", phoneTracker.maxBlobSize, 1, 500).setSize(200,30);
	gui.addSlider("videoFrameRate", phoneTracker.videoFrameRate, 0, 100).setSize(200,30);
	gui.addSlider("b w threshold", phoneTracker.bwthreshold, 0, 1).setSize(200,30);
	gui.addSlider("track distance", phoneTracker.trackDistance, 0, 40).setSize(200,30);

	gui.addPage("screenwarp"); 
	gui.addQuadWarper("warp", phoneTracker.cvColour, warpPoints);
	gui.addToggle("syncing", commsManager.syncing).setSize(200,30);
	
	gui.addPage("PhoneRenderer"); 

	gui.addToggle("syncing S", commsManager.syncing).setSize(200,30);
	gui.addSlider("concurrent syncs", commsManager.numConcurrentSyncs, 1, 200).setSize(200,30);
	
	phoneRenderer.initGui(&gui); 
	
	gui.loadFromXML();

	phoneRenderer.nyanCatch->gameStartSwitch = false; 
	phoneRenderer.startProgramSwitch = false; 
	phoneTracker.recording = false; 
	
	gui.show();
    
    commsManager.updateWarpPoints(warpPoints, phoneTracker.cvColour.getWidth(), phoneTracker.cvColour.getHeight()); 

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
			//case 'p': gui.nextPageWithBlank(); break;
			case 'i':
				toggleBroadcastIDs = !toggleBroadcastIDs; 
				break; 
			case 's':
				commsManager.syncing = !commsManager.syncing; 
				break; 
            case 'p':
				commsManager.showPositions = !commsManager.showPositions; 
				break; 
            case 't':
				phoneTracker.showTrails = !phoneTracker.showTrails; 
				break;     
			case 'v': 

				phoneTracker.cameraManager.videoSettings(); 
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

	float xf = (float)(x - (ofGetWidth()/2 + phoneTracker.vidWidth/2)) / (float)phoneTracker.vidWidth; 
	float yf = (float)(y - (ofGetHeight()/2 + phoneTracker.vidHeight/2)) / (float)phoneTracker.vidHeight; 
	cout << xf << " " << yf << "\n";
	if(!gui.isOn()) commsManager.mousePressed(x,y,button); 
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
	float xf = (float)(x - (ofGetWidth()/2 + phoneTracker.vidWidth/2)) / (float)phoneTracker.vidWidth; 
	float yf = (float)(x - (ofGetWidth()/2 + phoneTracker.vidWidth/2)) / (float)phoneTracker.vidWidth; 
	
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
	if(commsManager.TCP.close()) { 
        cout << "server closed\n";
    } else { 
        cout << "server failed to close :( \n";
        
    }
	
	//phoneTracker.vidGrabber.close(); 
	//phoneTracker.libdcCamera.close(); 
	
	//cameraManager.close(); 
	phoneTracker.cameraManager.close(); 
	
	if(phoneTracker.trackingDebugDataLine>0) {
		ofPixels pixels;
		phoneTracker.trackingDebugData.getTextureReference().readToPixels(pixels);
		
		ofSaveImage(pixels,"testdata/"+ofGetTimestampString()+".png"); 
	}

}