/*
 * ThreadPool.h
 *
 *  Created on: 22/08/2012
 *      Author: arturo
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include "ofMain.h"
#include "WorkerThreads.h"

class ThreadPool {
public:
	ThreadPool();
	virtual ~ThreadPool();

	WorkerThreads * getThread();
	void releaseThread(WorkerThreads * thread);

private:
	vector<WorkerThreads*> threads;
	ofMutex mutex;
};

#endif /* THREADPOOL_H_ */
