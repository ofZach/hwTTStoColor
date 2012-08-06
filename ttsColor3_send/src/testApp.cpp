#include "testApp.h"
#include "sndfile.hh"

ofPoint midiColor;


//#ifdef XBEE_SEND

void testApp::setupXbee(){
    
    XB.high = "0x0013a200";
    XB.low =  "0x406C77CB";
    XB.local = "0xC0A9";
    
    
    serial.listDevices();
    serial.setup(0,57600);
    
    XBEE.Serial = &serial;
    XBEE_SR.Serial = &serial;

    
}


void testApp::newMidiMessage(ofxMidiMessage& msg) {
    
    midiMessage = msg;
    //cout << msg.value << endl;
    //cout << msg.control << endl;
    if (msg.control == 0){
        midiColor.x = ofMap(msg.value, 0,127,0,253, true);
        //printf("adjusting red\n");
        //balloonTarget.r = ofMap(msg.value, 0,127,0,255, true);
    }
    
    if (msg.control == 1){
        printf("adjusting green\n");
        midiColor.y = ofMap(msg.value, 0,127,0,253, true);
        //balloonTarget.g = ofMap(msg.value, 0,127,0,255, true);
    }
    
    if (msg.control == 2){
        printf("adjusting blue\n");
        midiColor.z = ofMap(msg.value, 0,127,0,253, true);
        //balloonTarget.b = ofMap(msg.value, 0,127,0,255, true);
    }
    
}



unsigned char colorData[7];      // r,g,b,time,time,mode;


void preparePacket (ofPoint temp, int fade){
    
    
    colorData[0] = (unsigned char)255;          // nothing yet. 
    colorData[1] = (unsigned char)0;            // nothing yet. 
    colorData[2] = (unsigned char) temp.x;
    colorData[3] = (unsigned char) temp.y;
    colorData[4] = (unsigned char) temp.z;
    colorData[5] = (unsigned char)fade;    // smoothing! 
    colorData[6] = (unsigned char)254;
}


void testApp::sendColorData(){
    
    byte data[7];
    
    
    // BALLOON2:
    long destination[2];
    
    
    for (int i = 0; i < xbees.size(); i++){
        
        ofPoint colorz;
        colorz.x = lights[i]->color.x;
        colorz.y = lights[i]->color.y;
        colorz.z = lights[i]->color.z;
        
        if (colorz.x < 0) colorz.x = 0;
        if (colorz.y < 0) colorz.y = 0;
        if (colorz.z < 0) colorz.z = 0;
        if (colorz.x > 254) colorz.x = 254;
        if (colorz.y > 254) colorz.y = 254;
        if (colorz.z > 254) colorz.z = 254;
        
        //cout << i << " " << color << endl;
        preparePacket(colorz, balloonFade);
        
        for (int j = 0; j < 7; j++){
            data[j] = colorData[j];
        }
        
        
        destination[0] = ofHexToInt(xbees[i].high);
        destination[1] = ofHexToInt(xbees[i].low);
        int local = ofHexToInt(xbees[i].local);
        XBEE_SR.sendData(data, 7, destination,local);
        XBEE_SR.Serial->flush();
        
    }
}


//#endif


//--------------------------------------------------------------
void testApp::setup(){

  //  #ifdef XBEE_SEND
        setupXbee();
   // #endif
    
    
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
    
    
    
    
    
    GUI.addSlider("delay", ofRectangle(10,340+160,180,30), 0,200,10, &delay);
    GUI.addSlider("balloonFade", ofRectangle(10,340+200,180,30), 0,255,10, &balloonFade);
    
    
    
    
       
    left = new float[256];
	
    AA.setup(44100);    // sample rate!
    ACM.setup();
    
    
    
    bPlaying = false;

    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);	
	
    ofSetFrameRate(30);
    
    
    
    
    // ------- midi 
    
    // print input ports to console
    midiIn.listPorts();
    
    // open port by number
    midiIn.openPort(0);
    //midiIn.openPort("IAC Pure Data In");	// by name
    //midiIn.openVirtualPort("ofxMidiIn Input");	// open a virtual port
    
    // don't ignore sysex, timing, & active sense messages,
    // these are ignored by default
    midiIn.ignoreTypes(false, false, false);
    
    // add testApp as a listener
    midiIn.addListener(this);
    
    // print received messages to the console
    midiIn.setVerbose(true);
    
    midiColor.set(0,0,0);
    
    loadXbeesFromFile();
    
    for (int i = 0; i < xbees.size(); i++){
        light * temp = new light();
        lights.push_back(temp);
        if (i > 0){
            lights[i-1]->nextOne = temp;
        }
        
    }
    

    
    
    delay = 10;
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
    
    
    lights[0]->color = temp;
    
    
//#ifdef XBEE_SEND
    //if (ofGetFrameNum() % 10 == 0){
        sendColorData();
   // }
//#endif

    for (int i = 0; i < lights.size(); i++){
        lights[i]->update();
    }
    
    
    for (int i = 0; i < lights.size(); i++){
        lights[i]->delayLength = (int)delay;
        
    }
    
    
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
    
    
    
    for (int i = 0; i < lights.size(); i++){
       
        ofFill();
        lights[i]->draw(200 + i * 110 + 50, 100);
        ofSetColor(255,255,255);
        ofNoFill();
        ofRect(200 + i * 110 + 50, 100, 100,100);
    }
    
    
    
    
    
    for (int i = 0; i < colorsForMessage.size(); i++){
        ofSetColor(colorsForMessage[i]);
        ofLine(i,0,i,10);
    }
    
    
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
    
    float samples[256];
    
    audioAnalysisFrame aaFrameTemp;
    audioAnaylzer AAtemp;
    AAtemp.setup(44100);
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
    }

    
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

//--------------------------------------------------------------
void testApp::writeXbeesToFile(){
    
    ofxXmlSettings xml;
    
    for (int i = 0; i < xbees.size(); i++){
        xml.addTag("xbee");
        xml.pushTag("xbee", i);
        xml.addTag("address");
        xml.pushTag("address");
        xml.addValue("high", xbees[i].high);
        xml.addValue("low", xbees[i].low);
        xml.addValue("local", xbees[i].local);
        xml.popTag();
        xml.popTag();
    }
    
    xml.saveFile("settings.xml");
    
}
//--------------------------------------------------------------
void testApp::loadXbeesFromFile(){
    
    
    ofxXmlSettings xml;
    xml.loadFile("settings.xml");
    int nXbees = xml.getNumTags("xbee");
    
    
    for (int i = 0; i < nXbees; i++){
        
        xbee temp;
        
        xml.pushTag("xbee", i);
        xml.pushTag("address");
        temp.high =  xml.getValue("high", "");
        temp.low =  xml.getValue("low", "");
        temp.local =  xml.getValue("local", "");
        xml.popTag();
        xml.popTag();
        
        xbees.push_back(temp);
        
    }
    
}


