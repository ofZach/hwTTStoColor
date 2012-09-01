/*
 * ThreadPool.cpp
 *
 *  Created on: 22/08/2012
 *      Author: arturo
 */

#include "ThreadPool.h"
#include "Poco/ScopedLock.h"

ThreadPool::ThreadPool() {
	// TODO Auto-generated constructor stub

}

ThreadPool::~ThreadPool() {
	// TODO Auto-generated destructor stub
}

WorkerThreads * ThreadPool::getThread(){
	Poco::ScopedLock<ofMutex> lock(mutex);
	WorkerThreads * thread;
	if(threads.empty()){
		thread = new WorkerThreads;
	}else{
		thread = threads.back();
		threads.erase(threads.end()-1);
	}
	return thread;
}

void ThreadPool::releaseThread(WorkerThreads * thread){
	Poco::ScopedLock<ofMutex> lock(mutex);
	threads.push_back(thread);
}
