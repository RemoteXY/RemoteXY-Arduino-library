#ifndef RemoteXYClass_h
#define RemoteXYClass_h
                        
#include <inttypes.h> 
//#include <stdlib.h>
//#include <Arduino.h>
#include <Stream.h>

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

#include "RemoteXYType.h"
#include "RemoteXYRealTime.h"
#include "RemoteXYRealTimeNet.h"

#include "RemoteXYType_Notification.h"


class CRemoteXY: public CRemoteXYData {
  public:
  CRemoteXYGui * guis;
  

  private:
  void init () {
    nets = NULL;
    guis = NULL;   
    realTime = NULL;   
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
    RemoteXYDebugLog.write("RemoteXY started");
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
      RemoteXYDebugLog.write("Out of memory: ");
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
  uint8_t initEeprom (uint8_t callBegin = 1) {
#if defined(REMOTEXY_HAS_EEPROM)
    if (eeprom.initialized == 0) { 
      if (eeprom.init (callBegin) == 0) {
        ::delay (1000);
        return 0;
      } 
    }
#endif    
    return 1; 
  }
  
  
  // HANDLER
    
    
    
  public:
  void handler () {     
#if defined(REMOTEXY_HAS_EEPROM)
    if (initEeprom ()) eeprom.handler ();
#endif  

    if (realTime == NULL) realTime = new CRemoteXYRealTime ();
    realTime->handler ();
    
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
  

    
 
  public:
  void initRealTime () {
    if (realTime == NULL) {  
      realTime = new CRemoteXYRealTimeApp (); 
    }
  }
  
  public:
  void initRealTimeNet () { 
    if (realTime == NULL) { 
      realTime = new CRemoteXYRealTimeNet (nets);
    }
  }  
  
  
  public:
  RemoteXYTimeStamp getRealTimeStamp (int16_t timeZone) {    
    if (realTime == NULL) return RemoteXYTimeStamp ();
    RemoteXYTimeStamp time = ((CRemoteXYRealTimeApp*)realTime)->getTime ();
    time.applyTimeZone (timeZone);
    return time;
  }    
  
  public:
  RemoteXYTime getRealTime (int16_t timeZone) {    
    if (realTime == NULL) return RemoteXYTimeStamp ();
    RemoteXYTimeStamp time = ((CRemoteXYRealTimeApp*)realTime)->getTime ();
    time.applyTimeZone (timeZone);
    return RemoteXYTime (time);
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
    uint8_t connect_flag = 0;
    CRemoteXYGui * pg = guis;
    while (pg) {
      if (pg->connect_flag) connect_flag = 1;
      pg = pg->next;
    }   
    return connect_flag;
  }
      

  
};




   
#endif //RemoteXYClass_h

