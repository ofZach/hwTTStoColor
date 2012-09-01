#pragma once

#include "ofMain.h"
#include "ofxHttpUtils.h"
#include "ofxHTTPServer.h"
#include "ThreadPool.h"
#include "WorkerThreads.h"

class testApp : public ofBaseApp, public ofxHTTPServerListener{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void threadFinished(const void * sender,const bool & finished);

		void getRequest(ofxHTTPServerResponse & response);
		void postRequest(ofxHTTPServerResponse & response);
		void fileNotFound(ofxHTTPServerResponse & response);
		
		ThreadPool pool;
		ofxHTTPServer * httpServer;
		unsigned long sumTime,currentTime;
		double seconds;
		ofParameter<int> averageTime, threadsFinished;

		unsigned long timeServer;
		unsigned long oneSec, oneMinute;
		unsigned int reqPerSecondSum, reqPerMinuteSum;
		float requestsPerSecond, requestsPerMinute;
		float maxReqPerMinute, maxReqPerSecond;

		unsigned int droppedRequests, droppedLastMinute, totalRequests, requestTimeOut, requestsBeforeDrop;

		ofMutex timeCalcMutex;
		ofxHttpUtils http;
};
