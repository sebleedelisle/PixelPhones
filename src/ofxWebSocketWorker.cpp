#include "ofxWebSocketWorker.h"
#include <sys/time.h>
#include <iostream>
#include <sstream>

using namespace std;
ofxWebSocketWorker* ofxWebSocketWorker::instance = NULL;

static struct libwebsocket_protocols protocols[] = {
{
	"openFrameworks",		/* name */
	callback_http,		/* callback */
	0			/* per_session_data_size */
},
{
	NULL, NULL, 0		/* End of list */
}
};


static int callback_http(struct libwebsocket_context * context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason, void *user,
							   void *in, size_t len)
{
	char client_name[128];
	char client_ip[128];
	printf("Callback http\n");
	//libwebsocket_write(struct libwebsocket *wsi, unsigned char *buf, size_t len,
//				     enum libwebsocket_write_protocol protocol);
	unsigned char buf[1] = {'c'};
	
	switch(reason) {
		case LWS_CALLBACK_ESTABLISHED: {
			printf("Callback established.\n");
			break;
		}
		
		case LWS_CALLBACK_BROADCAST: {
			printf("Callback broadcast: %d.\n", len);
			unsigned char* data = new unsigned char[len];
			memcpy(data, (unsigned char*)in, len);

			int n = libwebsocket_write(wsi, data, len, LWS_WRITE_TEXT);
			printf("Send some data: %d\n", n);
			delete[] data;	
			break;
			
		}
		
		case LWS_CALLBACK_RECEIVE: {
			vector<char>* buffer = ofxWebSocketWorker::getInstance()->getClientBuffer(wsi);
			std::copy(static_cast<char*>(in), static_cast<char*>(in)+len, std::back_inserter(*buffer)); 
			return 0;
			break;
		}

		case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION: {
			printf("Callback connection.\n");
			return 0;
			break;
		}
		case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED: {
			printf("Callback: client confirm extension\n");
			return 1;
			break;
		}
		
		default: {
			printf("Callback - not handled! - %d\n", reason);
			break;
		}
	}
	return 0;

}


ofxWebSocketWorker::ofxWebSocketWorker() 

{
}
ofxWebSocketWorker::~ofxWebSocketWorker() {
}

void ofxWebSocketWorker::setup(string h, int p) {
	if(ofxWebSocketWorker::instance != NULL) {
		printf("Sorry, only one websocker server instance allowed\n");
		exit(1);
	}
	ofxWebSocketWorker::instance = this;
	host = h;
	port = p;
}

ofxWebSocketWorker* ofxWebSocketWorker::getInstance() {
	return ofxWebSocketWorker::instance;
}
	
void ofxWebSocketWorker::run() {
	// create a new context.
	int options = 0;
	context = libwebsocket_create_context(
		port
		,NULL
		,protocols
		,libwebsocket_internal_extensions
		,NULL
		,NULL
		,-1
		,-1
		,options
	);
	if(context == NULL) {
		printf("Cannot a context.\n");
		exit(1);
	}
	
	int oldus = 0;
	unsigned char buf[1] = {'c'};
	while (true) {
		//cout << "." << endl;
		{
			Poco::Mutex::ScopedLock lock(mutex);
			deque<string>::iterator it = messages.begin();
			while(it != messages.end()) {
				//cout << "broadcasting" << endl;
			//	cout << ">>>" << *it << endl;
				//int n = libwebsocket_write(wsi, buf, 1, LWS_WRITE_TEXT);
				string& to_send = *it;
				if(to_send.size() > 0) {
					unsigned char* msg = (unsigned char*)to_send.c_str();
					
					libwebsockets_broadcast(
						&protocols[0],
						msg
						,to_send.size()
					);
				}
				it = messages.erase(it);
				libwebsocket_service(context,1);
			}
		}
	//	cout << "..." << endl;
		
//			++it;
		
		libwebsocket_service(context, 1);
		//Thread:sleep(50000);
	}
	
}

void ofxWebSocketWorker::send(string message) {
cout << "Adding message... " << endl;
	Poco::Mutex::ScopedLock lock(mutex);
	messages.push_back(message);
	cout << "num: " << messages.size() << endl;
}

void ofxWebSocketWorker::send(char* toCopy, int size) {
	Poco::Mutex::ScopedLock lock(mutex);
	string buf(toCopy, toCopy+size);
//	buf.copy((char*)toCopy, size, 0);
	messages.push_back(buf);
//	cout << "ok" << buf.size() << endl;
	/*
	//Poco::Mutex::ScopedLock lock(mutex);
	//string1.copy( ptr2, length, 0 );
	for(int i = 0; i < 10; ++i) {
		char c = (char)toCopy[i];
		cout << c << ", " << endl;
	}
	string buf;
	buf.copy((char*)toCopy, size, 0);
	cout << "SENDING" << endl;
	cout << buf.size() << ", " << size << endl;
	*/
}

vector<char>* ofxWebSocketWorker::getClientBuffer(libwebsocket* sock) {
	ofxWebSocketWorker::buffer_iterator it = socket_buffers.find(sock);
	if(it == socket_buffers.end()) {
		vector<char>* new_buff = new vector<char>();
		socket_buffers.insert(std::pair<libwebsocket*, vector<char>* >(sock, new_buff));
		return new_buff;
	}
	return (it->second);
}