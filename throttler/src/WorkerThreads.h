/*
 * WorkerThreads.h
 *
 *  Created on: 22/08/2012
 *      Author: arturo
 */

#ifndef WORKERTHREADS_H_
#define WORKERTHREADS_H_

#include "ofThread.h"
#include "ofTypes.h"
#include "ofEvents.h"
#include "ofxHttpUtils.h"

class ThreadPool;

class WorkerThreads: public ofThread {
public:
	WorkerThreads();
	virtual ~WorkerThreads();


	void threadedFunction();
	static unsigned numThreads;
	static unsigned maxThreads;
	static ofMutex mutex;
	static string url;
	static ThreadPool * pool;

	ofEvent<const bool> threadFinished;

	unsigned long time;
	ofxHttpUtils http;
	ofBuffer data;
	unsigned int requestTimeOut;
};

#endif /* WORKERTHREADS_H_ */
