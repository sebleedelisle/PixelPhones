#include "WebSocketServer.h"
#include "WebSocketClient.h"


//--------------------------
WebSocketServer::WebSocketServer(){
	verbose		= true;
	connected	= false;
	idCount		= 0;
	port		= 0;
	str			= "";
}

//--------------------------
WebSocketServer::~WebSocketServer(){
	close();
}

//--------------------------
void WebSocketServer::setVerbose(bool _verbose){
	verbose = _verbose;
}

//--------------------------
bool WebSocketServer::setup(int _port, bool blocking){
	if( !TCPManager.Create() ){
		if(verbose)printf("WebSocketServer: create() failed\n");
		return false;
	}
	if( !TCPManager.Bind(_port) ){
		if(verbose)printf("WebSocketServer: bind(port = %i) failed\n", _port);
		TCPManager.Close(); 
		return false;
	}
	
	connected		= true;
	port			= _port;
	bClientBlocking = blocking;
	
	startThread(true, false);
	return true;
}


//--------------------------
bool WebSocketServer::close(){
	
	if(!connected) return true;
	
	map<int,WebSocketClient>::iterator it;
	for(it=webSocketClients.begin(); it!=webSocketClients.end(); it++){
		it->second.close();
	}
	webSocketClients.clear();
	
	stopThread(); //stop the thread
	
	if( !TCPManager.Close() ){
		if(verbose)printf("WebSocketServer: unable to close connection\n");
		return false;
	}else{
		connected = false;
		return true;
	}
}

//--------------------------
bool WebSocketServer::disconnectClient(int clientID){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer::disconnectClient - client %i doesn't exist\n", clientID);
		return false;
	}
	else if(webSocketClients[clientID].close()){
		webSocketClients.erase(clientID);
		return true;
	}
	return false;
}

//--------------------------
bool WebSocketServer::send(int clientID, string message){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer::send -  client %i doesn't exist\n", clientID);
		return false;
	}
	else{
		webSocketClients[clientID].send(message);
		if(!webSocketClients[clientID].isConnected()) webSocketClients.erase(clientID);
		return true;
	}
}

//--------------------------
bool WebSocketServer::sendToAll(string message){
	if(webSocketClients.size() == 0) return false;
	
	map<int,WebSocketClient>::iterator it;
	vector<int> disconnect;
	for(it=webSocketClients.begin(); it!=webSocketClients.end(); it++){
	    int err = 0;
		if(it->second.isConnected()) err = it->second.send(message);
		if(!webSocketClients[it->first].isConnected()) disconnect.push_back(it->first);
	}
	for(int i=0; i<(int)disconnect.size(); i++){
    	webSocketClients.erase(disconnect[i]);
    }
	return true;
}

//--------------------------
string WebSocketServer::receive(int clientID){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer::receive -  client %i doesn't exist\n", clientID);
		return "client doesn't exist";
	}
	
	if( !webSocketClients[clientID].isConnected() ){
		disconnectClient(clientID);
		return "";
	}
	
	return webSocketClients[clientID].receive();
}

//--------------------------
bool WebSocketServer::sendRawBytes(int clientID, const char * rawBytes, const int numBytes){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer: client %i doesn't exist\n", clientID);
		return false;
	}
	else{
		return webSocketClients[clientID].sendRawBytes(rawBytes, numBytes);
	}
}

//--------------------------
bool WebSocketServer::sendRaw(int clientID, string rawString){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer: client %i doesn't exist\n", clientID);
		return false;
	}
	else{
		return webSocketClients[clientID].sendRaw(rawString);
	}
}

//--------------------------
bool WebSocketServer::sendRawBytesToAll(const char * rawBytes, const int numBytes){
	if(webSocketClients.size() == 0 || numBytes <= 0) return false;
	
	map<int,WebSocketClient>::iterator it;
	for(it=webSocketClients.begin(); it!=webSocketClients.end(); it++){
		if(it->second.isConnected())it->second.sendRawBytes(rawBytes, numBytes);
	}
	return true;
}

//--------------------------
int WebSocketServer::getNumReceivedBytes(int clientID){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer: client %i doesn't exist\n", clientID);
		return 0;
	}
	
	return webSocketClients[clientID].getNumReceivedBytes();
}

//--------------------------
int WebSocketServer::receiveRawBytes(int clientID, char * receiveBytes,  int numBytes){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer: client %i doesn't exist\n", clientID);
		return 0;
	}
	
	return webSocketClients[clientID].receiveRawBytes(receiveBytes, numBytes);
}

//--------------------------
int WebSocketServer::getClientPort(int clientID){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer: client %i doesn't exist\n", clientID);
		return 0;
	}
	else return webSocketClients[clientID].getPort();
}

//--------------------------
string WebSocketServer::getClientIP(int clientID){
	if( !isClientSetup(clientID) ){
		if(verbose)printf("WebSocketServer: client %i doesn't exist\n", clientID);
		return "000.000.000.000";
	}
	else return webSocketClients[clientID].getIP();
}

//--------------------------
int WebSocketServer::getNumClients(){
	return webSocketClients.size()-1;
}

//--------------------------
int WebSocketServer::getLastID(){
	return idCount;
}

//--------------------------
int WebSocketServer::getPort(){
	return port;
}

//--------------------------
bool WebSocketServer::isConnected(){
	return connected;
}

//--------------------------
bool WebSocketServer::isClientSetup(int clientID){
	return webSocketClients.find(clientID)!=webSocketClients.end();
}

//--------------------------
bool WebSocketServer::isClientConnected(int clientID){
	return isClientSetup(clientID) && webSocketClients[clientID].isConnected();
}

//don't call this
//--------------------------
void WebSocketServer::threadedFunction(){
	
	while( isThreadRunning() ){
		
		int acceptId;
		for(acceptId = 0; acceptId <= idCount; acceptId++){
			if(!isClientConnected(acceptId)) break;
		}
		
		if(acceptId == WEBSOCKET_MAX_CLIENTS){
			if(verbose)printf("WebSocketServer: reached max connected clients! \nWebSocketServer: no more connections accepted\n");
			break;
		}
		
		if( !TCPManager.Listen(WEBSOCKET_MAX_CLIENTS) ){
			if(verbose)printf("WebSocketServer: Listen() failed\n");
		}
		
		// this check makes a new client at acceptId, in other words, 
		// the next available slot. Seems weird to me but what do I know?
		// maybe Accept is a blocking function? 
		
		if( !TCPManager.Accept(webSocketClients[acceptId].TCPClient) ){
			if(verbose)printf("WebSocketServer: Accept() failed\n");
		}else{
			
			webSocketClients[acceptId].serverPortNum = port; // this is for the handshake
			webSocketClients[acceptId].setup(acceptId, bClientBlocking);
			
			if(verbose)printf("WebSocketServer: client %i connected on port %i\n", acceptId, webSocketClients[acceptId].getPort());
			
			if(acceptId == idCount) idCount++;
		}
	}
	if(verbose)printf("WebSocketServer: listen thread ended\n");
}





