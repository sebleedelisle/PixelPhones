/*
 *  TrackedBlobData.cpp
 *  PixelPhones
 *
 *  Created by Seb Lee-Delisle on 14/03/2012.
 *  Copyright 2012 seb.ly. All rights reserved.
 *
 */

#include "TrackedBlobData.h"

bool compareFloats (float i,float j) { return (i<j); }

void TrackedBlobData::updateGreys() { 
	
	
	//vector <float> brightnesses; 
	
	int minBrightness = 255*3, 
	maxBrightness = 0; 
	
	for (int i = 0; i<colours.size(); i++) {
		
		int brightness = colours[i].getLightness(); 
		
		if(brightness<minBrightness) minBrightness = brightness; 
		if(brightness>maxBrightness) maxBrightness = brightness; 
		if(greys.size()>i) greys[i] = brightness; 
		else greys.push_back(brightness); 
		
	}
	
	// now normalise the brightnesses so they're all values between 0 and 1 : 
	
//	cout << "normalised brightnesses : \n";
	
	for(int i = 0; i<greys.size(); i++) {
		//normalise the brightness
		greys[i] = ofMap(greys[i], minBrightness, maxBrightness, 0, 1);
		//(greys[i] - minBrightness) /( maxBrightness - minBrightness) ; 
		
	//	cout << greys[i] << " ";
		
	}
	//cout << "\n\n";
	
	
	
}

int TrackedBlobData::getID(float bwthreshold) {
	
	// first normalise the colours by getting max and min brightness
	
	updateGreys();
	// now work out an array of lengths... 
	pulseLengths.clear(); 
	
	
	//vector <IDPulseData *> pulseLengths; 
	// should always have a zero bit at first - this object is discarded though
	IDPulseData * pulseData = new IDPulseData(); 
	pulseData->bit = 0; 
	
	// go through and calculate the length of each bit
	for (int i = 0 ; i<greys.size(); i++) { 
		
		float brightness = greys[i]; 
		float time = times[i]; 
		
		// if same as current colour then add 1
		if((brightness<bwthreshold) == (pulseData->isBlack())) { 
			
			pulseData->length+=time; 
			
		} else { // if different then make new colData
			
			IDPulseData * newpulsedata = new IDPulseData(); 
			newpulsedata->bit = !pulseData->bit; 
			
			// add proportion to last colour data, 
			newpulsedata->length += (newpulsedata->isBlack() ? (1-brightness) : brightness)*time; 
			pulseData->length += (pulseData->isBlack() ? (1-brightness) : brightness)*time; 
			
			
			// add colour data to array
			pulseData = newpulsedata; 
			pulseLengths.push_back(pulseData); 
			
		}
		
	}
	
	// if we have no colours, return! 
	// TODO should make this kinda roughly the number of pixels we should 
	// expect for the frame rate and the number of bits
	if(pulseLengths.size()<numBits*2) return -1; 
	
	// and if the last colour is black (it should be) get rid of it. 
	if(pulseLengths.back()->isBlack()) pulseLengths.pop_back(); 
	
	
	// make two vectors - one for all the zeros' lengths and one for all the ones' lengths
	vector <float> zeroLengths; 
	vector <float> oneLengths; 
	
	
	for(int i = 0; i<pulseLengths.size(); i++) {
		
		pulseData = pulseLengths[i]; 
		
        // which list do we use? 
		vector <float> * vec = (pulseData->isBlack()) ? &zeroLengths : &oneLengths; 
		
        // NOTE! WE CAN'T DO THAT ANYMORE OR THE CLEVER FAST OUT WON'T WORK... 
		// if we don't already have that number, add it into the vector
		//if(std::find(vec->begin(), vec->end(), pulseData->length) == vec->end()) {
		//}
    
        vec->push_back(pulseData->length); 
		
        
	}
	
	// now sort them! 
	sort(zeroLengths.begin(), zeroLengths.end(), compareFloats);
	sort(oneLengths.begin(), oneLengths.end(), compareFloats);
	

	// we should now have two vectors, one for each colour. 
	// so now use each length of each vector as a threshold 
	float zeroThreshold = 0; 
	float oneThreshold = 0; 
    float zeroConfidence = 0; 
    float oneConfidence = 0; 
	
	int resultCode = -1; 
	
#ifdef USECHECKSUM	
	// we're expecting twice the number of bits (manchester encoded ID) plus
	// half the numBits *2 for the checksum, plus 2 at the front ant 2 at the back
	int totalBitCount = (numBits*2) + (ceil((float)numBits/2.0f)*2) + 4; 
#else
	int totalBitCount = (numBits*4) + 4; 
	
#endif
    
    // an array to store the solution in
    unsigned long long solution = 0; 
    int bitIndex = 0; 
    //cout << "-----------------------------------------------------\n";
    int solutioncount = 0; 
    float lastConfidenceLevel = -1; 
	//iterate through all the zeroLengths
	for(int i = 0 ; i<=zeroLengths.size(); i++) { 
		
		zeroThreshold = (i<zeroLengths.size()) ? zeroLengths[i] : FLT_MAX; 
        zeroConfidence = ((i>0)&&(i<zeroLengths.size())) ? zeroLengths[i]-zeroLengths[i-1] : 0; 
        
		
		// and the one lengths
		for(int j = 0; j<=oneLengths.size(); j++) { 
			
			oneThreshold = (j<oneLengths.size())? oneLengths[j] : FLT_MAX; 
            oneConfidence = ((j>0)&&(j<oneLengths.size())) ? oneLengths[j]-oneLengths[j-1] : 0; 
        	
            // SUPER CLEVER FAST OUT! 
			int bitlength = (i) + ((zeroLengths.size()-i)*2) + (j) + ((oneLengths.size()-j)*2); 
			if(bitlength!=totalBitCount) continue; 
            
            
			
			
			// and now work out a solution for those two thresholds... 
			string solutionstring (""); 
            solutionstring.reserve(totalBitCount); 
            
            // ABANDONED ATTEMPT AT BINARY VERSION... 
			//solution = 0; 
            //bitIndex = totalBitCount;
            
			for(int k = 0; k<pulseLengths.size(); k++) {
				
				IDPulseData * pulsedata = pulseLengths[k]; 
				
				float threshold = pulsedata->bit ? oneThreshold : zeroThreshold; 
				
                /*cout << bitIndex << " " << pulseData->bit << " "; 
                
				if(pulseData->bit) solution |= 1 << bitIndex;
                bitIndex--;
                    
                
                if(pulsedata->length >= threshold) {
                    if(pulseData->bit) solution |= 1 << bitIndex;
                    bitIndex--; 
                }
                cout << convBase(solution,2)+"\n";
                */
                
                
                string digitchar = pulsedata->bit ? "1":"0";  
                solutionstring+= (pulsedata->length < threshold) ? digitchar : digitchar+digitchar; 
				
			}	
			
			int decoded = decodeManchesterString(&solutionstring);
			
            //cout <<  i<< " " << j << " "<< solution << " " << ((decoded>-1) ? "<<<<<<<<<<<<<<<<<<<<<< " : " ") << decoded << "\n";
			
			// TODO : Add confidence and make array of solutions. 
			if(decoded>-1) {
                float confidence = zeroConfidence * oneConfidence;
                
				if(confidence > lastConfidenceLevel) {
                    resultCode = decoded;
                    lastConfidenceLevel = confidence;
                }
                
                cout << solutioncount << " " << zeroConfidence <<" "<< oneConfidence <<" "<< solutionstring << " " << decoded << "\n";
                
                solutioncount++; 
//				if(bitlength!=totalBitCount) { 
//					cout << "bitlength = " << bitlength << " string length = " << solution.size() << " totalBitCount "<< totalBitCount << "\n"; 
//					cout <<  i<< " " << j << " "<< zeroLengths.size() << " " << oneLengths.size() << "\n";
//					cout <<  i<< " " << j << " "<< solution << " " << ((decoded>-1) ? "<<<<<<<<<<<<<<<<<<<<<< " : " ") << decoded << "\n";
//				}
			}
			
		}
		
		
	}	return resultCode; 
	
	
	
}


int TrackedBlobData::decodeManchesterString(string * code) { 
	
	string str = *code;
	
	bool bit = true; 
	bool count = 0; 
	
    // this function assumes that it's the right number of bits that we're looking for
    // all data cleaning should have been done before now. 

	vector<int> decoded; 
	
	for(int i = 0 ; i<str.size(); i+=2) { 
		
		int bit1 = (str[i]=='0')? 0 : 1; 
		int bit2 = (str[i+1]=='0')? 0 : 1; 
		
		// two bits the same! not a valid pair
		if((bit1^bit2) == 0)  return -1; 
		
		decoded.push_back( bit1); 
	}
    
	// first digit must be 1!
	int firstBit = BLACKGAPS ? 1 : 0; 
	
	
	if(decoded[0]!=firstBit) return -1; 
	// last digit must be 0
	if(decoded[decoded.size()-1] == firstBit ) return -1; 
		
	
	// now delete first and last digits - we don't need them
	decoded.erase(decoded.begin(), decoded.begin() + 1); 
	decoded.erase(decoded.end()-1, decoded.end()); 
	
	// this is the number of bits in the final number
#ifdef USECHECKSUM
    
	int bitcount = floor(decoded.size()/1.5); 

	// make sure digits match checksum
    
	for(int i=0; i<bitcount; i+=2) {
		if(i+1<bitcount) {
			int checkdigit = (decoded[i]==decoded[i+1]) ? 0 : 1; 
			if(decoded[bitcount + (i/2)] != checkdigit) return -1;
		} else {
			if(decoded[i] != decoded[bitcount + (i/2)]) return -1; 
		}
		
	}

#else
	int bitcount = (decoded.size()/2); 
	// make sure digits are same both times
	for(int i=0; i<bitcount; i++) {
		if(decoded[i] != decoded[bitcount + i]) return -1; 
		
	}

#endif
	

	int num = 0; 
	
	for(int i = 0; i< bitcount ; i++) {
		int bit = decoded[i]; 
		
		num |= (bit<<(bitcount-1-i)); 
		
	
		
	}
	
	return num;
	
}

void TrackedBlobData::reset() { 

	colours.clear(); 
	greys.clear(); 
	times.clear(); 


}


string TrackedBlobData::convBase(unsigned long long v, long base)
{
	string digits = "0123456789abcdef";
	string result;
	if((base < 2) || (base > 16)) {
		result = "Error: base out of range.";
	}
	else {
		do {
			result = digits[v % base] + result;
			v /= base;
		}
		while(v);
	}
	return result;
}
