/*
 * TTS.cpp
 *
 *  Created on: 02/08/2012
 *      Author: arturo
 */

#include "TTS.h"
#include "ofUtils.h"

static bool initialized = false;

extern "C"{
cst_val *flite_set_voice_list(void);
}

TTS::TTS() {
#ifdef USE_FESTIVAL_SERVER
	server = NULL;
	wave = NULL;
#elif defined(USE_FLITE)
	voice = NULL;
#endif
}

TTS::~TTS() {
}

void TTS::initialize(){
#ifdef USE_FESTIVAL_SERVER
	FT_Info info = {0,"127.0.0.1",1234,""};
	server = festivalOpen(NULL);
#elif defined(USE_FLITE)
	if(!initialized){
		flite_init();
		initialized = true;
	}
	flite_voice_list = flite_set_voice_list();
	voice = flite_voice_select(NULL);
	if(!voice) ofLogError() << "couldn't initialize voices";
#else
	//int time = ofGetElapsedTimeMicros();
	festival_initialize(1,210000);
	//time = ofGetElapsedTimeMicros() - time;
#endif
}

void TTS::start(){
	startThread();
}



TTSData TTS::convertToAudio(string text, int samplingRate, ofSoundBuffer & soundBuffer){
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
#elif defined(USE_FLITE)
	cst_wave * wave = flite_text_to_wave(text.c_str(),voice);
	if(wave){
		/*for(int i=0;i<wave->num_samples;i++){
			wave->samples[i]) cout << wave->samples[i] << " at " << i<< "/" << wave->num_samples << endl;
		}*/
		soundBuffer.copyFrom(wave->samples,wave->num_samples,wave->num_channels,wave->sample_rate);
		if(samplingRate!=-1) soundBuffer.resample(float(wave->sample_rate)/float(samplingRate),ofSoundBuffer::Linear);
		data.buffer = &soundBuffer;
		data.text = text;
		delete[] wave->samples;
		delete wave;
	}else{
		ofLogError() << "couldn't generate wave";
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
			TTSData data = convertToAudio(text,44100,soundBuffer);
			ofNotifyEvent(newSoundE,data);
			mutex.lock();
		}
		condition.wait(mutex);
	}
}
