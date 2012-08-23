/*
 * AudioAnalysisPool.cpp
 *
 *  Created on: 22/08/2012
 *      Author: arturo
 */

#include "AudioAnalysisPool.h"
#include "Poco/ScopedLock.h"

AudioAnalysisPool::AudioAnalysisPool() {
	// TODO Auto-generated constructor stub

}

AudioAnalysisPool::~AudioAnalysisPool() {
	// TODO Auto-generated destructor stub
}

ofPtr<AudioAnalysis> AudioAnalysisPool::getAnalyzer(){
	Poco::ScopedLock<ofMutex> lock(mutex);
	if(pool.empty()){
		ofPtr<AudioAnalysis> analizer(new AudioAnalysis);
		analizer->setup();
		cout << "pool empty returning new AudioAnalysis" << endl;
		return analizer;
	}else{
		cout << "pool size " << pool.size() << endl;
		ofPtr<AudioAnalysis> analizer = pool.back();
		pool.erase(pool.end()-1);
		return analizer;
	}
}

void AudioAnalysisPool::releaseAnalyzer(ofPtr<AudioAnalysis> analizer){
	Poco::ScopedLock<ofMutex> lock(mutex);
	pool.push_back(analizer);
}
