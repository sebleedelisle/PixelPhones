#ifndef OFXWEBSOCKETSH
#define OFXWEBSOCKETSH

#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/Mutex.h>
#include <string>
#include "libwebsockets.h"
#include "ofxWebSocketWorker.h"

using std::string;

class ofxWebSocket {
public:
	ofxWebSocket();
	~ofxWebSocket();
	void setup(string host, int port);
	void start();
	void sendToAllClients(string msg);
	void sendToAllClients(uint8_t* buffer, int size);
private: 
	int port;
	string host;
	struct libwebsocket_context *context;
	Poco::Thread* thread;
	ofxWebSocketWorker worker;

};

#endif