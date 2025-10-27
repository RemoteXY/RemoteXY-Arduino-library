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


class CRemoteXY: public CRemoteXYData {
  public:
  CRemoteXYGui * guis;
  

  private:
  void init () {
    nets = NULL;
    guis = NULL;   
    
    handlerMillis = 0;
    boardTime = 0;
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
    RemoteXYDebugLog.write(F("RemoteXY started"));
#endif 
  }

  public:
  CRemoteXY () {    
    init ();
  }        

  public:
  CRemoteXY (const void * _conf, void * _var, const char * _accessPassword = NULL) {                    
    init ();
    addGui (_conf, _var, _accessPassword);   
  }
   
  public:
  CRemoteXY (const void * _conf, void * _var, CRemoteXYConnectionNet * _conn, const char * _accessPassword = NULL) {                     
    init ();
    addGui (_conf, _var, _accessPassword);    
    addConnection (_conn);  
  }
  
  public:
  CRemoteXY (const void * _conf, void * _var, CRemoteXYStream * _stream, const char * _accessPassword = NULL) {                     
    init ();
    addGui (_conf, _var, _accessPassword);    
    addConnection (_stream);
  }
  
  public:
  CRemoteXY (const void * _conf, void * _var, Stream * _stream, const char * _accessPassword = NULL) {                     
    init ();
    addGui (_conf, _var, _accessPassword);    
    addConnection (_stream);
  
  }
    
  public: 
  CRemoteXYGui * addGui (const void * _conf, void * _var, const char * _accessPassword = NULL) {
    CRemoteXYGui * gui = new CRemoteXYGui (this, _conf, _var, _accessPassword); 
    if (gui == NULL) {
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.init ();
      RemoteXYDebugLog.write(F("Out of memory: "));
      RemoteXYDebugLog.writeAdd((uint16_t) sizeof (CRemoteXYGui));
      ::delay(1000);
#endif 
      return NULL;
    }  
    gui->next = guis;
    guis = gui;
    return gui;
  }
  


  public:  
  void addConnection (CRemoteXYStream * stream) {  
    if (guis) guis->addConnection (stream);     
  } 


  public:  
  void addConnection (Stream * stream) {  
    if (guis) guis->addConnection (stream);     
  }
   
  public:  
  void addConnection (CRemoteXYConnectionNet * conn) {   
    if (guis) guis->addConnection (conn);     
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




   
#endif //RemoteXYClass_h

