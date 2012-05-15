#include "WebSocketClient.h"
#include "ofAppRunner.h"
#include "ofxNetworkUtils.h"

//--------------------------
WebSocketClient::WebSocketClient(){

	verbose		= true;
	connected	= false;
	messageSize = 0;
	port		= 0;
	index		= -1;
	str			= "";
	tmpStr		= "";
	ipAddr		="000.000.000.000";

	partialPrevMsg = "";
	
	// Websocket protocol requires a null char to begin and 
	// a 255 char at the end. 
	
	messageDelimiter = '\xff';
	messagePrefix = '\0'; 


	memset(tmpBuff,  0, TCP_MAX_MSG_SIZE+1);
}


bool WebSocketClient::setup(int _index, bool blocking){
	handshakeComplete = false; 

	return ofxTCPClient::setup(_index, blocking); 

}
bool WebSocketClient::send(string message) {
	
	// tcp is a stream oriented protocol
	// so there's no way to be sure were
	// a message ends without using a terminator
	// note that you will receive a trailing [/TCP]\0
	// if sending from here and receiving from receiveRaw or
	// other applications
	if(!connected){
		if(verbose)printf("WebSocketClient: trying to send while not connected\n");
		return false;
	}
	
	message = partialPrevMsg + messagePrefix + message + messageDelimiter;
	
	// this is uncommented in the original - testing without it
	//message += (char)0; //for flash
	
	
	int ret = TCPClient.SendAll( message.c_str(), message.length() );
	if( ret == 0 ){
		if(verbose)printf("WebSocketClient: other side disconnected\n");
		close();
		return false;
	}else if(ret<0){
		if(verbose)printf("WebSocketClient: send() failed\n");
		return false;
	}else if(ret<(int)message.length()){
		// in case of partial send, store the
		// part that hasn't been sent and send
		// with the next message to not corrupt
		// next messages
		partialPrevMsg=message.substr(ret);
		return true;
	}else{
		partialPrevMsg = "";
		return true;
	}	
	
	
}


//--------------------------
string WebSocketClient::receive(){
	
	str = ofxTCPClient::receive(); 
	

	if(handshakeComplete){
		return str; 
	}
	
	
	str = tmpStr; 
	if( str.size()>0){
		
		//char lastchar = str[(str.size()-1)];
		//cout << "last char : " << lastchar << "\n";
		//cout << "header : --------------------------------------\n" << str << "\n--------------------------------\n";
		
		
		// this is a flash socket pretending to be a websocket :) 
		if(str.find("<policy-file-request/>")!=string::npos) {
			sendRaw("<cross-domain-policy>\r\n<allow-access-from domain=\"*\" to-ports=\"*\" />\r\n</cross-domain-policy>\r\n"+ofToString(char(0x00))+ofToString(char(0xff)));
			
			//receiveString = "";
		} else {
			
			
			
			string response = procClientHeader(str, serverPortNum);
			
			if(response!=""){
				//tcpClient->sendRaw(ofToString((char)0));
				sendRawBytes(response.c_str(), response.length());
				
				
				//tmpStr = ""; 
				sendRaw(ofToString((char)0xff));
				tmpStr = ""; 
				//ofToString((char)0) + msg + ofToString((char)0xff)
				//cout << "sending response : ---------------------------\n"<<response<< "\n---------------------------\n";
				handshakeComplete= true; 
				
			}
		} 
	 
	}
	
	
	
	return str;
	
}


string WebSocketClient::procClientHeader(string header, int port){
	
	vector<string> data = ofSplitString(header, "\n", true, true);
	
	
	long longnum1 = -1;
	long longnum2 = -1;
	long key1 = -1;
	long key2 = -1;
	string h_upgrade, h_connection, h_origin;
	
	string key3;
	
	for (int i = 0 ; i<data.size(); i++) {
		string str = data[i]; 
		if(str.find("Sec-WebSocket-Key1:")!=string::npos){
			//cout << "key1 uncoded : " << str.substr(20) <<"\n"; 
			key1 = getKeyValue(str.substr(20)); 
		} else if(str.find("Sec-WebSocket-Key2:")!=string::npos){
			//cout << "key2 uncoded : "<< str.substr(20) <<"\n"; 
			
			key2 = getKeyValue(str.substr(20)); 
			if( i+1 < data.size() ) {
				key3 = data[i+1];
			}
		} else if( str.find("Upgrade:") != string::npos ) {
			h_upgrade = str;
		} else if( str.find("Origin: http://") != string::npos ) {
			h_origin = str.substr(15);
		}
	}
	
	if((key1==-1) || (key2==-1) || (key3 =="") || (h_upgrade=="") || (h_origin=="")) return ""; 
	//printf("key1 %d key2 %d key3 %s\n", key1, key2, key3.c_str());
	unsigned char sum[16];
	
	sum[0] = (unsigned char)((key1 >> 24) & 0xFF) ;
	sum[1] = (unsigned char)((key1 >> 16) & 0xFF);
	sum[2] = (unsigned char)((key1 >> 8) & 0xFF);
	sum[3] = (unsigned char)((key1) & 0xFF);
	sum[4] = (unsigned char)((key2 >> 24) & 0xFF);
	sum[5] = (unsigned char)((key2 >> 16) & 0xFF);
	sum[6] = (unsigned char)((key2 >> 8) & 0xFF);
	sum[7] = (unsigned char)((key2) & 0xFF);
	
	string k;
	k.assign(sum, sum+8);
	k.append(key3);
	
	MD5 md5;
	md5.digestString((char*)k.c_str());
	
	string tmp;
	tmp.assign(md5.digestRaw, md5.digestRaw+16);
	
	string EOLN = "\r\n";
	string response = 
	"HTTP/1.1 101 WebSocket Protocol Handshake" + EOLN +
	h_upgrade + EOLN +
	"Connection: Upgrade" + EOLN +
	"Sec-WebSocket-Origin: http://" + h_origin + EOLN +
	"Sec-WebSocket-Location: ws://" + h_origin + ":" + ofToString(port) + "/" + EOLN +
	"Sec-WebSocket-Protocol: sebly" + EOLN + EOLN + tmp + EOLN;
	
	int size = response.length();
	
	char *returnValue = new char[1024];
	memset(returnValue, 0, 1024);
	response.copy(returnValue, size);
	returnValue[size] = '\0';
	
	
	// should we clear the mem now? 
	
	return response;
	
};


long WebSocketClient:: getKeyValue(string str) { 
	//cout << "getting key Value for "<< str<<"\n";
	trim(&str); 
	string numstring = "";
	
	int spacecount = 0; 
	for(int i=0; i< str.length(); i++) { 
		char c = str.at(i); 
		if(isdigit(c)){
			numstring+=c; 
		} else if (c==' ') {
			spacecount++; 
		}
	}
	//char * pEnd;
	//long num = strtol(numstring.c_str(),&pEnd, 10); 
	
	unsigned long num = (unsigned long)atoll(&numstring[0]); 
	
	//cout << "numstring : "<< numstring << " num : " << num << "\n";
	//cout << num << " " << spacecount << " " << (num/spacecount)<< "\n";
	
	return (unsigned long)(num/spacecount); 
	
}

void WebSocketClient::trim(string* s){
	s->erase(s->begin(), std::find_if(s->begin(), s->end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	s->erase(std::find_if(s->rbegin(), s->rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s->end());
	//return s; 
	
}


