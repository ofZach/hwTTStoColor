/*
 * TTS.cpp
 *
 *  Created on: 02/08/2012
 *      Author: arturo
 */

#include "TTS.h"
#include "ofUtils.h"

TTS::TTS() {
	// TODO Auto-generated constructor stub

}

TTS::~TTS() {
	// TODO Auto-generated destructor stub
}


void TTS::start(){
	startThread();
}



void TTS::convertToAudio(string text, int samplingRate){
	TTSData data;
	data.processingTime = ofGetElapsedTimeMicros();
	festival_text_to_wave(text.c_str(),wave);

	if(samplingRate!=-1) wave.resample(samplingRate);
	soundBuffer.copyFrom(&wave.values()(0,0),wave.length(),wave.num_channels(),wave.sample_rate());
	data.buffer = &soundBuffer;
	data.text = text;
	data.processingTime = ofGetElapsedTimeMicros() - data.processingTime;

	ofNotifyEvent(newSoundE,data);
}

void TTS::addText(string text){
	mutex.lock();
	texts.push(text);
	condition.signal();
	mutex.unlock();
}

void TTS::threadedFunction(){
	//int time = ofGetElapsedTimeMicros();
	festival_initialize(1,210000);
	//time = ofGetElapsedTimeMicros() - time;

	while(isThreadRunning()){
		while(!texts.empty()){
			string text = texts.front();
			texts.pop();
			mutex.unlock();
			convertToAudio(text);
			mutex.lock();
		}
		condition.wait(mutex);
	}
}
