/*
   RemoteXY example: setup WiFi client settings using BLE mode for ESP32 
   
   To connect, use the RemoteXY mobile 
   application at http://remotexy.com/en/download/               
    
   Copyright (c) 2014-2025 Evgenii Shemanuev
   Licensed under the MIT License. See LICENSE file in the project root for 
   full license information.   
*/

//#define REMOTEXY__DEBUGLOG

#include <EEPROM.h>
#include <WiFi.h>  
#include "BLEDevice.h"

#include <RemoteXY.h>

#define PIN_BUTTON_WIFISETUP 3   // pin of button for wifi settings, press this button when devise reset
#define EPPROM_CHECK_VALUE 0x55


#pragma pack(push, 1)
////////////////////////////////////////
// WiFi settings configuration
////////////////////////////////////////

// structure for EEPROM 
struct {
  char ssid[21]; 
  char pass[21];  
} WiFiSettings;


uint8_t const PROGMEM RemoteXY_CONF_WiFiSettings [] =
  { 255,43,0,0,0,106,0,10,172,2,
  7,4,28,18,47,7,9,25,46,7,
  24,31,2,21,7,4,28,34,47,7,
  9,44,46,7,24,31,2,21,129,0,
  28,13,34,4,9,20,35,4,31,87,
  105,70,105,32,110,97,109,101,32,40,
  83,83,73,68,41,58,0,129,0,28,
  29,35,4,9,39,35,4,31,80,97,
  115,115,119,111,114,100,58,0,1,2,
  28,46,47,8,8,61,48,8,190,31,
  82,69,83,69,84,32,68,69,86,73,
  67,69,0 };

struct {

    // input variables
  char ssid[21];  
  char pass[21];  
  uint8_t buttonReset; 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 


} RemoteXY_WiFiSettings;

////////////////////////////////////////
// Main configuration
////////////////////////////////////////

uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =
  { 255,1,0,1,0,27,0,10,13,2,
  1,0,9,9,46,46,6,7,50,50,
  2,31,88,0,65,4,62,16,31,31,
  14,62,35,35 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t button_1; // =1 if button pressed, else =0 

    // output variables
  uint8_t led_1_r; // =0..255 LED Red brightness 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;

#pragma pack(pop)



/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

uint8_t ifSettingsMode = 0;


boolean readWiFiSettings () {
  if (EEPROM.read(sizeof(WiFiSettings)) == EPPROM_CHECK_VALUE) {
    for (uint8_t i = 0; i<sizeof(WiFiSettings); i++) {
      *(((uint8_t*)&WiFiSettings)+i) = EEPROM.read(i); 
    }
    return true;
  }
  return false;  
}

void writeWiFiSettings () {
  for (uint8_t i = 0; i<sizeof(WiFiSettings); i++) {
    EEPROM.write (i, *(((uint8_t*)&WiFiSettings)+i));
  }
  EEPROM.write (sizeof(WiFiSettings), EPPROM_CHECK_VALUE);    
  EEPROM.commit ();  
}

void setup() 
{
  EEPROM.begin(sizeof(WiFiSettings)+1);
  pinMode (PIN_BUTTON_WIFISETUP, INPUT);
  
  boolean EEPROMisValid = readWiFiSettings ();
  if (!digitalRead(PIN_BUTTON_WIFISETUP) || (!EEPROMisValid)) {
    // wifi setup mode using BLE mode
    ifSettingsMode = 1;
    RemoteXYGui * gui = RemoteXYEngine.addGui (RemoteXY_CONF_WiFiSettings, &RemoteXY_WiFiSettings);
    gui->addConnection (new CRemoteXYStream_BLEDevice (
      "Device Settings"       // BLUETOOTH_NAME 
    ));  
    if (EEPROMisValid) {
      strcpy (RemoteXY_WiFiSettings.ssid, WiFiSettings.ssid);
      strcpy (RemoteXY_WiFiSettings.pass, WiFiSettings.pass);      
    }     
  }
  else {
    CRemoteXYNet * net =  new CRemoteXYNet_WiFi (
      WiFiSettings.ssid,    // WIFI_SSID
      WiFiSettings.pass     // WIFI_PASSWORD
    );  
    RemoteXYGui * gui = RemoteXYEngine.addGui (RemoteXY_CONF_PROGMEM, &RemoteXY);
    gui->addConnectionCloud (net,       
      "cloud.remotexy.com",   // CLOUD_SERVER
      6376,                   // CLOUD_PORT
      "xxxxxxxxxxxxxxxxxxxx"  // CLOUD_TOKEN
    );
  
    // TODO you setup code

  } 
  
}


void loop() 
{ 
  RemoteXYEngine.handler ();
  
  if (ifSettingsMode) {

    if (RemoteXY_WiFiSettings.buttonReset) {
      strcpy (WiFiSettings.ssid, RemoteXY_WiFiSettings.ssid);
      strcpy (WiFiSettings.pass, RemoteXY_WiFiSettings.pass);         
      writeWiFiSettings ();  
      ESP.restart (); 
    }
  }  
  else {
  
    // TODO you loop code
  
  
  }

}

