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
	splineBriCoeffs.clear();
	splineHueCoeffs.clear();

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

	/*splineBri["DX"] = splineBriInfo.DX;
	splineBri["mean"] = splineBriInfo.mean;
	splineBri["xmin"] = (int)splineBriInfo.xmin;
	splineBri["xmax"] = (int)splineBriInfo.xmax;
	for (int i = 0; i <= splineBriInfo.M; i++){
		splineBriCoeffs[i] = splineBriInfo.coefficients[i];
	}
	splineBri["coeffs"] = splineBriCoeffs;

	json["splineBri"] = splineBri;

	splineHue["DX"] = splineHueInfo.DX;
	splineHue["mean"] = splineHueInfo.mean;
	splineHue["xmin"] = (int)splineHueInfo.xmin;
	splineHue["xmax"] = (int)splineHueInfo.xmax;
	for (int i = 0; i <= splineHueInfo.M; i++){
		splineHueCoeffs[i] = splineHueInfo.coefficients[i];
	}
	splineHue["coeffs"] = splineHueCoeffs;

	json["splineHue"] = splineHue;*/

	return json.getRawString(true);
}
