

#pragma once

#include "ofMain.h"
#include "ofSerialForArduino.h"
#include "xbeeATst.h"
#include "xbeeSRst.h"

#include "ofxXmlSettings.h"

class xbee {
public: 
    string high;
    string low;
    string local;
};


//#endif

class light {
    
public: 
    
    int delayLength;
    vector < ofPoint > colors;
    
    
    light() {
        nextOne = NULL;
        
        delayLength = 10;
        
    }
    
    void draw(float x,float y){
        ofFill();
        ofSetColor(color.x, color.y, color.z);
        ofRect(x,y,100,100);
    }
    ofPoint colorTarget;
    ofPoint color;
    void update(){
        
        colors.push_back(color);
        
        //        while(colors.size() > delayLength){
        //            colors.erase(colors.begin());
        //        }
        
        while (colors.size() > delayLength){
            ofPoint end = colors[0];
            colors.erase(colors.begin());
            if (nextOne != NULL){
                nextOne->color = end;
            }
        }
        //        color = 0.7f * color + 0.3 * colorTarget;
        //        if ((color - colorTarget).length() < 1.0f ){
        //            if (nextOne != NULL){
        //                nextOne->colorTarget = colorTarget;
        //            }
        //        }
    }
    light * nextOne;
    
    
};



class xbeeController {

public: 
    
    void setup();
    void update(ofPoint color);
    void draw();
    
    void setupXbee();
    void sendColorData();
    ofSerialForArduino serial;
    xbeeATst XBEE;
    xbeeSRst XBEE_SR;
    xbee XB;
    
    
    void writeXbeesToFile();
    void loadXbeesFromFile();
    
    vector < light * > lights;
    
    float delay;
    float balloonFade;
    
    
    vector < xbee > xbees;
    
        
};



