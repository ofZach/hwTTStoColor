/*
 * JSONExporter.cpp
 *
 *  Created on: 08/08/2012
 *      Author: arturo
 */

#include "JSONExporter.h"

JSONExporter::JSONExporter() {
	// TODO Auto-generated constructor stub

}

string JSONExporter::getJSON(string text, ofSoundBuffer & soundBuffer, vector<ofColor> & colorsForMessage, int time){
	json.clear();
	colors.clear();
	json["text"] = text;
	json["durationms"] = (int)soundBuffer.getDuration();
	json["colorsamplems"] = (int)(soundBuffer.getDuration()/colorsForMessage.size());
	json["colorsampleroundms"] = (int)round(float(soundBuffer.getDuration())/float(colorsForMessage.size()));
	json["numsamples"] = (int)colorsForMessage.size();
	json["timeprocessingus"] = (int)time;
	for(int i=0;i<(int)colorsForMessage.size();i++){
		color[0] = (int)colorsForMessage[i].r;
		color[1] = (int)colorsForMessage[i].g;
		color[2] = (int)colorsForMessage[i].b;
		colors[i] = color;
	}
	json["colors"] = colors;
	return json.getRawString(true);
}
