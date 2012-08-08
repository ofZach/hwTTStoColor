#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "ofAppNoWindow.h"

//========================================================================
int main(  int argc, char *argv[]  ){

	testApp * app = new testApp;
	app->headless = false;
	app->threaded = false;
	for(int i=1;i<argc;i++){
		string arg = argv[i];
		if(arg.find("--")==0){
			arg = arg.substr(2);
			if(arg=="headless"){
				app->headless = true;
			}
			if(arg=="threaded"){
				app->threaded = true;
			}
			if(arg=="service"){
				ofFile pidFile("/var/run/tts.pid",ofFile::WriteOnly);
				pidFile << getpid() << endl;
				app->headless = true;
				ofSetDataPathRoot("/var/www/");
			}
		}
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