/* RemoteXY.h 
   A RemoteXY Library - Remote device control
   version 3.1.14   
   ===========================================================
   For use RemoteXY library visit website http://remotexy.com
   This website will help you use the library for configuring 
   a remote control from a smartphone or tablet.
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
        
   Supported modes: 
   All boards:
    #define REMOTEXY_MODE__HARDSERIAL                  - direct data transfer via HARDSERIAL
    #define REMOTEXY_MODE__SOFTSERIAL                  - direct data transfer via SOFTSERIAL
    #define REMOTEXY_MODE__ALTSOFTSERIAL               - direct data transfer via AltSoftSerial.h library
    #define REMOTEXY_MODE__USBSERIAL                   - direct data transfer via USB CDC for ATmega32u4, STM32
    #define REMOTEXY_MODE__ETHERNET                    - data transfer using <ethernet.h> library and open server
    #define REMOTEXY_MODE__ETHERNET_CLOUD              - data transfer using <ethernet.h> library and cloud connection
    #define REMOTEXY_MODE__HARDSERIAL_ESP8266          - data transfer via HARDSERIAL using AT commands of ESP8266 and open server
    #define REMOTEXY_MODE__HARDSERIAL_ESP8266_POINT    - data transfer via HARDSERIAL using AT commands of ESP8266 and open access point with a server
    #define REMOTEXY_MODE__HARDSERIAL_ESP8266_CLOUD    - data transfer via HARDSERIAL using AT commands of ESP8266 and cloud connection
    #define REMOTEXY_MODE__SOFTSERIAL_ESP8266          - data transfer via SOFTSERIAL using AT commands of ESP8266 and open server
    #define REMOTEXY_MODE__SOFTSERIAL_ESP8266_POINT    - data transfer via SOFTSERIAL using AT commands of ESP8266 and open access point with a server
    #define REMOTEXY_MODE__SOFTSERIAL_ESP8266_CLOUD    - data transfer via SOFTSERIAL using AT commands of ESP8266 and cloud connection
    #define REMOTEXY_MODE__ALTSOFTSERIAL_ESP8266       - data transfer via AltSoftSerial.h library using AT commands of ESP8266 and open server
    #define REMOTEXY_MODE__ALTSOFTSERIAL_ESP8266_POINT - data transfer via AltSoftSerial.h library using AT commands of ESP8266 and open access point with a server
    #define REMOTEXY_MODE__ALTSOFTSERIAL_ESP8266_CLOUD - data transfer via AltSoftSerial.h library using AT commands of ESP8266 and cloud connection
    #define REMOTEXY_MODE__WIFI                        - data transfer using generic wifi.h (esp8266wifi.h, WiFiS3.h) library and open server
    #define REMOTEXY_MODE__WIFI_POINT                  - data transfer using generic wifi.h (esp8266wifi.h, WiFiS3.h) library and open access point with a server
    #define REMOTEXY_MODE__WIFI_CLOUD                  - data transfer using generic wifi.h (esp8266wifi.h, WiFiS3.h) library and cloud connection
    
   Only ESP32 boards:
    #define REMOTEXY_MODE__ESP32CORE_BLE                     - data transfer using <BLEdevice.h> library
    #define REMOTEXY_MODE__ESP32CORE_BLUETOOTH               - data transfer using <BluetoothSerial.h> library

   Only NRF52xx based boards: 
    #define REMOTEXY_MODE__NRFCORE_BLEPERIPHERAL             - data transfer using <BLEPeripheral.h> library

   Boards supporting the ArduinoBLE.h library: 
    #define REMOTEXY_MODE__ARDUINOBLE                        - data transfer using <ArduinoBLE.h> library

   Parameters depending on the selected mode (for example):
    #define REMOTEXY_SERIAL Serial  // for Hardware Serial
    #define REMOTEXY_SERIAL_SPEED 115200 
    #define REMOTEXY_SERIAL_RX 2   // for Software Serial
    #define REMOTEXY_SERIAL_TX 3   // for Software Serial
    #define REMOTEXY_WIFI_SSID "RemoteXY"  
    #define REMOTEXY_WIFI_PASSWORD "1234567890" 
    #define REMOTEXY_ETHERNET_MAC "DE:AD:BE:EF:EF:ED"  // for Ethernet modules
    #define REMOTEXY_SERVER_PORT 6377 
    #define REMOTEXY_BLUETOOTH_NAME "remotexy"   // for ESP32
    #define REMOTEXY_CLOUD_TOKEN "xxxx" // for Cloud
    #define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com" // for Cloud
    #define REMOTEXY_CLOUD_PORT 6376  // for Cloud
    #define REMOTEXY_ACCESS_PASSWORD "1" 

   Debug log info on serial 115200 (define before include this library):
    #define REMOTEXY__DEBUGLOG
    #define REMOTEXY__DEBUGLOG_SERIAL Serial   
    #define REMOTEXY__DEBUGLOG_SPEED 115200

   = Version history ========================================

   version 2.2.5   
     - support MPIDE;
   version 2.3.1    
     - Support the device access password;
     - Support the cloud server as beta test;
     - Fixed a bug where the length of variables more than 255;
     - Fixed a bug where ESP module reboot and device did not see it;
     - Fixed a bug where the connection was filed and the device 
         did not see it and reconnection is impossible 
   version 2.3.4  
     - Fixed a bug where the length of all input variables more than 256;
     - Fixed a bug where millis() overflow in 50 days;
     - Fixed some bugs;
   version 2.3.5  
     - Fixed some bugs;
   version 2.4.1
     - support ESP32 WiFi and Bluetooth   
   version 2.4.2
     - Fixed some bugs;
   version 2.4.3
     - Fixed some bugs;
   version 2.4.4
     - Fixed ESP32 BLE bugs;
   version 3.1.1
     - Full update the library: multiple connections, set input variables from board and more;
   version 3.1.3
     - Multiple connection for cloud, fixed some bugs;
   version 3.1.4
     - fixed some bugs;
   version 3.1.5
     - add NRF5XXX and BLEPeripheral.h library as Beta, need define REMOTEXY_MODE__NRFCORE_BLEPERIPHERAL;
   version 3.1.6
     - fixed some bugs;
   version 3.1.7
     - add USB CDC Serial for Leonardo, Micro, ..;
   version 3.1.8
     - fixed some bugs;
   version 3.1.9
     - added function RemoteXY_delay (ms) as alternative delay (ms);
   version 3.1.10
     - added support for AltSoftSerial.h library;
     - support SoftwareSerial.h for ESP8266;
   version 3.1.11
     - added support USB CDC serial for STM32;
   version 3.1.12
     - fixed some bugs;
     - any Stream class can be used for communication: example/RemoteXY/Various/AnyStreamClass.pde
   version 3.1.13
     - fixed some bugs;
     - added support for the arduinoBLE.h library
     - added support for the WiFiS3.h library, which uses Arduino UNO R4 WiFi
   version 3.1.14
     - support ESP32 ver.3.0.x
               
*/

#ifndef _REMOTEXY_H_
#define _REMOTEXY_H_



#ifndef REMOTEXY_MAX_CLIENTS
#define REMOTEXY_MAX_CLIENTS 4
#endif


//#define REMOTEXY__DEBUGLOG
//#define REMOTEXY__DEBUGLOG_SERIAL Serial
//#define REMOTEXY__DEBUGLOG_SPEED 115200


#include <inttypes.h>
#include <Arduino.h>
#include "RemoteXYDebugLog.h"

#if defined(REMOTEXY_MODE__ESP32CORE_BLUETOOTH)
  #include "BluetoothSerial.h"
#elif defined(REMOTEXY_MODE__ESP32CORE_BLE)
  #include "BLEDevice.h"

#endif


#include "RemoteXYApi.h"

#include "RemoteXYStream_Stream.h"           // any stream
#include "RemoteXYStream_HardSerial.h"
#include "RemoteXYStream_SoftSerial.h"        // need SoftwareSerial.h or SoftSerial.h      
#include "RemoteXYStream_USBSerial.h"         
#include "RemoteXYStream_AltSoftSerial.h"     // need AltSoftSerial.h    
#include "RemoteXYStream_BluetoothSerial.h"   // need BluetoothSerial.h
#include "RemoteXYStream_BLEDevice.h"         // need BLEDevice.h
#include "RemoteXYStream_BLEPeripheral.h"     // need BLEPeripheral.h
#include "RemoteXYStream_ArduinoBLE.h"        // need ArduinoBLEl.h
#include "RemoteXYComm_WiFi.h"                // need ESP8266WiFi.h (ESP8266) or WiFi.h (ESP32) or WiFi.h (Arduino shield) 
#include "RemoteXYComm_Ethernet.h"            // need Ethernet.h 
#include "RemoteXYComm_ESP8266.h"


#ifndef REMOTEXY_ACCESS_PASSWORD 
#define REMOTEXY_ACCESS_PASSWORD ""
#endif 


#define RemoteXY_Handler() remotexy->handler ()
#define RemoteXY_CONF const PROGMEM RemoteXY_CONF_PROGMEM

// predefined configurations

#if defined(REMOTEXY_MODE__HARDSERIAL) || defined(REMOTEXY_MODE__SERIAL) || defined(REMOTEXY_MODE__HC05_HARDSERIAL)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYStream_HardSerial (&REMOTEXY_SERIAL, REMOTEXY_SERIAL_SPEED), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__SOFTSERIAL) || defined(REMOTEXY_MODE__SOFTWARESERIAL) || defined(REMOTEXY_MODE__HC05_SOFTSERIAL)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYStream_SoftSerial (REMOTEXY_SERIAL_RX, REMOTEXY_SERIAL_TX, REMOTEXY_SERIAL_SPEED), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__USBSERIAL) || defined(REMOTEXY_MODE__CDCSERIAL)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYStream_USBSerial (&REMOTEXY_SERIAL, REMOTEXY_SERIAL_SPEED), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__ALTSOFTSERIAL) 
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYStream_AltSoftSerial (REMOTEXY_SERIAL_SPEED), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__ESP32CORE_BLE)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYStream_BLEDevice (REMOTEXY_BLUETOOTH_NAME), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__WIFI) || defined(REMOTEXY_MODE__ESP32CORE_WIFI) || defined(REMOTEXY_MODE__ESP8266CORE_ESP8266WIFI) || defined(REMOTEXY_MODE__ESP8266WIFI_LIB) 
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_WiFi (REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__WIFI_POINT) || defined(REMOTEXY_MODE__ESP32CORE_WIFI_POINT) || defined(REMOTEXY_MODE__ESP8266CORE_ESP8266WIFI_POINT) || defined(REMOTEXY_MODE__ESP8266WIFI_LIB_POINT) || defined(REMOTEXY_MODE__ESP8266WIFIPOINT_LIB) 
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_WiFiPoint (REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__WIFI_CLOUD) || defined(REMOTEXY_MODE__ESP32CORE_WIFI_CLOUD) || defined(REMOTEXY_MODE__ESP8266CORE_ESP8266WIFI_CLOUD) || defined(REMOTEXY_MODE__ESP8266WIFI_LIB_CLOUD)               
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionCloud (new CRemoteXYComm_WiFi (REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_CLOUD_SERVER, REMOTEXY_CLOUD_PORT, REMOTEXY_CLOUD_TOKEN), REMOTEXY_ACCESS_PASSWORD)      

#elif defined(REMOTEXY_MODE__HARDSERIAL_ESP8266_POINT) || defined(REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT) || defined(REMOTEXY_MODE__ESP8266POINT_HARDSERIAL)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_ESP8266Point (new CRemoteXYStream_HardSerial (&REMOTEXY_SERIAL, REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 
 
#elif defined(REMOTEXY_MODE__SOFTSERIAL_ESP8266_POINT) || defined(REMOTEXY_MODE__ESP8266_SOFTSERIAL_POINT) || defined(REMOTEXY_MODE__ESP8266POINT_SOFTSERIAL)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_ESP8266Point (new CRemoteXYStream_SoftSerial (REMOTEXY_SERIAL_RX, REMOTEXY_SERIAL_TX, REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__ALTSOFTSERIAL_ESP8266_POINT) || defined(REMOTEXY_MODE__ESP8266_ALTSOFTSERIAL_POINT)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_ESP8266Point (new CRemoteXYStream_AltSoftSerial (REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__HARDSERIAL_ESP8266) || defined(REMOTEXY_MODE__ESP8266_HARDSERIAL)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_ESP8266 (new CRemoteXYStream_HardSerial (&REMOTEXY_SERIAL, REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 
  
#elif defined(REMOTEXY_MODE__SOFTSERIAL_ESP8266) || defined(REMOTEXY_MODE__ESP8266_SOFTSERIAL)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_ESP8266 (new CRemoteXYStream_SoftSerial (REMOTEXY_SERIAL_RX, REMOTEXY_SERIAL_TX, REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__ALTSOFTSERIAL_ESP8266) || defined(REMOTEXY_MODE__ESP8266_ALTSOFTSERIAL)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_ESP8266 (new CRemoteXYStream_AltSoftSerial (REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 
  
#elif defined(REMOTEXY_MODE__HARDSERIAL_ESP8266_CLOUD) || defined(REMOTEXY_MODE__ESP8266_HARDSERIAL_CLOUD)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionCloud (new CRemoteXYComm_ESP8266 (new CRemoteXYStream_HardSerial (&REMOTEXY_SERIAL, REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_CLOUD_SERVER, REMOTEXY_CLOUD_PORT, REMOTEXY_CLOUD_TOKEN), REMOTEXY_ACCESS_PASSWORD)      

#elif defined(REMOTEXY_MODE__SOFTSERIAL_ESP8266_CLOUD) || defined(REMOTEXY_MODE__ESP8266_SOFTSERIAL_CLOUD)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionCloud (new CRemoteXYComm_ESP8266 (new CRemoteXYStream_SoftSerial (REMOTEXY_SERIAL_RX, REMOTEXY_SERIAL_TX, REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_CLOUD_SERVER, REMOTEXY_CLOUD_PORT, REMOTEXY_CLOUD_TOKEN), REMOTEXY_ACCESS_PASSWORD)      

#elif defined(REMOTEXY_MODE__ALTSOFTSERIAL_ESP8266_CLOUD) || defined(REMOTEXY_MODE__ESP8266_ALTSOFTSERIAL_CLOUD)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionCloud (new CRemoteXYComm_ESP8266 (new CRemoteXYStream_AltSoftSerial (REMOTEXY_SERIAL_SPEED), REMOTEXY_WIFI_SSID, REMOTEXY_WIFI_PASSWORD), REMOTEXY_CLOUD_SERVER, REMOTEXY_CLOUD_PORT, REMOTEXY_CLOUD_TOKEN), REMOTEXY_ACCESS_PASSWORD)      

#elif defined(REMOTEXY_MODE__ETHERNET) || defined(REMOTEXY_MODE__ETHERNET_LIB) || defined(REMOTEXY_MODE__W5100_SPI)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionServer (new CRemoteXYComm_Ethernet (REMOTEXY_ETHERNET_MAC), REMOTEXY_SERVER_PORT), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__ETHERNET_CLOUD) || defined(REMOTEXY_MODE__ETHERNET_LIB_CLOUD)
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYConnectionCloud (new CRemoteXYComm_Ethernet (REMOTEXY_ETHERNET_MAC), REMOTEXY_CLOUD_SERVER, REMOTEXY_CLOUD_PORT, REMOTEXY_CLOUD_TOKEN), REMOTEXY_ACCESS_PASSWORD)      

#elif defined(REMOTEXY_MODE__ESP32CORE_BLUETOOTH)
  CRemoteXY *remotexy;      
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYStream_BluetoothSerial (REMOTEXY_BLUETOOTH_NAME), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__NRFCORE_BLEPERIPHERAL)
  CRemoteXY *remotexy;      
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYStream_BLEPeripheral (REMOTEXY_BLUETOOTH_NAME), REMOTEXY_ACCESS_PASSWORD) 

#elif defined(REMOTEXY_MODE__ARDUINOBLE) 
  CRemoteXY *remotexy;   
  #define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, new CRemoteXYStream_ArduinoBLE (REMOTEXY_BLUETOOTH_NAME), REMOTEXY_ACCESS_PASSWORD) 

#endif

//API
#define RemoteXY_delay(ms) remotexy->delay (ms)
#define RemoteXY_isConnected() remotexy->isConnected ()


#endif //_REMOTEXY_H_

