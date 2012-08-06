/*
 * TTS.h
 *
 *  Created on: 02/08/2012
 *      Author: arturo
 */

#ifndef TTS_H_
#define TTS_H_

#include "ofConstants.h"
#include "ofTypes.h"
#include "ofSoundBuffer.h"
#include "ofThread.h"
#include "ofEvents.h"
#include "Poco/Condition.h"
#include <queue>
#include <festival.h>

class TTSData{
public:
	ofSoundBuffer * buffer;
	string text;
};

class TTS: public ofThread {
public:
	TTS();
	virtual ~TTS();

	void start();

	void addText(string text);
	void convertToAudio(string text, int samplingRate=-1);

	ofEvent<const TTSData> newSoundE;
	ofSoundBuffer soundBuffer;

private:
	void threadedFunction();

	EST_Wave wave;
	queue<string> texts;
	ofMutex mutex;
	Poco::Condition condition;
};

#endif /* TTS_H_ */
