#pragma once

#include "ofMain.h"
#include "ofxHTTPServer.h"
#include "TTS.h"
#include "audioToColorMapper.h"
#include "JSONExporter.h"
#include "splineConverter.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void getRequest(ofxHTTPServerResponse & response);
		void postRequest(ofxHTTPServerResponse & response);

		void newSoundBuffer(const TTSData & soundBuffer);

		void generateWave();
		void computeMessageColors();
		void saveWave(ofCairoRenderer::Type type);



		void audioOut(float * output, int buffersize, int nChannels, int deviceID, unsigned long long int tickCount);

		ofPtr<ofCairoRenderer> cairoScreenshot;

		bool headless, threaded, initialized;
		
		ofSoundStream soundStream;
		ofSoundBuffer soundBuffer;
		unsigned int position;

		ofxHTTPServer * httpServer;
		TTS tts;
		ofMutex mutex;
		ofPath wave;
		Poco::Condition condition;
		bool firstRun;


        vector < ofColor > colorsForMessage;
        string lastText;
        audioToColorMapper ACM;
        audioAnaylzer AA;
        audioAnalysisFrame aaFrameTemp;

        JSONExporter json;
        unsigned int lastServed;
        vector<string> alreadyAnalyzed;

        vector < float > brightnessMessage;
        vector < float > hueDiffMessage;
        //splineInfo splineBriInfo;
        //splineInfo splineHueInfo;

        //splineConverter SC;

        bool bOn[80]; // 10 bytes?
    	unsigned char data[30];
};
