

#pragma once



#include "ofMain.h"
#include "audioAnalyzer.h"
#include "Graph.h"



class audioToColorMapper {

public: 
    
    void setup();
    ofColor update(audioAnalysisFrame frame, ofColor color);
    
        
    
    Graph temp;
    Graph temp2;
    Graph pitch;
    
    vector < float > volume;
    vector < float > volumeTrigger;
    float volumeSmooth;
    
    float peaks[3];
    
    float volumeSmoothing;
    float hueShiftValStart;
    float darkness;
    float hueFadeRate;
    float hueRange;
        
};



