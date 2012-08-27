#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "Poco/Condition.h"

int availableServers[4] = {8885,8886,8887,8888};

void killServer(int port){
	ofLogError() << ofGetTimestampString() << ": killing " << port << endl;
	ofFile pidFile("/var/www/"+ofToString(port)+".pid");
	if(!pidFile.exists()) return;
	int pid;
	pidFile >> pid;

	kill(pid,SIGTERM);
	system(("/usr/bin/tts --port " + ofToString(port) + " --service &").c_str());
}

//========================================================================
int main( ){
	ofLogToFile("/var/www/watchdog.log",true);
    /*ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());*/

	while(true){
		ofSleepMillis(10000);
		for(int i=0;i<4;i++){
			ofHttpResponse resp = ofLoadURL("http://localhost:" + ofToString(availableServers[i]) + "/tts.of?text=test&type=base64&r=0&g=0&b=0");
			if(resp.status==-1){
				killServer(availableServers[i]);
			}
		}
	}

}
