/*
   The example shows how any data stream can be used.
   Any class inherited from the Arduino Stream class can be connected to RemoteXY
   
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.11 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.11.4 or later version;
     - for iOS 1.9.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/


#include <RemoteXY.h>


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 28 bytes
  { 255,1,0,1,0,21,0,16,31,1,70,16,16,55,34,34,26,37,0,1,
  0,15,8,34,34,2,31,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t button_1; // =1 if button pressed, else =0 

    // output variables
  uint8_t led_1; // led state 0 .. 1 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

CRemoteXY *remotexy;

void setup() 
{

  Serial.begin (9600);

  remotexy = new CRemoteXY (
    RemoteXY_CONF_PROGMEM, 
    &RemoteXY, 
    &Serial
  ); 
  
  
  // TODO you setup code
  
}

void loop() 
{ 
  remotexy->handler ();
  
  RemoteXY.led_1 = RemoteXY.button_1;
  
  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay(), use instead remotexy->delay() 


}