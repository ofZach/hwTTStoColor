#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "ofAppNoWindow.h"

//========================================================================
int main(  int argc, char *argv[]  ){

	testApp * app = new testApp;
	app->headless = false;
	app->threaded = false;
	app->port = 8888;
	bool service = false;
	for(int i=1;i<argc;i++){
		string arg = argv[i];
		if(arg.find("--")==0){
			arg = arg.substr(2);
			if(arg=="headless"){
				app->headless = true;
			}
			if(arg=="notthreaded"){
				app->threaded = false;
			}
			if(arg=="service"){
				service = true;
				app->headless = true;
				ofSetDataPathRoot("/var/www/");
			}
			if(arg=="port"){
				app->port = ofToInt(argv[i+1]);
			}
		}
	}

	if(service){
		ofFile pidFile(ofFilePath::join("/var/www/","tts"+ofToString(app->port) + ".pid"),ofFile::WriteOnly);
		pidFile << getpid() << endl;
		ofLogToFile("/var/www/tts"+ofToString(app->port) + ".log",true);
	}

	if(app->headless){
		ofAppNoWindow * window = new ofAppNoWindow;
		ofSetupOpenGL(window, 1024,240, OF_WINDOW);			// <-------- setup the GL context
	}else{
		ofAppGlutWindow * window = new ofAppGlutWindow;
		ofSetupOpenGL(window, 1024,240, OF_WINDOW);			// <-------- setup the GL context
	}

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( app );

}
