/*
 * TTS.cpp
 *
 *  Created on: 02/08/2012
 *      Author: arturo
 */

#include "TTS.h"
#include "ofUtils.h"


TTS::TTS() {
#ifdef USE_FESTIVAL_SERVER
	server = NULL;
	wave = NULL;
#endif
}

TTS::~TTS() {
}

void TTS::initialize(){
#ifdef USE_FESTIVAL_SERVER
	FT_Info info = {0,"127.0.0.1",1234,""};
	server = festivalOpen(NULL);
#else
	//int time = ofGetElapsedTimeMicros();
	festival_initialize(1,210000);
	//time = ofGetElapsedTimeMicros() - time;
#endif
}

void TTS::start(){
	startThread();
}



TTSData TTS::convertToAudio(string text, int samplingRate){
	TTSData data;
	data.processingTime = ofGetElapsedTimeMicros();
#ifdef USE_FESTIVAL_SERVER
	wave = festivalStringToWave(server,text.c_str());
	if(wave){
		ofLogNotice() << "got wave";
		soundBuffer.copyFrom(wave->samples,wave->num_samples,1,wave->sample_rate);
		if(samplingRate!=-1) soundBuffer.resample(wave->sample_rate/samplingRate);
		data.buffer = &soundBuffer;
		data.text = text;
	}else{
		ofLogError() << "couldn't read wave from server";
	}
#else
	if(festival_text_to_wave(text.c_str(),wave)){
		if(samplingRate!=-1) wave.resample(samplingRate);
		soundBuffer.copyFrom(&wave.values()(0,0),wave.length(),wave.num_channels(),wave.sample_rate());
		data.buffer = &soundBuffer;
		data.text = text;
	}
#endif
	data.processingTime = ofGetElapsedTimeMicros() - data.processingTime;
	return data;
}

void TTS::addText(string text){
	mutex.lock();
	texts.push(text);
	condition.signal();
	mutex.unlock();
}

void TTS::threadedFunction(){
	initialize();
	while(isThreadRunning()){
		while(!texts.empty()){
			string text = texts.front();
			texts.pop();
			mutex.unlock();
			TTSData data = convertToAudio(text);
			ofNotifyEvent(newSoundE,data);
			mutex.lock();
		}
		condition.wait(mutex);
	}
}
