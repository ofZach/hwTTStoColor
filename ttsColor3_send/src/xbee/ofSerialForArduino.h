//
//  ofSerialForArduino.h
//  guiExample
//
//  Created by molmol on 4/21/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef guiExample_ofSerialForArduino_h
#define guiExample_ofSerialForArduino_h

#include "ofMain.h"

class ofSerialForArduino : public ofSerial {
    
public: 
    
    
    
    unsigned char read(){
        return readByte();
    }
    
    void println(string temp){
                
    }
    
    void println(unsigned char temp, int type){
    }
    
    void print(unsigned char temp, int type){
        writeByte(temp);
    }
    
    void println(unsigned char * str){
        
    }
    
    void println(unsigned long  str){
        
    }
    
   
    
    
    
};


#endif
