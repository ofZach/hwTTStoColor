#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	int port = 8080;
	oneSec = ofGetElapsedTimeMillis();
	oneMinute = oneSec;
	maxReqPerSecond = 0.2;
	maxReqPerMinute = 60;
	droppedRequests=0;
	droppedLastMinute=0;
	totalRequests=0;
	ofSetDataPathRoot("/var/www/");
	ofSetFrameRate(10);
	httpServer = ofxHTTPServer::getServer();
	httpServer->setServerRoot("www");		 // folder with files to be served
	httpServer->setUploadDir("upload");		 // folder to save uploaded files
	httpServer->setCallbackExtension("of");	 // extension of urls that aren't files but will generate a post or get event
	httpServer->setListener(*this);
	httpServer->setMaxNumberClients(1000);
	httpServer->setMaxNumberActiveClients(4);
	httpServer->start(port,true);
}

void testApp::getRequest(ofxHTTPServerResponse & response){
	if(response.url=="/stats.of"){
		response.response =
		"<html>"
		"<head>"
		"</head>"
		"<body>"
				"requestsPerSecond: " + ofToString(requestsPerSecond) + "<br/>"
				"requestsPerMinute: " + ofToString(requestsPerMinute) + "<br/>"
				"total requests: " + ofToString(totalRequests) + "<br/>"
				"dropped requests total: " + ofToString(droppedRequests) + "<br/>"
				"dropped requests last minute: " + ofToString(droppedLastMinute) + "<br/>"
				"max requests per second: " + ofToString(maxReqPerSecond) + "<br/>"
				"max requests per minute: " + ofToString(maxReqPerMinute) + "<br/>"
				"<a href=\"/params.of\">params</a>"
		"</body>"
		"<html>";
		cout << response.response << endl;
	}else if(response.url=="/params.of"){
		ofFile templateP("www/params.html");
		ofBuffer templateS;
		templateP >> templateS;
		string templateText = templateS.getText();
		ofStringReplace(templateText,"%persecond%",ofToString(maxReqPerSecond));
		ofStringReplace(templateText,"%perminute%",ofToString(maxReqPerMinute));
		ofStringReplace(templateText,"%timeout%",ofToString(requestTimeOut));
		ofStringReplace(templateText,"%reqtodrop%",ofToString(requestsBeforeDrop));
		response.response = templateText;
	}
}

void testApp::postRequest(ofxHTTPServerResponse & response){
	if(response.url=="/sci.of"){
		if(response.requestFields.find("data")==response.requestFields.end()) return;
		if(WorkerThreads::numThreads>requestsBeforeDrop) return;
		timeCalcMutex.lock();
		reqPerSecondSum++;
		reqPerMinuteSum++;
		totalRequests++;
		if(requestsPerSecond>maxReqPerSecond || requestsPerMinute>maxReqPerMinute){
			droppedRequests++;
			droppedLastMinute++;
			timeCalcMutex.unlock();
			return;
		}
		timeCalcMutex.unlock();

		WorkerThreads * thread = pool.getThread();
		thread->data = response.requestFields["data"];
		ofAddListener(thread->threadFinished,this,&testApp::threadFinished);
		thread->startThread();
	}else if(response.url=="/setparams.of"){
		if(response.requestFields.find("persecond")!=response.requestFields.end()){
			maxReqPerSecond = ofToFloat(response.requestFields["persecond"]);
		}
		if(response.requestFields.find("perminute")!=response.requestFields.end()){
			maxReqPerMinute = ofToFloat(response.requestFields["perminute"]);
		}
		if(response.requestFields.find("timeout")!=response.requestFields.end()){
			requestTimeOut = ofToFloat(response.requestFields["timeout"]);
		}
		if(response.requestFields.find("reqtodrop")!=response.requestFields.end()){
			requestsBeforeDrop = ofToFloat(response.requestFields["reqtodrop"]);
		}
		response.location = "/stats.of";
		response.errCode=302;
	}
}

void testApp::threadFinished(const void * sender,const bool & finished){
	WorkerThreads * thread = (WorkerThreads*)sender;
	sumTime += thread->time;
	currentTime = currentTime * .8 + thread->time *.2;
	threadsFinished++;
	averageTime = float(sumTime)/float(threadsFinished);
	seconds = 0;
	ofRemoveListener(thread->threadFinished,this,&testApp::threadFinished);
}

void testApp::fileNotFound(ofxHTTPServerResponse & response){

}

//--------------------------------------------------------------
void testApp::update(){
	timeCalcMutex.lock();
	unsigned long now = ofGetElapsedTimeMillis();
	unsigned long timeElapsedSec = now - oneSec;
	unsigned long timeElapsedMin = now - oneMinute;
	requestsPerSecond=float(reqPerSecondSum)/float(timeElapsedSec)*1000;
	requestsPerMinute=float(reqPerMinuteSum)/float(timeElapsedMin)*1000;
	if(timeElapsedSec>1000){
		reqPerSecondSum=0;
		oneSec=now;
	}
	if(timeElapsedMin>60000){
		reqPerSecondSum=0;
		droppedLastMinute=0;
		oneSec=now;
	}
	timeCalcMutex.unlock();
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

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
