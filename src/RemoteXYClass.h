#ifndef RemoteXYClass_h
#define RemoteXYClass_h
                        
#include <inttypes.h> 
//#include <stdlib.h>
//#include <Arduino.h>
//#include <Stream.h>

#include "RemoteXYDebugLog.h"
#include "RemoteXYGui.h"
#include "RemoteXYData.h" 

#include "RemoteXYConnection.h"
#include "RemoteXYConnectionStream.h"
#include "RemoteXYConnectionServer.h"
#include "RemoteXYConnectionCloud.h"

#include "RemoteXYNet.h"
#include "RemoteXYCloudServer.h"
#include "RemoteXYThread.h"
#include "RemoteXYWire.h"
#include "RemoteXYStream_Stream.h" 

#include "RemoteXYCloudServer.h"

// types
#include "RemoteXYType_RealTime.h"
#include "RemoteXYType_RealTimeNet.h"
#include "RemoteXYType_Terminal.h"
#include "RemoteXYType_Notification.h"
#include "RemoteXYType_NotificationNet.h"

#define UNUSED(x) (void)(x)

class CRemoteXY: public CRemoteXYData {
  
  private:
  void init () {
    nets = NULL;
    guis = NULL;   
    
    handlerMillis = 0;
    boardTime = 0;
    
  }

  public:
  CRemoteXY () {    
    init ();
  }        
     
  public:
  //deprecated
  CRemoteXY (const void * _conf, void * _var, const char * _accessPassword = NULL) {                    
    init ();
    addGui (_conf, _var, _accessPassword);   
  }
   
  public:
  //deprecated
  CRemoteXY (const void * _conf, void * _var, CRemoteXYConnectionNet * _conn, const char * _accessPassword = NULL) {                     
    init ();
    CRemoteXYGui * gui = addGui (_conf, _var, _accessPassword);    
    gui->addConnection (_conn);  
  }
  
  public:
  //deprecated
  CRemoteXY (const void * _conf, void * _var, CRemoteXYStream * _stream, const char * _accessPassword = NULL) {                     
    init ();
    CRemoteXYGui * gui = addGui (_conf, _var, _accessPassword);    
    gui->addConnection (_stream);
  }
  
  public:
  //deprecated
  CRemoteXY (const void * _conf, void * _var, Stream * _stream, const char * _accessPassword = NULL) {                     
    init ();
    CRemoteXYGui * gui = addGui (_conf, _var, _accessPassword);    
    gui->addConnection (_stream);  
  }
    
  public: 
  CRemoteXYGui * addGui (const void * _conf, void * _var, const char * _accessPassword = NULL) {
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
    RemoteXYDebugLog.write(F("Add GUI"));
#endif 
    CRemoteXYGui * gui = new CRemoteXYGui (this, _conf, _var, _accessPassword); 
    if (gui) {
      gui->next = guis;
      guis = gui;
    }
#if defined(REMOTEXY__DEBUGLOG)
    else {
      RemoteXYDebugLog.write(F("Out of memory for RemoteXYGui"));
    }
#endif 
    return gui;
  }
  
  
  // HANDLER
    
        
  public:
  void handler () {   
  
    uint32_t t = millis ();
    boardTime += (t - handlerMillis);  
    handlerMillis = t;     
  
#if defined(REMOTEXY_HAS_EEPROM)
    eeprom.init ();
    eeprom.handler ();
#endif  
    
    // nets handler    
    CRemoteXYNet * net = nets; 
    while (net) {
      net->handler (); 
      net = net->next;
    } 
  
    // guis handler 
    CRemoteXYGui * pg = guis;
    while (pg) {
      pg->handler();
      pg = pg->next;
    }  
    
      
  }  
  

  int64_t getBoardTime () {
    return boardTime;
  }    
 


///////////////////////////////////////////////////////////////////////////////
// API - PUBLIC DOCUMENTED FUNCTIONS
  
  public:
  void delay (uint32_t ms) {
    uint32_t t = millis ();   
    while (true) {
      handler ();
      ::delay (1); // if does not use the delay it does not work on ESP8266 
      if (millis () - t >= ms) break;
    }   
  }
  

  public:
  uint8_t appConnected () {
    CRemoteXYGui * pg = guis;
    while (pg) {
      if (pg->appConnected ()) return 1;
      pg = pg->next;
    }   
    return 0;
  }
      
  
  public:
  uint8_t netsConfigured () {
    CRemoteXYNet * net = nets; 
    while (net) {
      if (net->configured () == 0) return 0;
      net = net->next;
    } 
    return 1;
  }
  
  public:
  uint8_t connectionsConfigured () {
    CRemoteXYGui * pg = guis;
    while (pg) {
      if (pg->connectionsConfigured() == 0) return 0;
      pg = pg->next;
    }  
    return 1;
  }

  // EEPROM SUPPORT FUNCTIONS
  
  public:
  uint16_t getEepromSize () {
#if defined(REMOTEXY_HAS_EEPROM)
    return eeprom.getSize ();
#else
    return 0;
#endif    
  }
  
  public:
  void setEepromOffset (uint16_t offset) {
#if defined(REMOTEXY_HAS_EEPROM)
    eeprom.setOffset (offset);
#else
    UNUSED (offset);
#endif    
  }  

  public:
  int8_t initEeprom () {
#if defined(REMOTEXY_HAS_EEPROM)
    eeprom.init ();
    return eeprom.initialized;   
#else
    return -1;
#endif
  }
  
  
};


CRemoteXY RemoteXYEngine;

   
#endif //RemoteXYClass_h

