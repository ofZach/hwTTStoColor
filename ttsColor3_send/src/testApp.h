#pragma once

#include "ofMain.h"

#include "ofxMidi.h"
#include "ofxNativeGui.h"
#include "audioAnalyzer.h"
#include "audioToColorMapper.h"


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




class testApp : public ofBaseApp, public ofxNativeGuiEventInterface,  public ofxMidiListener  {

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void audioRequested		(float * output, int bufferSize, int nChannels); 
        
//#ifdef XBEE_SEND
    
    void setupXbee();
    void sendColorData();
    ofSerialForArduino serial;
    xbeeATst XBEE;
    xbeeSRst XBEE_SR;
    xbee XB;
    
    
    void writeXbeesToFile();
    void loadXbeesFromFile();
    
    
    
    
    vector < xbee > xbees;
    
   // vector < light * > lights;
    
    
//#endif
        
    	// the gui window. 
        ofxNativeGui GUI;
        void guiEvent(nativeWidget & widget);
    
        audioAnaylzer AA;
    
    
    
        // for playing audio that's been saved out: 
        float * left;
        vector < float > audioSamples;
        bool bPlaying;
        int counter;
        
    
        audioAnalysisFrame aaFrame;
        
        audioToColorMapper ACM;
    
        
        ofColor color;
        
        float fadeToBlack;
        float fadeActiveVal;
    
        void newMidiMessage(ofxMidiMessage& eventArgs);
        ofxMidiIn	midiIn;
        ofxMidiMessage midiMessage;
        vector < light * > lights;
    
        
        float delay;
        float balloonFade;
        ofColor colorAltered;
        
        vector < ofColor > colorsForMessage;
        void computeMessageColors();
    
    
        
};
