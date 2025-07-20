/*
   RemoteXY example shows how to use any data stream. Any class inherited from 
   the Arduino Stream class can be used with RemoteXY.
   
   To connect, use the RemoteXY mobile 
   application at http://remotexy.com/en/download/               
    
   Copyright (c) 2014-2025 Evgenii Shemanuev
   Licensed under the MIT License. See LICENSE file in the project root for 
   full license information.   
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