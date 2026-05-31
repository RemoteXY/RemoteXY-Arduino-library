/*
   RemoteXY Example: WiFi Client Configuration using Access Point Mode (ESP32 & ESP8266) 
   
   To connect, use the RemoteXY mobile 
   application at http://remotexy.com/en/download/               
    
   Copyright (c) 2014-2025 Evgenii Shemanuev
   Licensed under the MIT License. See LICENSE file in the project root for 
   full license information.     
*/


// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

#include <EEPROM.h>
#if defined (ESP8266)
  #include <ESP8266WiFi.h>
#elif defined (ESP32)
  #include <WiFi.h>  
#endif
#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_WIFIPOINT_NAME "Device Setup"
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com"
#define REMOTEXY_CLOUD_PORT 6376
#define REMOTEXY_CLOUD_TOKEN "xxxxxxxxxxx" // replace with your token

#define PIN_BUTTON_WIFISETUP 3   // pin of button for wifi WiFiSetup, press this button when devise reset

// RemoteXY GUI configuration  
#pragma pack(push, 1)  

// MAIN GUI
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =   // 102 bytes V21 
  { 254,1,0,3,0,0,0,1,0,0,1,0,88,0,21,0,0,0,0,31,
  1,106,200,1,1,6,0,70,16,101,24,24,16,26,37,0,3,15,59,75,
  28,131,2,26,70,41,101,24,24,16,26,37,0,70,66,101,24,24,16,26,
  37,0,129,23,12,57,12,64,24,84,101,115,116,32,109,111,100,101,0,129,
  24,48,57,7,64,24,83,97,118,101,100,32,116,111,32,69,69,80,82,79,
  77,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t selectorSwitch; // from 0 to 2, saved to EEPROM

    // output variables
  uint8_t led_01; // from 0 to 1
  uint8_t led_02; // from 0 to 1
  uint8_t led_03; // from 0 to 1

} RemoteXY;  

// WiFiSetup GUI
uint8_t const PROGMEM RemoteXY_CONF_WiFiSetup[] =   // 148 bytes V21 
  { 254,67,0,0,0,0,0,2,0,0,33,0,33,33,0,131,0,21,0,0,
  0,87,105,45,70,105,32,83,101,116,117,112,0,31,1,106,200,1,1,6,
  0,7,6,46,95,10,4,0,31,28,33,129,6,39,36,6,64,25,87,105,
  70,105,32,110,101,116,119,111,114,107,58,0,129,6,61,28,6,64,25,80,
  97,115,115,119,111,114,100,58,0,7,6,67,95,10,4,0,31,28,33,129,
  10,15,84,7,64,24,82,101,109,111,116,101,32,68,101,118,105,99,101,32,
  87,105,45,70,105,32,83,101,116,117,112,0,1,17,172,72,16,5,133,31,
  67,111,110,110,101,99,116,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  char wifiSsid[33]; // string UTF8 end zero, saved to EEPROM
  char wifiPass[33]; // string UTF8 end zero, saved to EEPROM
  uint8_t wifiConnectButton; // =1 if button pressed, else =0, from 0 to 1

} RemoteXY_WiFiSetup;   

#pragma pack(pop)
// END RemoteXY GUI configuration  


uint8_t wiFiSetupMode = 0;

void setup() 
{
  RemoteXYGui *wifiSetupGui = RemoteXYEngine.addGui (RemoteXY_CONF_WiFiSetup, &RemoteXY_WiFiSetup);
  RemoteXYGui *mainGui = RemoteXYEngine.addGui (RemoteXY_CONF_PROGMEM, &RemoteXY); 
  
  // init EEPROM
  EEPROM.begin(RemoteXYEngine.getEepromSize()); 
  RemoteXYEngine.initEeprom ();


  if (!digitalRead(PIN_BUTTON_WIFISETUP) || (RemoteXY_WiFiSetup.wifiSsid[0] != 0)) {
    // WiFiSetup button not pressed, go to main mode
    RemoteXYNet * net =  new CRemoteXYNet_WiFi (
      RemoteXY_WiFiSetup.wifiSsid,    // WIFI_SSID
      RemoteXY_WiFiSetup.wifiPass     // WIFI_PASSWORD
    );  
    mainGui->addConnectionCloud (net,       
      REMOTEXY_CLOUD_SERVER,   // CLOUD_SERVER
      REMOTEXY_CLOUD_PORT,     // CLOUD_PORT
      REMOTEXY_CLOUD_TOKEN     // CLOUD_TOKEN
    );
  }
  else {
    // go to WiFiSetup mode
    wiFiSetupMode = 1;
    RemoteXYNet * net = new CRemoteXYNet_WiFiPoint (
        REMOTEXY_WIFIPOINT_NAME,     // WIFI_SSID
        ""                           // NO PASSWORD
    );
    wifiSetupGui->addConnectionServer (net, 6377); // REMOTEXY_SERVER_PORT
  }
  
}

void loop() 
{ 
  RemoteXYEngine.handler (); 
  if (wiFiSetupMode == 1) {
    if (RemoteXY_WiFiSetup.wifiConnectButton !=0) {
      ESP.restart (); 
    }
    return;
  }


  // TODO you loop code
  // use the RemoteXY structure for data transfer
  // do not call delay(), use instead RemoteXYEngine.delay() 
  RemoteXY.led_01 = RemoteXY.selectorSwitch == 0 ? 1 : 0;
  RemoteXY.led_02 = RemoteXY.selectorSwitch == 1 ? 1 : 0;
  RemoteXY.led_03 = RemoteXY.selectorSwitch == 2 ? 1 : 0;
}

