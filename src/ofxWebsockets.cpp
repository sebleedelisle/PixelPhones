#include "ofxWebsockets.h"
#include "ofMain.h"

ofxWebSocket::ofxWebSocket() {
}

ofxWebSocket::~ofxWebSocket() {
	thread->join();
}

void ofxWebSocket::setup(string host, int port) {
	worker.setup(host, port);
}

void ofxWebSocket::start() {

	thread = new Poco::Thread();
	thread->start(worker);
}

void ofxWebSocket::sendToAllClients(string msg) {
	worker.send(msg);
}

void ofxWebSocket::sendToAllClients(uint8_t* buffer, int size) {
	cout << "send: "<< size << endl << flush;
	//worker.send((char*)buffer, size);
}


