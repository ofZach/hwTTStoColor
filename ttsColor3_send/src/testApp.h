#pragma once

#include "ofMain.h"
#include "ofxNativeGui.h"
#include "audioAnalyzer.h"
#include "audioToColorMapper.h"

#include "splineConverter.h"



class testApp : public ofBaseApp, public ofxNativeGuiEventInterface  {

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
        
        ofColor color;
        
        float fadeToBlack;
        float fadeActiveVal;
    
        
        ofColor colorAltered;
        vector < ofColor > colorsForMessage;
        void computeMessageColors();
    
        audioAnaylzer AAtemp;


        vector < float > brightnessMessage;
        vector < float > hueDiffMessage;
        splineInfo splineBriInfo;
        splineInfo splineHueInfo;
    
        splineConverter SC;
        
        
};
