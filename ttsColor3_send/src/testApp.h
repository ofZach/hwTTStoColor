#pragma once

#include "ofMain.h"
#include "ofxNativeGui.h"
#include "audioAnalyzer.h"
#include "audioToColorMapper.h"
#include "ofxXmlSettings.h"
#include "ofxMidi.h"
#include "xbeeController.h"




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
        xbeeController XBC;
        
        
        ofColor color;
        
        float fadeToBlack;
        float fadeActiveVal;
    
        void newMidiMessage(ofxMidiMessage& eventArgs);
        ofxMidiIn	midiIn;
        ofxMidiMessage midiMessage;
        
        
       
        ofColor colorAltered;
        
        vector < ofColor > colorsForMessage;
        void computeMessageColors();
    
    
        
};
