
#include "xbeeController.h"

void xbeeController::setupXbee(){
    
    
    serial.listDevices();
    serial.setup(0,57600);
    
    XBEE.Serial = &serial;
    XBEE_SR.Serial = &serial;
    
    
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


void xbeeController::sendColorData(){
    
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


void xbeeController::setup(){
    
    setupXbee();
    
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

void xbeeController::update(ofPoint temp){
    
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
    
}

void xbeeController::draw(){
    
    for (int i = 0; i < lights.size(); i++){
        
        ofFill();
        lights[i]->draw(200 + i * 110 + 50, 100);
        ofSetColor(255,255,255);
        ofNoFill();
        ofRect(200 + i * 110 + 50, 100, 100,100);
    }
    
}


//--------------------------------------------------------------
void xbeeController::writeXbeesToFile(){
    
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
void xbeeController::loadXbeesFromFile(){
    
    
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


