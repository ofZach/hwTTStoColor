#include "testApp.h"
#include "ofCairoRenderer.h"
#include "ofFileUtils.h"
#include "audioAnalyzer.h"
#include "ofxJSONElement.h"
#include "Poco/Base64Encoder.h"

//--------------------------------------------------------------
void testApp::saveWave(ofCairoRenderer::Type type){
	ofPtr<ofBaseRenderer> storedRenderer = ofGetCurrentRenderer();

	//"www/"+path,
	cairoScreenshot->setupMemoryOnly(type, false);

	ofPtr<ofRendererCollection> rendererCollection = ofPtr<ofRendererCollection>(new ofRendererCollection);
	rendererCollection->renderers.push_back(storedRenderer);
	rendererCollection->renderers.push_back(cairoScreenshot);

	ofSetCurrentRenderer(rendererCollection);
	draw();
	cairoScreenshot->close();
	ofSetCurrentRenderer(storedRenderer);

	//ofFile index("www/index.html",ofFile::Append);
	//index << ("<a href=\"" + path + "\">"+path+"</a><br/>") ;
}

void testApp::computeMessageColors(){
    int bufferSize = 256;
	colorsForMessage.clear();
	brightnessMessage.clear();
	hueDiffMessage.clear();

    ofColor color;
    float hue = ofRandom(0,255);
    float sat = ofRandom(190,230);
    float bri = ofRandom(220,238);
    color.setHsb(hue, sat, bri);

	unsigned int audioCounter = 0;
	soundBuffer.resize(soundBuffer.size()+(256-(soundBuffer.size()%256)),0);
	while (audioCounter < soundBuffer.size()){
		AA.analyzeFrame(&soundBuffer[audioCounter], bufferSize, aaFrameTemp);
		const ofColor & colorAltered = ACM.update(aaFrameTemp, color);
		colorsForMessage.push_back(colorAltered);
		brightnessMessage.push_back(ofMap(colorAltered.getBrightness() - color.getBrightness(), 0, 255, 0,1));
		hueDiffMessage.push_back(ofMap(color.getHue() - colorAltered.getHue(), -ACM.hueRange, ACM.hueRange, 0,1));
		audioCounter+=bufferSize;
	}


	// now, let's do some spline fitting!
	splineBriInfo = SC.splineFromData(brightnessMessage,  80);
	splineHueInfo = SC.splineFromData(hueDiffMessage,  80);
}



//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255);
	ofSetColor(0);
	ofEnableAlphaBlending();

	ofDirectory dir("www");
	dir.listDir();
	dir.sort();
	for(unsigned int i=0;i<dir.size();i++){
		alreadyAnalyzed.push_back( dir.getName(i) );
	}
	ofFile lastServedStored("lastServed");
	if(lastServedStored.exists()){
		lastServedStored >> lastServed;
	}else{
		lastServed=0;
	}

	cairoScreenshot = ofPtr<ofCairoRenderer>(new ofCairoRenderer);

    AA.setup(44100);

    ACM.setup();
    ACM.volumeSmoothing = 0.8;
    ACM.hueRange = 35;
    ACM.hueShiftValStart = 0.2;
    ACM.darkness = 0.5;
    ACM.hueFadeRate = 0.96;

	if(threaded){
		initialized=true;
		ofAddListener(tts.newSoundE,this,&testApp::newSoundBuffer);
		tts.start();
	}else{
		initialized=false;
	}

	if(!headless){
		position = 0;
		soundStream.setup(this,2,0,44100,256,4);
	}


	httpServer = ofxHTTPServer::getServer();
	httpServer->setServerRoot("www");		 // folder with files to be served
	httpServer->setUploadDir("upload");		 // folder to save uploaded files
	httpServer->setCallbackExtension("of");	 // extension of urls that aren't files but will generate a post or get event
	ofAddListener(httpServer->getEvent,this,&testApp::getRequest);
	ofAddListener(httpServer->postEvent,this,&testApp::postRequest);
	httpServer->setMaxNumberClients(100);
	httpServer->start(8888);

	wave.setMode(ofPath::PATHS);
	wave.setArcResolution(20);
	wave.setCurveResolution(20);
	wave.setStrokeWidth(1);
	wave.setStrokeColor(ofColor(0));
	wave.setFilled(false);

	firstRun = true;
}


//--------------------------------------------------------------
void testApp::getRequest(ofxHTTPServerResponse & response){
	if(response.url=="/tts.of"){
		if(response.requestFields.find("text")==response.requestFields.end()){
			response.errCode=404;
			return;
		}
		string text = response.requestFields["text"];
		string type;
		if(response.requestFields.find("type")!=response.requestFields.end()){
			type = response.requestFields["type"];
		}

		if(threaded && type==""){
			tts.addText(text);
		}else{
			if(!initialized){
				tts.initialize();
				initialized = true;
			}
			unsigned long time = ofGetElapsedTimeMicros();
			lastText = text;
			TTSData data = tts.convertToAudio(text,44100);
			if(!headless) mutex.lock();
			if(data.buffer){
				soundBuffer = *data.buffer;
				computeMessageColors();
			}
			time = ofGetElapsedTimeMicros() - time;

			if(type==""){
				type = "json";
			}

			if(!headless || type!="json"){
				generateWave();
			}

			if(!headless) mutex.unlock();

			if(type == "svg"){
				saveWave(ofCairoRenderer::SVG);
				response.response = cairoScreenshot->getContentBuffer();
				response.contentType = "image/svg+xml";
			}else if(type=="pdf"){
				saveWave(ofCairoRenderer::PDF);
				response.response = cairoScreenshot->getContentBuffer();
				response.contentType = "application/pdf";
			}else if(type=="png"){
				saveWave(ofCairoRenderer::IMAGE);
				ofSaveImage(cairoScreenshot->getImageSurfacePixels(),response.response,OF_IMAGE_FORMAT_PNG);
				response.contentType = "image/png";
			}else if(type=="jpeg"){
				saveWave(ofCairoRenderer::IMAGE);
				ofSaveImage(cairoScreenshot->getImageSurfacePixels(),response.response,OF_IMAGE_FORMAT_JPEG);
				response.contentType = "image/jpeg";
			}else{
				response.response = json.getJSON(text,soundBuffer,colorsForMessage,splineBriInfo,splineHueInfo,time);
				response.contentType = "application/json";
			}
		}
	}else if(response.url=="/nextAnalized.of"){
		if(response.requestFields.find("type")!=response.requestFields.end()){
			stringstream o;
			Poco::Base64Encoder encoder(o);
			encoder << "abcdefghijklmnopqrstuvwz0987654321a";
			encoder.close();
			response.response.set(o);
			return;
		}
		mutex.lock();
		/*if(lastServed >= alreadyAnalyzed.size()){
			response.errCode = 404;
			mutex.unlock();
			return;
		}*/
		string path = alreadyAnalyzed[ofRandom(alreadyAnalyzed.size())];
		/*lastServed++;
		ofFile lastServedStored("lastServed",ofFile::WriteOnly);
		lastServedStored << lastServed;*/
		mutex.unlock();
		ofFile jsonFile("www/" + path);
		jsonFile >> response.response;
	}
}

//--------------------------------------------------------------
void testApp::postRequest(ofxHTTPServerResponse & response){
}


//--------------------------------------------------------------
void testApp::newSoundBuffer(const TTSData & tts){
	if(!headless) mutex.lock();
	unsigned long time = ofGetElapsedTimeMicros();
	soundBuffer = *tts.buffer;
	computeMessageColors();
	if(!headless) generateWave();
	time = ofGetElapsedTimeMicros() - time + tts.processingTime;
	string path = ofGetTimestampString()+".json";
	if(headless) mutex.lock();
	lastText = tts.text;
	alreadyAnalyzed.push_back(path);
	mutex.unlock();
	ofFile jsonFile("www/" + path,ofFile::WriteOnly);
	jsonFile << json.getJSON(tts.text,soundBuffer,colorsForMessage,splineBriInfo,splineHueInfo,time);
}

//--------------------------------------------------------------
void testApp::audioOut(float * output, int buffersize, int nChannels, int deviceID, unsigned long long int tickCount){
	mutex.lock();
	soundBuffer.copyTo(output,buffersize,nChannels,position,true);
	if(soundBuffer.size()>0){
		position += buffersize;
		position %= soundBuffer.bufferSize();
	}
	mutex.unlock();
}

void testApp::exit(){
	mutex.lock();
	condition.signal();
	mutex.unlock();
}

//--------------------------------------------------------------
void testApp::update(){
	if(headless && firstRun){
		mutex.lock();
		condition.wait(mutex); // we don't want update draw loop so wait till app exits
		firstRun = false;
	}
}

//--------------------------------------------------------------
void testApp::generateWave(){
	wave.clear();
	if(soundBuffer.bufferSize()>0){
		int increment = float(soundBuffer.bufferSize())/float(ofGetWidth());
		for(unsigned int i=0;i<soundBuffer.bufferSize();i+=increment){
			wave.lineTo(i/increment,ofGetHeight()*.5 + soundBuffer[i]*ofGetHeight()*.5);
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	mutex.lock();
	ofFill();
	ofSetColor(0);
	ofDrawBitmapString(lastText,10,10);
	wave.draw();
	if(!headless && soundBuffer.bufferSize()>0){
		int increment = float(soundBuffer.bufferSize())/float(ofGetWidth());
		ofSetColor(255,0,0);
		ofLine(position/increment,0,position/increment,ofGetHeight());
	}
	if(!colorsForMessage.empty()){
		float width = float(ofGetWidth())/float(colorsForMessage.size());
		for(int i=0;i<(int)colorsForMessage.size();i++){
			ofSetColor(colorsForMessage[i]);
			ofRect(i*width-1,ofGetHeight()-10,0,width+2,10);
			//ofLine(i,ofGetHeight()-10,i,ofGetHeight());
		}
	}

	//audioVisualizer.draw();
	ofNoFill();


	// ----------------------------
	// draw the brightness ramp, store in a polyline
	if(!brightnessMessage.empty()){
		ofSetColor(0,0,0);
		float width = float(ofGetWidth())/float(brightnessMessage.size());
		ofBeginShape();
		for (int i = 0; i < brightnessMessage.size(); i++){
			ofVertex(i*width, ofGetHeight()-ofMap(brightnessMessage[i],0,1,50,80));
		}
		ofEndShape();

		// ----------------------------
		// get data back from spline.
		vector < ofPoint > ptsBack = SC.dataFromSpline(splineBriInfo);
		width = float(ofGetWidth())/float(ptsBack.size());
		// ----------------------------
		// draw that.
		ofSetColor(0,0,255);
		ofBeginShape();
		for (int i = 0; i < ptsBack.size(); i++){
			ofVertex(ptsBack[i].x*width, ofGetHeight()-ofMap(ptsBack[i].y,0,1,50,90));
		}
		ofEndShape();
	}



	// ----------------------------
	// draw the hue ramp,
	/*if(!hueDiffMessage.empty()){
		ofSetColor(0,0,0);
		float width = float(ofGetWidth())/float(hueDiffMessage.size());
		ofBeginShape();
		for (int i = 0; i < hueDiffMessage.size(); i++){
			ofVertex(i*width, ofGetHeight()-ofMap(hueDiffMessage[i],0,1,90,130));
		}
		ofEndShape();

		// ----------------------------
		// get data back from spline.
		vector < ofPoint > ptsBack = SC.dataFromSpline(splineHueInfo);
		width = float(ofGetWidth())/float(ptsBack.size());

		// ----------------------------
		// draw that.
		ofSetColor(0,0,255);
		ofBeginShape();
		for (int i = 0; i < ptsBack.size(); i++){
			ofVertex(ptsBack[i].x*width, 20+ ofMap(ptsBack[i].y,0,1,90,140));
		}
		ofEndShape();
	}*/
	mutex.unlock();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
