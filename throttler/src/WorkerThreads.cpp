/*
 * WorkerThreads.cpp
 *
 *  Created on: 22/08/2012
 *      Author: arturo
 */

#include "WorkerThreads.h"
#include "ofURLFileLoader.h"
#include "ofUtils.h"
#include "ThreadPool.h"
#include "ofLog.h"

unsigned WorkerThreads::numThreads = 0;
unsigned WorkerThreads::maxThreads = 1;
ofMutex WorkerThreads::mutex;
//string WorkerThreads::url = "http://localhost:8888/tts.of?text=testing%20tts%20example%20with%20some%20long%20text&type=base64";
string WorkerThreads::url = "http://198.101.204.58/tts.of?text=testing%20tts%20example%20with%20some%20long%20text&type=base64";

ThreadPool * WorkerThreads::pool = new ThreadPool;

WorkerThreads::WorkerThreads() {
	time = 0;
	http.setBasicAuthentication("yesyesno","olympic#2012");
}

WorkerThreads::~WorkerThreads() {
	// TODO Auto-generated destructor stub
}

void WorkerThreads::threadedFunction(){
	mutex.lock();
	numThreads++;
	mutex.unlock();

	time = ofGetElapsedTimeMillis();
	const ofxHttpResponse & response = http.postData("http://my.idigi.co.uk/ws/sci",data,"text/xml; charset=\"UTF-8\"");
	ofLogNotice() << response.responseBody;
	time = ofGetElapsedTimeMillis()-time;
	ofNotifyEvent(threadFinished,true,this);
	mutex.lock();
	numThreads--;
	mutex.unlock();
	pool->releaseThread(this);
}
