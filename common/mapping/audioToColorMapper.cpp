

#include "audioToColorMapper.h"


float t = 0;
float trigSmooth = 0;
float whichWayHue;
ofColor colorHueShifted;
float hueShiftVal = 0;


void audioToColorMapper::setup(){
    
    
    temp.setup("volume");
    temp.setSmoothing(0.1,0.1);
    temp.setBidirectional(true);
    
    temp2.setup("volumeTrigger");
    temp2.setSmoothing(0.2, 0.2);
    
    pitch.setup("volumeTrigger");
    pitch.setBidirectional(true);
    
    volumeSmooth = 0.0;
    
    for (int i = 0; i < 3; i++){
        peaks[i] = 0;
    }
    
    
    volumeSmoothing = 0.8f;
    hueRange= 35;
    hueShiftValStart = 0.2;
    darkness = 0.5;
    
    hueFadeRate = 0.96;
    
    
}

ofColor audioToColorMapper::update(audioAnalysisFrame aaFrame, ofColor color){
    
    
    for (int i = 0; i < 9; i++){
        int who = i / 3;
        if (peaks[who] < aaFrame.fft[i]){
            peaks[who] = aaFrame.fft[i];
        }
    }
    for (int i = 0; i < 3; i++){
        peaks[i] *= 0.95f;
    }
    
    
    
    if (temp.getNormalized() > temp.threshold) t = 1;
    else t = 0;
    
    cout << volumeSmoothing << endl;
    
    volumeSmooth = volumeSmoothing * volumeSmooth + (1-volumeSmoothing) * aaFrame.volume;
    
    volume.push_back(volumeSmooth);
    if (volume.size()> 128){
        volume.erase( volume.begin());
    }
    
    float volTrig = (volumeSmooth*5) > 0.26 ? 1.0 : 0.0;
    
    volumeTrigger.push_back(volTrig);
    if (volumeTrigger.size()> 128){
        volumeTrigger.erase( volumeTrigger.begin());
    }
    
    
    
    trigSmooth = 0.85f * trigSmooth + 0.15f * volTrig;
    
    pitch.addSample(aaFrame.pitch);
    
    if (pitch.getTriggered()){
        
        float dir = peaks[1] > peaks[0] ? -1 : 1;
        
        whichWayHue = peaks[2]/10.0 * hueRange * dir;
        colorHueShifted = color;
        float hue = color.getHue() + whichWayHue;
        while (hue < 0) hue += 255;
        while (hue > 255) hue -= 255;
        colorHueShifted.setHsb(hue, color.getSaturation(), color.getBrightness());
        hueShiftVal = hueShiftValStart;
    }
    
    hueShiftVal *= hueFadeRate;
    
    ofColor colorDarker = color;
    colorDarker.setBrightness(color.getBrightness()*darkness);
    ofVec3f darker;
    darker.x = colorDarker.r;
    darker.y = colorDarker.g;
    darker.z = colorDarker.b;
    
    ofVec3f temp;
    temp.x = (1-hueShiftVal) * color.r + hueShiftVal * colorHueShifted.r;
    temp.y = (1-hueShiftVal) * color.g + hueShiftVal * colorHueShifted.g;
    temp.z = (1-hueShiftVal) * color.b + hueShiftVal * colorHueShifted.b;
    
    temp = trigSmooth * temp + (1-trigSmooth)* darker;
    
    
    ofColor newColor;
    newColor.set(temp.x, temp.y, temp.z);
    return newColor;
    
    
    
}