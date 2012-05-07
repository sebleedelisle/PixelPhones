#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"
#include "ofxSimpleGuiToo.h"
#include "ofxSimpleGuiContent.h"
#include "TrackedBlob.h"
#include "PhoneTracker.h"
#include "CommsManager.h"
#include "PhoneRenderer.h"
#include "CameraManager.h"

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);	
	
		void guiSetup() ;
	

	ofxSimpleGuiToo gui; 
	
	int portNum; 
	bool vidReset;
	bool toggleBroadcastIDs; 
	int vidScale;
	int phoneFrameRate; 
	int numBits; 
	int numBitsMin; 
    float doubleToSingleRatio; 
    int blackTimeOffset; 
    bool calibrating; 
    
    ofRectangle drawRect; 
	
	ofPoint warpPoints[4];

	
	PhoneTracker phoneTracker;
	CommsManager commsManager; 
	PhoneRenderer phoneRenderer; 

	
	
	
	
};
