/*
 *  xbeeATst.cpp
 *  xbeeLibrary

  Copyright (c) Justin Downs 12/9/08 All right reserved.
  for more info/tutorials visit
  www.johnHenrysHammer.com/WOW/
  or
  www.GRNDLAB.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#include "xbeeATst.h"

#define BYTE 0


//constructor
xbeeATst::xbeeATst(){
packetLength = 0;
highAddress = 0;
lowAddress = 0;
for (int i=0;i<2; i++){ATcommand[i]='*';} // 2 AT max Init with known args
}

//************************************************************************************************
bool xbeeATst::sendLocalAtCommand(char* _ATcommand) {
  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++;                                             // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1;                         //skip zero because this disables status response packets

  int CommandLength = (strlen(_ATcommand));              // length of ATcommand

  //******************************
  //calculate length of the packet
  //  Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  // (API identifier + frameId) = 2 "plus +" (ATcommand) = CommandLength
  int messageLength = (CommandLength + 2);

  // A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //  To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial->print(0x7E, BYTE);             // send start delimiter
  Serial->print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial->print(messageLength, BYTE);    // send length LSB
  Serial->print(0x08, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x08;           // minus command identifier from checksum FF
  Serial->print(frameID, BYTE);          // send frame ID (set to 0 if noresponse is required)
  checksum = checksum - frameID;

  //******************************
  //AT command
    
    
    if (CommandLength > 2 ){
        for (int i = 0; i <   CommandLength; i++){
            Serial->print(_ATcommand[i], BYTE);    // send ATcommand
            checksum = checksum - _ATcommand[i];
        }
        Serial->print(checksum,BYTE);          //  send checksum
    } else {
        Serial->print(_ATcommand[0], BYTE);    // send ATcommand
        checksum = checksum - _ATcommand[0];
        Serial->print(_ATcommand[1], BYTE);
        checksum = checksum - _ATcommand[1];
        Serial->print(checksum,BYTE);          //  send checksum
    }
    
    
  
  return true;
}
//************************************************************************************************

bool xbeeATst::sendLocalAtCommandWithOptions (char* _ATcommand, char* _AToptions,int _optionsize) {

  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++; // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1; //skip zero because this disables status response packets

  int CommandLength = (strlen(_ATcommand)); // length of ATcommand
  int AToptionLength =  _optionsize; // if there are options count them
  CommandLength += AToptionLength;

  //******************************
  //calculate length of the packet
  //  Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  // (API identifier + frameId) + 2 "plus +" (ATcommand + command options) = CommandLength
  int messageLength = (CommandLength + 2);

  // A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //  To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial->print(0x7E, BYTE);             // send start delimiter
  Serial->print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial->print(messageLength, BYTE);    // send length LSB
  Serial->print(0x08, BYTE);             // send API command identifier **ATlocal= 0x08**
  checksum = checksum - 0x08;           // minus command identifier from checksum FF
  Serial->print(frameID, BYTE);          // send frame ID (set to 0 if no response is required)
  checksum = checksum - frameID;

  //******************************
  //command options
  Serial->print(_ATcommand[0], BYTE); // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial->print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  //******************************
  //send options
  if(AToptionLength != 0){
    for (int i = 0; i < AToptionLength; i++){
      Serial->print(_AToptions[i] ,BYTE);
      checksum = checksum - _AToptions[i];
    }
  }

  Serial->print(checksum,BYTE);       //  send checksum
  return true;
}


//************************************************************************************************
bool xbeeATst::getLocalATCommandData(int _timeOut){

  unsigned int timeout = _timeOut;// bail out time
  long startTime;

  byte inByte=0x00;
  startTime = ofGetElapsedTimeMillis();
  //***************
//LOOKING FOR START BYTE AND PACKET LENGTH
while (inByte != 0x7E) {                            // clears out the buffer of junk
  if(Serial->available() < 1) return 0;              // no more and didn't find it
  inByte = Serial->read();                           // read a byte from the buffer
  if((ofGetElapsedTimeMillis() - startTime) > timeout)return 0;     // timed out bad message
}

  //***************
  //look for start byte 0x7E(~)
  if (inByte == 0x7E) {                             // if we got the API start byte
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 2){                 // block for length byte two bytes to be available
      if((ofGetElapsedTimeMillis() - startTime) > timeout)return 0; // timed out bad message
	}
    if (Serial->available() > 1){                    // if we got 2 bytes
      int lengthMSB = Serial->read();                // read the most significant length byte
      int lengthLSB = Serial->read();                // read the least significant length byte
      packetLength = (lengthMSB << 8) + lengthLSB;  // put the two bytes together
    }
  }

  //***************
  //  APIDENTIFIER
  if (packetLength > 0) {                            //IF WE HAVE A FULL PACKET
    apiIdentifier = 'Z';                             // set apiIdentifier to an impossible value
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 1){                  // wait for a byte or timeout
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0; // timed out bad message
    }

    if (Serial->available() > 0) {                    // if a byte is waiting in the buffer set it to the API
      apiIdentifier = (unsigned char)(Serial->read());
    }
    else return 0;                                   //else bad message


    //***************
    //FRAME ID
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 1){                 // wait for a byte or timeout
      if((ofGetElapsedTimeMillis() - startTime) > timeout)return 0; // timed out bad message
    }
    if (Serial->available() > 0) {                   // if a byte is waiting in the buffer set it to the API
      frameID = (unsigned char)(Serial->read());
    }
    else return 0;                                  // else bad message


    //***************
    //GET AT COMMAND BYTE
    startTime = ofGetElapsedTimeMillis();
    char ATcommandByte = '?';                       // what is it?
    while( Serial->available() < 3 ){                // stop for bits
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;// timed out bad message
    }
    if (Serial->available() > 1) {                   // if a byte is waiting in the buffer
      for (int i = 0; i<2; i++){                    // two command bytes
        ATcommandByte =(unsigned char)(Serial->read());
        ATcommand[i]=ATcommandByte;
      }
    }
    else return 0;                                  // timed out bad message


    //***************
    //GET STATUS/options
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 1){
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;        // timed out bad message

    }
    status = (unsigned char)(Serial->read());                           // get the options/status 0 = OK 1 = Error 2 = Invalid Command 3 = Invalid Parameter


    //***************
    //GET DATA
    startTime = ofGetElapsedTimeMillis();
    dataLength = (packetLength - 5);						//record the length of incoming data
    while (Serial->available() < (packetLength - 5)){
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;
    }
    static bool goneOnce = false;						// flag to not free the first time
    if (goneOnce)free(receivedData);						// if it has been allocated Free the memory
    else goneOnce = true;
    receivedData = (byte*) malloc(packetLength - 5);        // get memory, be sure to call free in sendData or it blows real quick
    if (receivedData == NULL){                                                  // if we couldn't get memory

      return 0; // couldn't allocate
    }

    for (int i= 0; i < (packetLength - 5); i++) {			// take out other info in packet + checksum

      receivedData [i] = (unsigned char)(Serial->read());				// get a byte of data
    }

    //***************
    //GET CHECKSUM
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 1){
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;        // timed out bad message
    }
    checksum = (unsigned char)(Serial->read());

      
      
      printf("packetLength \n");
      printf("%x \n",packetLength);
      printf("apiIdentifier \n");
      printf("%x \n",apiIdentifier);
      printf("frameID \n");
      printf("%x \n",frameID);
      printf("Atcommand \n");
      for (int i= 0; i < 2; i++) {  
      printf("%x \n",ATcommand[i]);
      }
      
      printf("status \n");
      printf("%x \n",status);

      printf("info \n");
      for (int i= 0; i < (packetLength - 5); i++) {           // take out other info in packet + checksum
        printf("%x \n", receivedData[i]);
      }
      
      
//    //***************
//    //PRINTALL
//#ifdef XBEEDEBUG
//    Serial->println("packetLength");
//    Serial->println(packetLength,HEX);
//    Serial->println("apiIdentifier");
//    Serial->println(apiIdentifier,HEX);
//    Serial->println("frameID");
//    Serial->println(frameID,HEX);
//    Serial->println("Atcommand");
//    for (int i= 0; i < 2; i++) {                            // take out other info in packet + checksum
//      Serial->println(ATcommand[i],HEX);
//    }
//    Serial->println("status");
//    Serial->println(status,HEX);
//    Serial->println("info");
//    for (int i= 0; i < (packetLength - 5); i++) {           // take out other info in packet + checksum
//      Serial->println(receivedData[i],HEX);
//    }
//    Serial->println("checksum  ");
//    Serial->println(checksum,HEX);
//    Serial->println("END");
//#endif

    return 1;                                               // if everything went well return a true
  }
  return 0;                                                 // something went wrong
}
//************************************************************************************************


//************************************************************************************************
bool xbeeATst::sendAtCommandWithOptions (long* _destination, char* _ATcommand, char* _AToptions,int _optionsize, int _localAddress ) {

  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++;                                    // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1;                //skip zero because this disables status response packets

  int CommandLength = strlen(_ATcommand);       // length of ATcommand
  int AToptionLength = _optionsize;             // if there are options count them
  CommandLength += AToptionLength;

  //******************************
  //calculate length of the packet
  //  Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  // (API identifier + frameId) = 2 "plus +" (ATcommand + command options) = CommandLength
  int messageLength = (CommandLength + 13);

  // A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //  To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial->print(0x7E, BYTE);             // send start delimiter
  Serial->print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial->print(messageLength, BYTE);    // send length LSB
  Serial->print(0x17, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x17;           // minus command identifier from checksum FF
  Serial->print(frameID, BYTE);          // send frame ID (set to 0 if no response is required)
  checksum = checksum - frameID;

  //******************************
  //SEND hardware Adress
  // set  the 64 bit factory address long high byte then low
  for(int i=0;i < 2; i++){                                 // address never bigger than 2
    for (int j = 4; j > 0; j--) {
      byte destinationByte = _destination[i] >> 8 * (j-1); // shift over one byte at a time, MSB first 4 for long
      Serial->print(destinationByte, BYTE);                 // send destination address
      checksum = checksum - destinationByte;
    }
  }

  //******************************
  // break up local address and send it
  byte localLow = _localAddress;
  byte localHigh = _localAddress >> 8;
  //send 16-bit address as unkown FUNCTION
  Serial->print(localHigh,BYTE);                             //send unknown for 16-bit address FF
  checksum = checksum - localHigh;
  Serial->print(localLow,BYTE);                              //send unknown for 16-bit address  FE
  checksum = checksum - localLow;
  //******************************
  // sets when to run command
  Serial->print(0x02,BYTE);                                  //send 0x02 to run command when received. IF not set the AT command "AC" must be sent to set and run.
  checksum = checksum - 0x02;
  //******************************
  //command options
  Serial->print(_ATcommand[0], BYTE);                        // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial->print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  //******************************
  //send options
  if(AToptionLength != 0){
    for (int i = 0; i < AToptionLength; i++){
      Serial->print(_AToptions[i] ,BYTE);
      checksum = checksum - _AToptions[i];

    }
  }

  Serial->print(checksum,BYTE);                              //  send checksum
  return true;
}
//************************************************************************************************


//************************************************************************************************
bool xbeeATst::sendAtCommand(long* _destination, char* _ATcommand, int _localAddress ) {

  // set frame ID for call back
  static byte frameID = 0xA1;
  frameID++;                                                // add one to the frame ID each time, okay to overflow
  if (frameID == 0) frameID = 1;                            //skip zero because this disables status response packets

  int CommandLength = strlen(_ATcommand);                   // length of ATcommand

  //******************************
  //calculate length of the packet
  //Length = all bytes in *DATA FRAME* (no start byte, length byte or checksum)
  //(API identifier + frameId) = 2 "plus +" (ATcommand + command options) = CommandLength
  int messageLength = (CommandLength + 13);

  //A checksum is calculated and verified on non-escaped data.
  //To calculate: Not including frame delimiters and length,
  //add all bytes keeping only the lowest 8
  //bits of the result and subtract the result from 0xFF.
  //To verify: Add all bytes (include checksum, but not the delimiter and length). If the checksum is
  //correct, the sum will equal 0xFF.
  byte checksum = 0xFF; // checksums are the hex FF minus the total of all bytes
  //******************************
  // send the packet
  Serial->print(0x7E, BYTE);             // send start delimiter
  Serial->print(0x0, BYTE);              // send length MSB (always zero because packets can't be more than 100 bytes)
  Serial->print(messageLength, BYTE);    // send length LSB
  Serial->print(0x17, BYTE);             // send API command identifier **AT REMOTESEND= 0x17**
  checksum = checksum - 0x17;           // minus command identifier from checksum FF
  Serial->print(frameID, BYTE);          // send frame ID (set to 0 if no response is required)
  checksum = checksum - frameID;

  //******************************
  //SEND hardware Address
  // set  the 64 bit factory address long high byte then low
  for(int i=0;i < 2; i++){                                 // address never bigger than 2
    for (int j = 4; j > 0; j--) {
      byte destinationByte = _destination[i] >> 8 * (j-1); // shift over one byte at a time, MSB first 4 for long
      Serial->print(destinationByte, BYTE);                 // send destination address
      checksum = checksum - destinationByte;
    }
  }

  //******************************
  // break up local address and send it
  byte localLow = _localAddress;
  byte localHigh = _localAddress >> 8;
  //send 16-bit address as unkown FUNCTION
  Serial->print(localHigh,BYTE);                             //send unknown for 16-bit address FF
  checksum = checksum - localHigh;
  Serial->print(localLow,BYTE);                              //send unknown for 16-bit address  FE
  checksum = checksum - localLow;
  //******************************
  // sets when to run command
  Serial->print(0x02,BYTE);                                  //send 0x02 to run command when received. IF not set the AT command "AC" must be sent to set and run.
  checksum = checksum - 0x02;
  //******************************
  //command options
  Serial->print(_ATcommand[0], BYTE);                        // send ATcommand
  checksum = checksum - _ATcommand[0];
  Serial->print(_ATcommand[1], BYTE);
  checksum = checksum - _ATcommand[1];
  //******************************
  Serial->print(checksum,BYTE);                              // send checksum
  return true;

}
//************************************************************************************************


//************************************************************************************************
bool xbeeATst::getATCommandData(int _timeOut){

  unsigned int timeout = _timeOut;// bail out time
  long startTime;

  byte inByte=0x00;
  startTime = ofGetElapsedTimeMillis();
  //***************
//LOOKING FOR START BYTE AND PACKET LENGTH
while (inByte != 0x7E) {                                    // clears out the buffer of junk
  if(Serial->available() < 1) return 0;                      // no more and didn't find it
  inByte = Serial->read();                                   // read a byte from the buffer
  if((ofGetElapsedTimeMillis() - startTime) > timeout)return 0;             // timed out bad message
}


  //***************
  //look for start byte 0x7E(~)
  if (inByte == 0x7E) {                                     // if we got the API start byte
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 2){                         // block for length byte two bytes to be available
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;        // timed out bad message
    }
    if (Serial->available() > 1){                            // if we got 2 bytes
      int lengthMSB = Serial->read();                        // read the most significant length byte
      int lengthLSB = Serial->read();                        // read the least significant length byte
      packetLength = (lengthMSB << 8) + lengthLSB;          // put the two bytes together
    }
  }

  //***************
  //  APIDENTIFIER
  if (packetLength > 0) {                                   //IF WE HAVE A FULL PACKET
    apiIdentifier = 'Z';                                    // set apiIdentifier to an impossible value
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 1){                         // wait for a byte or timeout
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;        // timed out bad message
    }

    if (Serial->available() > 0) {                           // if a byte is waiting in the buffer set it to the API
      apiIdentifier = (unsigned char)(Serial->read());
    }
    else return 0; //else bad message

    //***************
    //FRAME ID
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 1){                         // wait for a byte or timeout
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;        // timed out bad message

    }

    if (Serial->available() > 0) {                           // if a byte is waiting in the buffer set it to the API
      frameID = (unsigned char)(Serial->read());

    }
    else return 0; //else bad message

    //***************
    //GET LONG ADDRESS
    long addrByte = 0;
    highAddress = 0;                                        // clear out address
    lowAddress = 0;
    startTime = ofGetElapsedTimeMillis();
    while(Serial->available() < 8){                          // stop for bytes
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;        // timed out bad message
    }
    if (Serial->available() > 7) {
      byte address[8];                                      // array for address
      for (int i = 0; i< 8; i++){
        addrByte = (unsigned char)(Serial->read());
        address[i] = addrByte;

        if (i < 4){                                         // get first half of address
          highAddress += (addrByte << ((3-i) * 8));         // get the first 4 bytes shift into place 4321
        }
        else {
          lowAddress += (addrByte << ((7-i) * 8));          // get second shift them int place 4321
        }

      }

    }
    else return 0;// timed out bad message

    //***************
    //GET LOCAL ADDRESS
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 2){                         // wait for at least two bytes to be available
    if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;          // timed out bad message
	}

    int localAd = Serial->read();                            // read the most significant length byte
    int localAd2 = Serial->read();                           // read the least significant length byte
    localAddress = (localAd << 8) + localAd2;               // put the two bytes together

    //***************
    //GET AT COMMAND BYTE
    startTime = ofGetElapsedTimeMillis();
    char ATcommandByte = '?';                               //what is it?
    while( Serial->available() < 3 ){                        // stop for 2 bytes
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;        // timed out bad message
    }

    if (Serial->available() > 1) {                           // if a byte is waiting in the buffer
      for (int i = 0; i<2; i++){                            // two command bytes
        ATcommandByte= (unsigned char)(Serial->read());
        ATcommand[i]= ATcommandByte;
      }
    }
    else return 0;                                          // timed out bad message

    //***************
    //GET STATUS/options
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 1){
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;        // timed out bad message

    }
    status = (unsigned char)(Serial->read());                           // get the options/status 0 = OK 1 = Error 2 = Invalid Command 3 = Invalid Parameter


    //***************
    //GET DATA
    startTime = ofGetElapsedTimeMillis();
    dataLength = (packetLength - 15);						//record the length of incoming data
    while (Serial->available() < (packetLength - 15)){
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;
    }

    static bool goneOnce = false;						// flag to not free the first time
    if (goneOnce)free(receivedData);						// if it has been allocated Free the memory
    else goneOnce = true;
    receivedData = (byte*) malloc(packetLength - 15);       // get memory, be sure to call free in sendData or it blows real quick
    if (receivedData == NULL) return 0;                     // if we couldn't get memory

    for (int i= 0; i < (packetLength - 15); i++) {			// take out other info in packet + checksum
      receivedData [i] = (unsigned char)(Serial->read());				// get a byte of data
    }


    //***************
    //GET CHECKSUM
    startTime = ofGetElapsedTimeMillis();
    while (Serial->available() < 1){
      if((ofGetElapsedTimeMillis() - startTime) > timeout) return 0;         // timed out bad message
    }
    checksum = (unsigned char)(Serial->read());


      
      printf("packetLength \n");
      printf("%x \n",packetLength);
      printf("apiIdentifier \n");
      printf("%x \n",apiIdentifier);
      printf("frameID \n");
      printf("%x \n",frameID);
      printf("Atcommand \n");
      for (int i= 0; i < 2; i++) {  
          printf("%x \n",ATcommand[i]);
      }
      
      printf("status \n");
      printf("%x \n",status);
      
      printf("info \n");
      for (int i= 0; i < (packetLength - 5); i++) {           // take out other info in packet + checksum
          printf("%x \n", receivedData[i]);
      }
      


//    //***************
//    //PRINTALL
//#ifdef XBEEDEBUG
//    Serial->println("packetLength");
//    Serial->println(packetLength,HEX);
//    Serial->println("apiIdentifier");
//    Serial->println(apiIdentifier,HEX);
//    Serial->println("frameID");
//    Serial->println(frameID,HEX);
//    Serial->println("localAddress");
//    Serial->println(localAddress,HEX);
//    Serial->println("Address");
//    Serial->println(highAddress,HEX);
//    Serial->println(lowAddress,HEX);
//    Serial->println("Atcommand");
//    for (int i= 0; i < 2; i++) {                                // take out other info in packet + checksum
//      Serial->println(ATcommand[i],HEX);
//    }
//    Serial->println("status");
//    Serial->println(status,HEX);
//    Serial->println("info");
//    for (int i= 0; i < (packetLength - 15); i++) {              // take out other info in packet + checksum
//      Serial->println(receivedData[i],HEX);
//    }
//    Serial->println("checksum  ");
//    Serial->println(checksum,HEX);
//    Serial->println("END");
//#endif

    return 1;                                                   // if everything went well return a true
  }
  return 0;                                                     // somthing went wrong
}
//************************************************************************************************
