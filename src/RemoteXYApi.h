#ifndef RemoteXYApi_h
#define RemoteXYApi_h
                        
#include <inttypes.h> 
#include <stdlib.h>
#include <Arduino.h>
#include <Stream.h>

#include "RemoteXYDebugLog.h"
#include "RemoteXYApiData.h"
#include "RemoteXYConnection.h"
#include "RemoteXYConnectionStream.h"
#include "RemoteXYConnectionServer.h"
#include "RemoteXYConnectionCloud.h"

#include "RemoteXYComm.h"
#include "RemoteXYCloudServer.h"
#include "RemoteXYThread.h"
#include "RemoteXYWire.h"
#include "RemoteXYWireStream.h" 
#include "RemoteXYStream_Stream.h" 

#include "RemoteXYCloudServer.h"



class CRemoteXY {
  public:
  CRemoteXYData data;  


  public:
  CRemoteXY (const void * _conf, void * _var, const char * _accessPassword = NULL) {                     
    data.init (_conf, _var, _accessPassword);
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
    RemoteXYDebugLog.write("RemoteXY started");
#endif 
  }
   
  public:
  CRemoteXY (const void * _conf, void * _var, CRemoteXYConnectionComm * _conn, const char * _accessPassword = NULL) {                     
    data.init (_conf, _var, _accessPassword);
    addConnection (_conn);
  
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
    RemoteXYDebugLog.write("RemoteXY started");
#endif 
  }
  
  public:
  CRemoteXY (const void * _conf, void * _var, CRemoteXYStream * _comm, const char * _accessPassword = NULL) {                     
    data.init (_conf, _var, _accessPassword);
    addConnection (_comm);
  
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
    RemoteXYDebugLog.write("RemoteXY started");
#endif 
  }
  
  public:
  CRemoteXY (const void * _conf, void * _var, Stream * _stream, const char * _accessPassword = NULL) {                     
    data.init (_conf, _var, _accessPassword);
    addConnection (_stream);
  
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
    RemoteXYDebugLog.write("RemoteXY started");
#endif 
  }
  
  public:
  void setPassword (const char * _accessPassword) {
    data.setPassword (_accessPassword); 
  }
  
  
  public:  
  void addConnection (CRemoteXYConnectionComm * conn) {   
    CRemoteXYComm * p = data.comms;
    while (p) {
      if (p == conn->comm) break;
      p = p->next;
    }
    if (!p) {
      conn->comm->next = data.comms;
      data.comms = conn->comm;
    }
    conn->next = data.connections;
    data.connections = conn; 
    conn->init (&data);    
  } 
          
  public:  
  void addConnection (CRemoteXYStream * comm) {  
    CRemoteXYConnection * conn = new CRemoteXYConnectionStream (comm);
    conn->init (&data);           
  } 


  public:  
  void addConnection (Stream * stream) {  
    CRemoteXYStream_Stream * comm = new CRemoteXYStream_Stream (stream);
    CRemoteXYConnection * conn = new CRemoteXYConnectionStream (comm);
    conn->init (&data);           
  }

    
  public:
  void handler () {
    uint8_t connect_flag = 0;
    
    // threads handler

    CRemoteXYThread * pt = data.threads;
    while (pt) {   
      pt->handler ();     
      connect_flag += pt->connect_flag;
      pt = pt->next;
    }
    *data.connect_flag = connect_flag;

    // communications handler    
    
    CRemoteXYComm * comm = data.comms; 
    while (comm) {
      comm->handler (); 
      comm = comm->next;
    } 
    
    // connections handler    

    CRemoteXYConnectionComm * connection = data.connections; 
    while (connection) {
      connection->handler (); 
      connection = connection->next;
    }    
      

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
  uint8_t isConnected () {
    return *data.connect_flag;
  }
  
  
};




   
#endif //RemoteXYApi_h

