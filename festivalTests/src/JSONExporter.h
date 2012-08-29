/*
 * JSONExporter.h
 *
 *  Created on: 08/08/2012
 *      Author: arturo
 */

#ifndef JSONEXPORTER_H_
#define JSONEXPORTER_H_

#include "ofxJSONElement.h"
#include "ofSoundBuffer.h"
#include "ofConstants.h"
#include "ofColor.h"
#include "splineConverter.h"

class JSONExporter {
public:
	JSONExporter();

	string getJSON(string text, ofSoundBuffer & soundBuffer, vector<ofColor> & colorsForMessage, int time, unsigned char data[36], string decodedData);

private:
	ofxJSONElement json;
	Json::Value colors;
	Json::Value color;
	Json::Value data;
	Json::Value splineBri, splineHue, splineBriCoeffs, splineHueCoeffs;
};

#endif /* JSONEXPORTER_H_ */
