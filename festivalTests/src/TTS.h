/*
 * TTS.h
 *
 *  Created on: 02/08/2012
 *      Author: arturo
 */

#ifndef TTS_H_
#define TTS_H_

//#define USE_FESTIVAL_SERVER

#include "ofConstants.h"
#include "ofTypes.h"
#include "ofSoundBuffer.h"
#include "ofThread.h"
#include "ofEvents.h"
#include "Poco/Condition.h"
#include <queue>

#ifdef USE_FESTIVAL_SERVER
#include "festival_client.h"
#else
#include <festival.h>
#endif


class TTSData{
public:
	ofSoundBuffer * buffer;
	string text;
	int processingTime;
};

class TTS: public ofThread {
public:
	TTS();
	virtual ~TTS();

	void initialize();
	void start();

	void addText(string text);
	TTSData convertToAudio(string text, int samplingRate=-1);

	ofEvent<const TTSData> newSoundE;

private:
	void threadedFunction();

#ifdef USE_FESTIVAL_SERVER
	FT_Wave * wave;
	FT_Info * server;
#else
	EST_Wave wave;
#endif
	queue<string> texts;
	ofMutex mutex;
	Poco::Condition condition;
	ofSoundBuffer soundBuffer;
};

#endif /* TTS_H_ */
