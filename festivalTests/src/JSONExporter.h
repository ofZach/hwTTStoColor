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

class JSONExporter {
public:
	JSONExporter();

	string getJSON(string text, ofSoundBuffer & soundBuffer, vector<ofColor> & colorsForMessage, int time);

private:
	ofxJSONElement json;
	Json::Value colors;
	Json::Value color;
};

#endif /* JSONEXPORTER_H_ */
