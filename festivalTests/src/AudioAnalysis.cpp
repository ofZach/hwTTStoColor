/*
 * AudioAnalysis.cpp
 *
 *  Created on: 22/08/2012
 *      Author: arturo
 */

#include "AudioAnalysis.h"
#include "Poco/Base64Encoder.h"

TTS AudioAnalysis::tts;

AudioAnalysis::AudioAnalysis() {
	// TODO Auto-generated constructor stub

}

AudioAnalysis::~AudioAnalysis() {
	// TODO Auto-generated destructor stub
}

void AudioAnalysis::setup(){
	AA.setup(44100);

	ACM.setup();
	ACM.volumeSmoothing = 0.8;
	ACM.hueRange = 35;
	ACM.hueShiftValStart = 0.2;
	ACM.darkness = 0.5;
	ACM.hueFadeRate = 0.96;

	tts.initialize();

	wave.setMode(ofPath::PATHS);
	wave.setArcResolution(20);
	wave.setCurveResolution(20);
	wave.setStrokeWidth(1);
	wave.setStrokeColor(ofColor(0));
	wave.setFilled(false);

}

void AudioAnalysis::analize(string text, bool _generateWave){
	TTSData ttsData = tts.convertToAudio(text,44100,soundBuffer);
	if(ttsData.buffer){
		computeMessageColors();
		if(_generateWave) generateWave();
	}
}

void AudioAnalysis::generateWave(){
	wave.clear();
	if(soundBuffer.bufferSize()>0){
		int increment = float(soundBuffer.bufferSize())/float(ofGetWidth());
		for(unsigned int i=0;i<soundBuffer.bufferSize();i+=increment){
			wave.lineTo(i/increment,ofGetHeight()*.5 + soundBuffer[i]*ofGetHeight()*.5);
		}
	}
}

void AudioAnalysis::computeMessageColors(){
    int bufferSize = 256;

    ofColor color;
    float hue = ofRandom(0,255);
    float sat = ofRandom(190,230);
    float bri = ofRandom(220,238);
    color.setHsb(hue, sat, bri);

    colorsForMessage.clear();
    brightnessMessage.clear();
    hueDiffMessage.clear();

	unsigned int audioCounter = 0;
	soundBuffer.resize(soundBuffer.size()+(bufferSize-(soundBuffer.size()%bufferSize)),0);

	while (audioCounter < soundBuffer.size()){
		/*for(int i=0;i<bufferSize;i++){
			if(abs(soundBuffer[audioCounter+i])>1) cout << soundBuffer[audioCounter+i] << " at " << audioCounter +i<< "/" << soundBuffer.size() << endl;
		}*/
		AA.analyzeFrame(&soundBuffer[audioCounter], bufferSize, aaFrameTemp);
		const ofColor & colorAltered = ACMtemp.update(aaFrameTemp, color);
        colorsForMessage.push_back(colorAltered);
        brightnessMessage.push_back(ofMap(colorAltered.getBrightness(), ACMtemp.darkness * 255, 255, 0,1));
        hueDiffMessage.push_back(ofMap(color.getHue() - colorAltered.getHue(), -ACMtemp.hueRange, ACMtemp.hueRange, 0,1));
		audioCounter+=bufferSize;
	}

    bool bOn[80]; // 10 bytes?
	float sums[80];
	for (int i=0;i < 80; i++){
		bOn[i] = false;

		float pctA = (float)(i) / (81.0f);
		float pctB = (float)(i+1) / (81.0f);

		int from =  (int)(pctA * brightnessMessage.size());
		int to =    (int)(pctB * brightnessMessage.size());
		int howMany =(to- from);
		float sum = 0;
		for (int j = from; j < to; j++){
			sum += brightnessMessage[j];
		}
		sum /= (float) howMany;

		sums[i] = sum;
		//cout << sum << endl;
		if (sum > 0.5){
			bOn[i] = true;
		}
	}


	for (int i = 0; i < 80; i++){
		float sumSmooth = 0;
		for (int j = -1; j <= 1; j++){

			int who = ofClamp(i+j, 0, 80-1);
			sumSmooth += sums[who] / 5.0f;

		}

		if (bOn[i] == true && sums[i] < sumSmooth  && fabs((sums[i] - sumSmooth)) > 0.006){
			bOn[i] = false;
		}

		// optional, force some on...
		if (false){
			if (bOn[i] == false && sums[i] > sumSmooth  && fabs((sums[i] - sumSmooth)) > 0.006){
				bOn[i] = true;
			}
		}

	}


	memset(data,0,30);
	data[0] = 255;
	data[1] = 1;
	data[2] = 127;
	data[3] = 255;
	for (int i = 0; i < 10; i++){
		data[4+i] = 0;
		for (int j = 0; j < 8; j++){
			data[4+i] |= bOn[i*8 + j] == true ? (0x01 << (7-j)) : 0;
		}
	}


	stringstream o;
	Poco::Base64Encoder encoder(o);
	for (size_t idx = 0; idx != 30; ++idx){
		encoder << data[idx];
	}
	encoder.close();
	base64.set(o);
}
