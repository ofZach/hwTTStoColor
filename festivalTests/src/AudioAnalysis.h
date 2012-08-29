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
	void analize(string text, const ofColor & c, float hueThres=0.0535, bool generateWave=false);
	void computeMessageColors(const ofColor & c);

    vector < float > brightnessMessage;
    vector < float > hueDiffMessage;
    vector < ofColor > colorsForMessage;
    ofSoundBuffer soundBuffer;
    ofBuffer base64;
	ofPath wave;
    TTS tts;
	string decoded_data;
	unsigned char data[36];

private:
	void generateWave();
	void generateBase64(int durationms);
	void decodeData();
    audioToColorMapper ACM;
    audioAnaylzer AA;
    audioAnalysisFrame aaFrameTemp;
    bool bOn[80]; // 10 bytes?
	float sums[80];
    bool hUp[80]; // hue changes up
    bool hDown[80]; // 10 bytes?
    float hueThresh;
};

#endif /* AUDIOANALYSIS_H_ */
