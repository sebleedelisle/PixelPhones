#pragma once


#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/Mutex.h>
#include <Poco/ScopedLock.h>
#include <string>
#include <deque>
#include <map>
#include <vector>
#include "libwebsockets.h"

using std::string;
using std::deque;
using std::map;
using std::vector;

extern "C" {
		static int callback_http(
			struct libwebsocket_context * context
			,struct libwebsocket *wsi		
			,enum libwebsocket_callback_reasons reason
			,void *user
			,void *in
			,size_t len
		);
		
		static void dump_handshake_info(struct lws_tokens *lwst);
}

class ofxWebSocketWorker :  public Poco::Runnable {
public:
	static ofxWebSocketWorker* instance;
	static ofxWebSocketWorker* getInstance();
	typedef map<libwebsocket*, vector<char>* >::iterator	buffer_iterator;
	ofxWebSocketWorker();
	~ofxWebSocketWorker();
	virtual void run();
	void setup(string host, int port);
	void send(string message);
	void send(char* buffer, int size);
	vector<char>* getClientBuffer(libwebsocket* sock);
private:
	
	map<libwebsocket*, vector<char>* > socket_buffers;
	string host;
	int port;
	struct libwebsocket_context *context;
	Poco::Mutex mutex;
	deque<string> messages;
};


