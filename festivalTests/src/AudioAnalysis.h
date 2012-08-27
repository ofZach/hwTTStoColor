/*
 * AudioAnalysis.h
 *
 *  Created on: 22/08/2012
 *      Author: arturo
 */

#ifndef AUDIOANALYSIS_H_
#define AUDIOANALYSIS_H_

#include "ofMain.h"
#include "audioAnalyzer.h"
#include "audioToColorMapper.h"
#include "TTS.h"

class AudioAnalysis {
public:
	AudioAnalysis();
	virtual ~AudioAnalysis();

	void setup();
	void analize(string text, const ofColor & c, bool generateWave=false);
	void computeMessageColors(const ofColor & c);

    vector < float > brightnessMessage;
    vector < float > hueDiffMessage;
    vector < ofColor > colorsForMessage;
    ofSoundBuffer soundBuffer;
    ofBuffer base64;
	ofPath wave;
    TTS tts;

private:
	void generateWave();
	void generateBase64();
    audioToColorMapper ACM;
    audioAnaylzer AA;
    audioAnalysisFrame aaFrameTemp;
    audioToColorMapper ACMtemp;
    bool bOn[80]; // 10 bytes?
	float sums[80];
	unsigned char data[36];
    bool hUp[80]; // hue changes up
    bool hDown[80]; // 10 bytes?
    float hueThresh;
};

#endif /* AUDIOANALYSIS_H_ */
