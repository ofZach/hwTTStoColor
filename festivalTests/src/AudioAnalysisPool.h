/*
 * AudioAnalysisPool.h
 *
 *  Created on: 22/08/2012
 *      Author: arturo
 */

#ifndef AUDIOANALYSISPOOL_H_
#define AUDIOANALYSISPOOL_H_

#include "ofMain.h"
#include "AudioAnalysis.h"


class AudioAnalysisPool {
public:
	AudioAnalysisPool();
	virtual ~AudioAnalysisPool();

	ofPtr<AudioAnalysis> getAnalyzer();
	void releaseAnalyzer(ofPtr<AudioAnalysis> analizer);
	void resize(int size);

private:
    vector<ofPtr<AudioAnalysis> > pool;
    ofMutex mutex;
};

#endif /* AUDIOANALYSISPOOL_H_ */
