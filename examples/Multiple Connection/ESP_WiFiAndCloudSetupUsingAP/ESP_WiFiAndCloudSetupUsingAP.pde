/*
   Example to setup WiFi client and cloud server settings using open access point mode 
   for ESP32 or ESP8266
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.1 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.10.1 or later version;
     - for iOS 1.8.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/
//#define REMOTEXY__DEBUGLOG

#include <EEPROM.h>
#if defined (ESP8266)
  #include <ESP8266WiFi.h>
#elif defined (ESP32)
  #include <WiFi.h>  
#endif

#include <RemoteXY.h>

#define PIN_BUTTON_WIFISETUP 3   // pin of button for wifi settings, press this button when devise reset
#define REMOTEXY_SERVER_PORT 6377
#define DEVICE_NAME "RemoteXY"

#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com" // url of cloud server, does not change
#define REMOTEXY_CLOUD_PORT 6376 // port for cloud server, does not change

#pragma pack(push, 1)
////////////////////////////////////////
// WiFi+Cloud settings configuration
////////////////////////////////////////

#define ssidLength 21 // 20 chars max + zero
#define passLength 21  // 20 chars max + zero
#define tokenLength 33 // 32 chars max + zero
#define checkLength 1  // 1 byte
#define EPPROMLength ssidLength+passLength+tokenLength+checkLength

#define ssid_EPPROM 0x0000 
#define pass_EPPROM  ssid_EPPROM+ssidLength 
#define token_EPPROM ssid_EPPROM+ssidLength+passLength 
#define check_EPPROM ssid_EPPROM+ssidLength+passLength+tokenLength
#define checkValueForEPPROM 0x55

uint8_t const PROGMEM RemoteXY_CONF_WiFiSettings [] =
  { 255,76,0,0,0,149,0,15,172,2,
  7,4,28,9,47,6,9,15,46,6,
  24,31,2,21,7,4,28,21,47,6,
  9,30,46,6,24,31,2,21,129,0,
  28,5,34,4,9,10,35,4,31,87,
  105,70,105,32,110,97,109,101,32,40,
  83,83,73,68,41,58,0,129,0,28,
  17,35,4,9,25,35,4,31,87,105,
  70,105,32,112,97,115,115,119,111,114,
  100,58,0,129,0,28,32,30,4,9,
  42,25,4,31,67,108,111,117,100,32,
  116,111,107,101,110,58,0,7,4,28,
  36,47,6,9,47,46,6,24,31,2,
  33,1,2,28,51,47,8,8,83,48,
  8,190,31,82,69,83,69,84,32,68,
  69,86,73,67,69,0 };

struct {

    // input variables
  char ssid[21];  // string UTF8 end zero  
  char pass[21];  // string UTF8 end zero  
  char token[33];  // string UTF8 end zero  
  uint8_t buttonReset; // =1 if button pressed, else =0 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY_WiFiSettings;

////////////////////////////////////////
// Main configuration
////////////////////////////////////////
uint8_t RemoteXY_CONF[] =
  { 255,4,0,2,0,245,0,10,8,2,
  130,1,3,22,36,19,2,30,35,23,
  8,130,1,3,42,36,18,2,58,35,
  25,8,129,0,6,30,18,6,4,75,
  0,6,8,0,66,1,44,3,36,47,
  41,6,10,77,5,26,129,0,82,45,
  9,6,53,79,6,4,17,48,37,0,
  129,0,82,24,9,6,53,42,8,4,
  17,53,48,37,0,129,0,81,3,15,
  6,52,7,11,4,17,49,48,48,37,
  0,7,54,4,30,32,10,4,70,32,
  11,5,26,5,129,0,4,23,31,6,
  4,64,24,5,16,65,117,116,111,32,
  108,101,118,101,108,58,0,2,0,4,
  10,31,10,4,12,31,10,2,26,31,
  31,77,65,78,85,65,76,0,65,85,
  84,79,0,129,0,3,3,34,6,4,
  5,29,5,16,80,117,109,112,32,109,
  111,100,101,0,129,0,4,43,21,6,
  4,32,18,5,16,77,97,110,117,97,
  108,0,2,0,4,49,31,10,4,39,
  31,10,2,26,31,31,79,78,0,79,
  70,70,0,65,4,48,52,9,9,16,
  87,9,9,129,0,60,54,18,6,28,
  89,26,6,11,80,117,109,112,32,111,
  110,0 };
  
struct {

    // input variables
  int16_t edit_level;  // 32767.. +32767 
  uint8_t switch_manual; // =1 if switch ON and =0 if OFF 
  uint8_t switch_pump; // =1 if switch ON and =0 if OFF 

    // output variables
  int8_t level; // =0..100 level position 
  uint8_t led_on_r; // =0..255 LED Red brightness 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;

#pragma pack(pop)



/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////


CRemoteXY *remotexy;
uint8_t wifiSettingMode = 0;

boolean readRemoteXYFromEEPROM () {
  if (EEPROM.read(check_EPPROM) == checkValueForEPPROM) {
    for (uint8_t i = 0; i<ssidLength; i++) RemoteXY_WiFiSettings.ssid[i] = EEPROM.read(ssid_EPPROM+i); 
    for (uint8_t i = 0; i<passLength; i++) RemoteXY_WiFiSettings.pass[i] = EEPROM.read(pass_EPPROM+i); 
    for (uint8_t i = 0; i<tokenLength; i++) RemoteXY_WiFiSettings.token[i] = EEPROM.read(token_EPPROM+i); 
    return true;
  }
  return false;  
}

void writeRemoteXYToEEPROM () {
  for (uint8_t i = 0; i<ssidLength; i++) EEPROM.write (ssid_EPPROM+i, RemoteXY_WiFiSettings.ssid[i]);  
  for (uint8_t i = 0; i<passLength; i++) EEPROM.write (pass_EPPROM+i, RemoteXY_WiFiSettings.pass[i]);
  for (uint8_t i = 0; i<tokenLength; i++) EEPROM.write (token_EPPROM+i, RemoteXY_WiFiSettings.token[i]);
  EEPROM.write (check_EPPROM, checkValueForEPPROM);    
  EEPROM.commit ();  
}

void setup() 
{
  EEPROM.begin(EPPROMLength);
  pinMode (PIN_BUTTON_WIFISETUP, INPUT);

  boolean EEPROMisValid = readRemoteXYFromEEPROM ();
  if (!digitalRead(PIN_BUTTON_WIFISETUP) || (!EEPROMisValid)) {
    // wifi setup mode
    wifiSettingMode = 1;
    remotexy = new CRemoteXY (RemoteXY_CONF_WiFiSettings, &RemoteXY_WiFiSettings, "", new CRemoteXYConnectionServer (new CRemoteXYComm_WiFiPoint (DEVICE_NAME, ""), REMOTEXY_SERVER_PORT)); 
  }
  else {
    remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, "", new CRemoteXYConnectionCloud (new CRemoteXYComm_WiFi (RemoteXY_WiFiSettings.ssid, RemoteXY_WiFiSettings.pass), REMOTEXY_CLOUD_SERVER, REMOTEXY_CLOUD_PORT, RemoteXY_WiFiSettings.token));      

    // TODO you setup code
    // example code
    RemoteXY.edit_level = 50; 
    // END example code
  } 
  
}

int prevSwitch_1 = 0;

void loop() 
{ 
  remotexy->handler ();
  
  if (wifiSettingMode) {

    if (RemoteXY_WiFiSettings.buttonReset) {
      writeRemoteXYToEEPROM ();  
      ESP.restart (); 
    }
  }  
  else myLoop();

}


float level = 50;
int pump = 0;
uint32_t prevMillis = 0;


void myLoop() {

  // TODO you loop code
  // example code
  uint32_t t = millis ();
  if (t>prevMillis) {
    float dt = float (t - prevMillis)/1000.0;
    prevMillis = t;

    //Serial.println (dt);
  
    if (RemoteXY.switch_manual == 1) {
      pump = RemoteXY.switch_pump;
    }
    else {
      if (pump==0) {
        if (level > RemoteXY.edit_level) pump = 1;
      }
      else {
         if (level < RemoteXY.edit_level-10) pump = 0;
      }
    }
  
    level = level + dt;
    if (pump != 0) level = level - dt*3;
    if (level<0) level = 0;
    if (level>100) level = 100;
  
    RemoteXY.level = level;
    RemoteXY.led_on_r = (pump==0)?0:255;
  }
  // END example code
}
