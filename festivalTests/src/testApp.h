#pragma once

#include "ofMain.h"
#include "ofxHTTPServer.h"
#include "JSONExporter.h"
#include "AudioAnalysis.h"
#include "AudioAnalysisPool.h"

class testApp : public ofBaseApp, public ofxHTTPServerListener{

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
		void fileNotFound(ofxHTTPServerResponse & response);

		void newSoundBuffer(const TTSData & soundBuffer);
		void saveWave(ofCairoRenderer::Type type);



		void audioOut(float * output, int buffersize, int nChannels, int deviceID, unsigned long long int tickCount);

		int numRequests;

		ofPtr<ofCairoRenderer> cairoScreenshot;

		bool headless, threaded, initialized;
		
		ofSoundStream soundStream;
		unsigned int position;

		ofxHTTPServer * httpServer;
		ofMutex mutex;
		Poco::Condition condition, clientsCondition;
		bool firstRun;

        string lastText;
        unsigned int lastServed;
        vector<string> alreadyAnalyzed;

        AudioAnalysisPool audioAnalysisPool;

        //splineInfo splineBriInfo;
        //splineInfo splineHueInfo;

        //splineConverter SC;
        int port;
};
