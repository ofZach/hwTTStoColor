#include "testApp.h"
#include "sndfile.hh"



//--------------------------------------------------------------
void testApp::setup(){

    
    fadeActiveVal = 0;
    fadeToBlack = 0;
    
  
    // --------------- setup the gui window
    GUI.setup(ofRectangle(0,0,500,800), "my sample gui");
    GUI.setEventInterface(this);
    GUI.addButton("repeatSound", "repeat sound", ofRectangle(10,10,120,30));
    GUI.addButton("newColor", "new color", ofRectangle(10,50,120,30));
    GUI.addTextEntry("textBox", "please enter text here", ofRectangle(10,120,180,90));
    
    
    GUI.addSlider("volSmth", ofRectangle(10,260,180,30), 0, 1, 0.8, &ACM.volumeSmoothing);
    GUI.addSlider("hueRange", ofRectangle(10,300,180,30), 0,400,35, &ACM.hueRange);
    GUI.addSlider("hueShiftValStart", ofRectangle(10,340,180,30), 0,1,0.2, &ACM.hueShiftValStart);
    GUI.addSlider("darkness", ofRectangle(10,340+40,180,30), 0,1,0.5, &ACM.darkness);
    GUI.addSlider("hueFadeRate", ofRectangle(10,340+80,180,30), 0,1,0.96, &ACM.hueFadeRate);
    GUI.addSlider("fadeToBlack", ofRectangle(10,340+120,180,30), 0,1,1, &fadeToBlack);
    
    
       AAtemp.setup(44100);
    
    
    //GUI.addSlider("delay", ofRectangle(10,340+160,180,30), 0,200,10, &XBC.delay);
    //GUI.addSlider("balloonFade", ofRectangle(10,340+200,180,30), 0,255,10, &XBC.balloonFade);
    
    
    
    
       
    left = new float[256];
	
    AA.setup(44100);    // sample rate!
    ACM.setup();
    
    
    
    bPlaying = false;

    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);	
	
    ofSetFrameRate(30);
    
    
    
   
}



//--------------------------------------------------------------
void testApp::update(){
    if (ofGetFrameNum() == 2) GUI.show();
    
    ofVec3f temp;
    temp.x = colorAltered.r;
    temp.y = colorAltered.g;
    temp.z = colorAltered.b;
    
    if (fadeToBlack > 0.5){
        temp *= fadeActiveVal;
    }
    
    if (bPlaying == true){
        fadeActiveVal = 0.9f * fadeActiveVal + 0.1 * 1.0;
    } else {
        fadeActiveVal = 0.9f * fadeActiveVal + 0.1 * 0.0;
    }
    ofBackground(temp.x, temp.y, temp.z);
    
   
    
    
    //if (temp2.getNormalized() > 0.33) ofBackground(100, 100, 100);
    //else ofBackground(50,50,50);
}


//--------------------------------------------------------------
void testApp::draw(){
    
       
    ofDrawBitmapStringHighlight(ofToString(aaFrame.volume), ofPoint(30,30));
    
    //temp.draw(30,100);
    ofNoFill();
    ofSetColor(255,255,255);
    ofRect(30,100, 128,32);
    ofBeginShape();
    for (int i = 0; i < ACM.volume.size(); i++){
        ofVertex(30+i, 100 + 32 - ofClamp(32*ACM.volume[i]*5,0,32));
    }
    ofEndShape();
    
    
    ofNoFill();
    ofRect(30,150, 128,32);
    ofBeginShape();
    for (int i = 0; i < ACM.volumeTrigger.size(); i++){
        ofVertex(30+i, 150 + 32 - 32*ACM.volumeTrigger[i]);
    }
    ofEndShape();
    
    
    
    //temp2.draw(30,150);
    ACM.pitch.draw(30,200);
    
    ofFill();
    ofSetColor(255,255,255);
    for (int i = 0; i < AA.FFTanalyzer.nAverages; i++){
		ofRect(200+(i*20),600,20,-AA.FFTanalyzer.averages[i] * 6);
	}
	
	ofSetColor(255,0,0);
	for (int i = 0; i < AA.FFTanalyzer.nAverages; i++){
		ofRect(200+(i*20),600-AA.FFTanalyzer.peaks[i] * 6,20,-4);
	}
    
    
    
    ofSetColor(0,255,255);
    
    for (int i = 0; i < AA.FFTanalyzer.nAverages; i++){
        int who = i / 3;
        //cout << peaks[who] << endl;
		ofRect(200+(i*20),600-ACM.peaks[who] * 6,20,-4);
	}
    
    
    
    
    
    
    
    
    for (int i = 0; i < colorsForMessage.size(); i++){
        ofSetColor(colorsForMessage[i]);
        ofLine(i,0,i,10);
    }
    
    ofNoFill();
    
    
    // ----------------------------
    // draw the brightness ramp, store in a polyline
    ofSetColor(0,0,0);
    ofBeginShape();
    for (int i = 0; i < brightnessMessage.size(); i++){
        ofVertex(i, ofMap(brightnessMessage[i],0,1,50,80));
    }
    ofEndShape();
    
    // ----------------------------
    // get data back from spline. 
    vector < ofPoint > ptsBack = SC.dataFromSpline(splineBriInfo);
    
    // ----------------------------
    // draw that. 
    ofPushMatrix();
    ofSetColor(0,0,255);
    ofBeginShape();
    for (int i = 0; i < ptsBack.size(); i++){
        ofVertex(ptsBack[i].x, ofMap(ptsBack[i].y,0,1,50,90));
    }
    ofEndShape();
    ofPopMatrix();
    
    
    
    // ----------------------------
    // draw the hue ramp,
    ofSetColor(0,0,0);
    ofBeginShape();
    for (int i = 0; i < hueDiffMessage.size(); i++){
        ofVertex(i, ofMap(hueDiffMessage[i],0,1,90,130));
    }
    ofEndShape();
    
    // ----------------------------
    // get data back from spline. 
    ptsBack = SC.dataFromSpline(splineHueInfo);
    
    // ----------------------------
    // draw that. 
    ofPushMatrix();
    ofSetColor(0,0,255);
    ofBeginShape();
    for (int i = 0; i < ptsBack.size(); i++){
        ofVertex(ptsBack[i].x, 20+ ofMap(ptsBack[i].y,0,1,90,140));
    }
    ofEndShape();
    ofPopMatrix();
    
    
       
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


//--------------------------------------------------------------
void testApp::audioRequested (float * output, int bufferSize, int nChannels){	
	
   
    for (int i = 0; i < bufferSize; i++){
        if (bPlaying == true){
            
            
            
            output[i*2] =  audioSamples[counter];		
            output[i*2+1] = audioSamples[counter];	
            
            counter++;
            
            if (counter > audioSamples.size()){
                counter = 0;
                bPlaying = false;
            }
            
        } else {
            
            output[i*2] =  0; //ofRandom(-0.1, 0.1);	 // random noise when not playing a speech text.	
            output[i*2+1] = 0; // ofRandom(-0.1, 0.1);	
        }
        
        left[i] = output[i*2];
       
	}
    
    AA.analyzeFrame(left, bufferSize, aaFrame);
    colorAltered = ACM.update(aaFrame, color);
    
}



// this is very ghetto, because we're competeing with threaded stuff, etc. 
// just helps to see a complete color profile. 

void testApp::computeMessageColors(){
    
    
    colorsForMessage.clear();
    
    brightnessMessage.clear();
    hueDiffMessage.clear();
    
    float samples[256];
    
    audioAnalysisFrame aaFrameTemp;
    audioToColorMapper ACMtemp;
    ACMtemp.setup();
    ACMtemp = ACM;
    
    
    int audioCounter = 0;
    while (audioCounter < audioSamples.size()){
        
        int bufferSize = 256;
        for (int i = 0; i < bufferSize; i++){
           samples[i]  =  audioSamples[audioCounter];		
           audioCounter++;
            
        }
        AAtemp.analyzeFrame(samples, bufferSize, aaFrameTemp);
        colorAltered = ACMtemp.update(aaFrameTemp, color);
        colorsForMessage.push_back(colorAltered);
        
        brightnessMessage.push_back(ofMap(colorAltered.getBrightness() - color.getBrightness(), 0, 255, 0,1));
        hueDiffMessage.push_back(ofMap(color.getHue() - colorAltered.getHue(), -ACM.hueRange, ACM.hueRange, 0,1));
        
    }

    
    // now, let's do some spline fitting!
    
    
    splineBriInfo = SC.splineFromData(brightnessMessage,  80);
    splineHueInfo = SC.splineFromData(hueDiffMessage,  80);
    
    // print out info!
    printf("\n");
    printf("\n");

    printf("bri: \n");
    printf("splineInfo: %f, %f, %i, %i  \n", splineBriInfo.DX, splineBriInfo.mean, (int)splineBriInfo.xmin, (int)splineBriInfo.xmax);
    printf("coefficients: ");
    for (int i = 0; i <= splineBriInfo.M; i++){
        printf("%f", splineBriInfo.coefficients[i]);
        if (i != splineBriInfo.M) printf(",");
    }
    printf("\n");
    printf("\n");
    printf("hue: \n");
    printf("splineInfo: %f, %f, %i, %i  \n", splineHueInfo.DX, splineHueInfo.mean, (int)splineHueInfo.xmin, (int)splineHueInfo.xmax);
    printf("coefficients: ");
    for (int i = 0; i <= splineHueInfo.M; i++){
        printf("%f", splineHueInfo.coefficients[i]);
        if (i != splineHueInfo.M) printf(",");
    }
    printf("\n");
    printf("\n");

    
    
}



//--------------------------------------------------------------
void testApp::guiEvent(nativeWidget & widget){
    
    ofDisableDataPath();
    ofEnableDataPath();
    
    if (widget.name == "newColor"){
        
        float hue = ofRandom(0,255);
        float sat = ofRandom(190,230);
        float bri = ofRandom(220,238);
        color.setHsb(hue, sat, bri);
        
        
    }
    
    if (widget.name == "repeatSound"){
        if (audioSamples.size() > 0){
            counter = 0;
            bPlaying = true;
        }
        
    }
    
    if (widget.name == "textBox" || widget.name == "textBox2" || widget.name == "textBox3"
        ){
        
        string time = ofGetTimestampString();
        string fileName = time + ".aiff";
        string fileNameMp3 = time + ".mp3";
        string toSay = *((string *)widget.variablePtr);
        string command = "say -o " + ofToDataPath(fileName) + " " + "\"" + toSay + "\"" + " --data-format=BEI32@44100";
            
        // big endian int 32 bit samples 44100 sample rate
        
        system(command.c_str());
        
        ofSleepMillis(100);         // sometimes really long files need time to save out. 
        
        SndfileHandle myf = SndfileHandle(ofToDataPath(fileName).c_str());
        
        float * data = new float[int(myf.frames())];
        myf.read (data, int(myf.frames()));                          
        audioSamples.clear();
        audioSamples.reserve(int(myf.frames()));
        for (int i = 0; i < int(myf.frames()); i++){
            audioSamples.push_back(data[i]);
        }
        delete [] data;
        
        bPlaying = true;
        counter = 0;
         
    }
    
    computeMessageColors();
}

