/*
 *  TrackedBlob.cpp
 *  PixelPhonesV1
 *
 *  Created by Seb Lee-Delisle on 05/06/2011.
 *  Copyright 2011 seb.ly. All rights reserved.
 *
 */

#include "TrackedBlob.h"


TrackedBlob::TrackedBlob() {
	
	reset(); 
	lifeExpectancy = 10; 
	colour.setHsb(ofRandom(150, 180), ofRandom(255), 255, 255);
	blackGaps = true; 
	
}
void TrackedBlob::update(ofColor col, int gapNumFrames, int numbits, vector<ofPoint *> * otherPoints) {
	if(!enabled) return; 
	lifeExpectancy = gapNumFrames; 
	numBits = numbits; 
	counter ++; 
	
	if(counter-lastUpdated>lifeExpectancy) enabled = false; 
	else {
		colours.push_back(col); 
	}
	
	labelCentre.set(centroid);
	
	for(int i =0; i<otherPoints->size(); i++) { 
		ofPoint * otherPoint  = otherPoints->at(i);
		
		float diffx = fabsf(otherPoint->x - labelCentre.x);
		float diffy = fabsf(otherPoint->y - labelCentre.y);
		if(diffx<20 && diffy<2) {
			// if it's nearer than 6 pixels... 
			labelCentre.y +=2; // -= (diffx * 0.1);
			//labelCentre.y -= (diffy * 0.1);
			
		}

		
	}

	otherPoints->push_back(&labelCentre); 
	
}

void TrackedBlob::draw(float bwthreshold) { 
	if(!enabled) return; 
	ofNoFill(); 
	ofSetLineWidth(1);
	ofSetColor(colour);
	
	ofRect(boundingRect); 
	ofSetLineWidth(1);
	//ofRect( centroid.x-1,  centroid.y-1,3,3);
	
	ofLine(centroid.x, centroid.y, labelCentre.x, labelCentre.y);
	
	ofFill();
	
	ofColor col; 
	
	//updateGreys(); 
	
	for(int i=0; i<greys.size(); i++) {
		
		col.set(greys[i]<bwthreshold ? 0 : 255 ); 
		
		ofSetColor(col); 
		
		ofRect(labelCentre.x+(i*1), labelCentre.y, 1, 1);
		//ofVertex(labelCentre.x+(i*1),labelCentre.y);
		
	}
	
}
void TrackedBlob::reset() {
	
	enabled = true; 
	counter =0; 
	lastUpdated = 0; 
	colours.clear(); 
	greys.clear(); 
	
	
}


void TrackedBlob::updateGreys() { 
	
	
	//vector <float> brightnesses; 
	
	float minBrightness = 1, maxBrightness = 0; 
	for (int i = 0; i<colours.size(); i++) {
		
		float brightness = colours[i].getLightness(); 
		
		if(brightness<minBrightness) minBrightness = brightness; 
		if(brightness>maxBrightness) maxBrightness = brightness; 
		if(greys.size()>i) greys[i] = brightness; 
		else greys.push_back(brightness); 
		
	}
	
	// now normalise the brightnesses so they're all values between 0 and 1 : 
	
	//cout << "normalised brightnesses : \n";
	
	for(int i = 0; i<greys.size(); i++) {
		//normalise the brightness
		greys[i] = (greys[i] - minBrightness) /( maxBrightness - minBrightness) ; 
		
		//cout << brightnesses[i] << " ";
		
	}
	//cout << "\n\n";
	
	
	
}

int TrackedBlob::getID(float bwthreshold) {
	
	// first normalise the colours by getting max and min brightness
	
	updateGreys();
	// now work out an array of lengths... 
	
	vector <IDPulseData *> colLengths; 
	
	// should always have a zero bit at first - this object is discarded though
	IDPulseData * pulseData = new IDPulseData(); 
	pulseData->bit = 0; 
	
	// go through and calculate the length of each bit
	for (int i = 0 ; i<greys.size(); i++) { 
	
		float brightness = greys[i]; 
		
		// if same as current colour then add 1
		if((brightness<bwthreshold) == (pulseData->isBlack())) { 
			
			pulseData->length++; 
			
		} else { // if different then make new colData
		
			IDPulseData * newpulsedata = new IDPulseData(); 
			newpulsedata->bit = !pulseData->bit; 
		
			// add proportion to last colour data, 
			//if(fabs(brightness-0.5)<0.25) {
				
				newpulsedata->length += newpulsedata->isBlack() ? (1-brightness) : brightness; 
				pulseData->length += pulseData->isBlack() ? (1-brightness) : brightness; 
				
				
			//} else { //otherwise just add 1 to new colour data
			
			//	newcoldata->length+=1;
			//}
			
			//cout << (colData->isBlack ? "Black : " : "White : ") << colData->length << "\n";
			// add colour data to array
			pulseData = newpulsedata; 
			colLengths.push_back(pulseData); 
		
		}
		
	}
	
	// if we have no colours, return! 
	// TODO should make this kinda roughly the number of pixels we should 
	// expect for the frame rate and the number of bits
	//if(colLengths.size()<10) return -1; 
	
	// and if the last colour is black (it should be) get rid of it. 
	if(colLengths.back()->isBlack()) colLengths.pop_back(); 

	
	// make two arrays - one for all the zeros' lengths and one for all the ones' lengths
	vector <float> zeroLengths; 
	vector <float> oneLengths; 
	
	
	for(int i = 0; i<colLengths.size(); i++) {
		
		pulseData = colLengths[i]; 
		
		//cout << i <<" isBlack : " << pulseData->isBlack() << " len " << pulseData->length << "\n";
		//if(pulseData->isBlack()) zeroLengths.push_back(pulseData->length); 
		//else oneLengths.push_back(pulseData->length); 
		
		// which list do we use? 
		vector <float> * vec = (pulseData->isBlack()) ? &zeroLengths : &oneLengths; 
		
		// if we don't already have that number, add it into the vector
		//if(std::find(vec->begin(), vec->end(), pulseData->length) == vec->end()) {
			vec->push_back(pulseData->length); 
		//}
		
	}
	
	// add 0 to each vector, just in case they are all doubles (which probably shouldn't happen)
	zeroLengths.push_back(0); 
	oneLengths.push_back(0); 
	
	
	// we should now have two vectors, one for each colour. 
	// so now use each length of each vector as a threshold 
	float zeroThreshold = 0; 
	float oneThreshold = 0; 
	
	int resultCode = -1; 
	
	for(int i = 0 ; i<zeroLengths.size(); i++) { 
		
		zeroThreshold = zeroLengths[i]; 
		
		for(int j = 0; j<oneLengths.size(); j++) { 
			
			oneThreshold = oneLengths[j]; 
			
			
			string solution (""); 
			
			for(int k = 0; k<colLengths.size(); k++) {
				
				IDPulseData * coldata = colLengths[k]; 
				
				float threshold = coldata->isBlack() ? zeroThreshold : oneThreshold; 
				string digitchar = ofToString(coldata->bit); 
				
				solution += (coldata->length <= threshold) ? digitchar : digitchar+digitchar; 
				
				
			}	
			
			int decoded = decodeManchesterString(&solution);
			//cout <<  i<< " " << j << " "<< solution << " " << ((decoded>-1) ? "<<<<<<<<<<<<<<<<<<<<<< " : " ") << decoded << "\n";
			
			if(decoded>-1) resultCode = decoded;
			
		}
		
		
	}	return resultCode; 
	

	
}


int TrackedBlob::decodeManchesterString(string * code) { 
	
	string str = *code;
	
	bool bit = true; 
	bool count = 0; 
	
	//cout << str; 
	
	// get rid of the 1s at the end of the string (the white padding)
	char padChar = blackGaps ? '0' : '1'; 
	while((str.size()>0) && (str[str.size()-1]==padChar))
		str.erase(str.size()-1,1); 
	
	// and if there's any superfluous 1s at the beginning get rid of those too. 
	while((str.size()>0) && (str[0]==padChar))
		str.erase(0,1); 
	
	
	// code too short - should place the actual number of bits we're looking for here
	if(str.size()!= (((numBits*2) + 2)*2)) {// != (2 + numBits*2) * 2 ) {
		//cout << "code too short";
		return -1; 
	}
	// odd number of digits
	if(str.size()%2==1) {
		//cout << "odd number of digits";
		return -1; 
	}
	vector<int> decoded; 
	
	for(int i = 0 ; i<str.size(); i+=2) { 
		
		int bit1 = (str[i]=='0')? 0 : 1; 
		int bit2 = (str[i+1]=='0')? 0 : 1; 
		
		// two bits the same! not a valid pair
		if(bit1^bit2 == 0)  return -1; 
		
		decoded.push_back( bit1); 
		
		
	}
	
	// first digit must be off!
	int firstBit = blackGaps ? 1 : 0; 
	
	
	if(decoded[0]!=firstBit) return -1; 
	// last digit must be 1
	if(decoded[decoded.size()-1] == firstBit ) return -1; 
	
	// if it's not an even number of bits then we're busted!
	if(decoded.size()%2!=0) return -1; 
	
	
	// now delete first and last digits - we don't need them
	decoded.erase(decoded.begin(), decoded.begin() + 1); 
	decoded.erase(decoded.end()-1, decoded.end()); 
	
	
	int halfsize = decoded.size()/2; 
	
	// make sure digits are same both times
	for(int i=0; i<halfsize; i++) {
		if(decoded[i]!=decoded[i+halfsize]) return -1; 
		
		
	}
	
	//cout << str <<"\n";
	
	int num = 0; 
	
	for(int i = 0; i< halfsize ; i++) {
		int bit = decoded[i]; 
		
		num |= (bit<<(halfsize-1-i)); 
		
		//printf("%d", bit); 
		
	}
	//printf("\n"); 
	//printf("%d \n", num); 
	//cout << str << " "<< str.size() <<"\n"; 
	
	return num; 
	
	
	//	cout << str[str.size()-1] << " "<<  (str[str.size()-1] =='0'); 
	
	
	
	
}



bool TrackedBlob::vectorContainsInt(vector <int> * vec, int num) { 
	for(int i = 0; i<vec->size(); i++) {
		if((*vec)[i] == num) return true; 
		
	}
	return false; 
	
}