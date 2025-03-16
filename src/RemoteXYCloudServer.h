#ifndef RemoteXYCloudServer_h
#define RemoteXYCloudServer_h  
 

#include "RemoteXYGuiData.h"
//#include "RemoteXYNet.h"
#include "RemoteXYConnection.h"
#include "RemoteXYWire.h"
#include "RemoteXYThread.h"

#define REMOTEXY_CLOUD_CONNECT_TIMEOUT 10000
#define REMOTEXY_CLOUD_ECHO_TIMEOUT 30000



class CRemoteXYCloudServer : public CRemoteXYReceivePackageListener  {
  public:
  CRemoteXYGuiData * data;
  CRemoteXYConnection * conn;
  CRemoteXYWire * wire;
  
  
  uint8_t cloudRegistPackage[38];
  enum { Registring, Working, Stopped } state;
  uint32_t timeOut;

  
  public:
  CRemoteXYCloudServer (CRemoteXYGuiData * _data, CRemoteXYConnection * _conn, const char * _cloudToken) {
    
    data = _data;
    conn = _conn;
    uint8_t i;
    uint8_t *p = cloudRegistPackage;
    *p++ = rxy_readConfByte(data->conf+0);
    *p++ = 0;    
    for (i=0; i<32; i++) {
      if (*_cloudToken==0) *(p++)=0;
      else *(p++)=*(_cloudToken++);
    }
    uint16_t *len = (uint16_t*)p;
    *len = data->outputLength + data->inputLength;
    if (data->confLength>*len) *len = data->confLength;   
    *len += 6+1;    
    len = (uint16_t*)(p+2);     
    *len = data->getReceiveBufferSize ();
#if REMOTEXY_MAX_CLIENTS == 1
    wire = new CRemoteXYWire (data);
#else
    wire = new CRemoteXYWire (data, 2);
#endif    
    state = Stopped;           
  }
  
  
  public:
  void begin (CRemoteXYClient * client) {
    wire->begin(client);
    wire->setReceivePackageListener (this);
    state = Registring;        
    wire->sendPackage (0x11, 0, cloudRegistPackage, 38);
    timeOut = millis ();
  }
  
  public:
  void stop () { 
    if (state != Stopped) {
      wire->stop ();
      state = Stopped;      
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write ("Cloud server stoped");
#endif  
    }     
  }
  
  public:
  uint8_t running () {
    if (state != Stopped) return 1;
    return 0;
  }
  
  
  public:
  void handler () {
    if (state != Stopped)  {
      CRemoteXYClient * client = (CRemoteXYClient*)wire->stream;
      wire->handler ();
      if (!client->connected ()) stop ();
      if (wire->running ()) {
        
        if (state == Registring) {
          if (millis() - timeOut > REMOTEXY_CLOUD_CONNECT_TIMEOUT) stop ();
        }
        else if (state == Working) {
          if (millis() - timeOut > REMOTEXY_CLOUD_ECHO_TIMEOUT) {
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.write("Cloud server timed out");
#endif
            stop ();
           }
        }
      }
      else {
        stop ();
      }
    }
  }  
  
  

  
  
  public:
  void receivePackage (CRemoteXYPackage * package) override {
    timeOut = millis ();
    if (package->command == 0x10) {
      wire->sendEmptyPackage (0x10, 0);
    }
    else if (package->command == 0x11) {      
      state = Working;
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write ("Cloud server registration successfully");
#endif   
    }
    else {  

      CRemoteXYThread * thread = data->threads;            
      while (thread) {
        if (thread->running ()) {
          if ((thread->wire == wire) && (thread->clientId == package->clientId)) {    
            thread->receivePackage (package);
            return;
          }
        }
        thread = thread->next;
      }

      // new connect
      thread = CRemoteXYThread::getUnusedThread (data);
      if (thread) {
        thread->begin (conn, wire, 1);
        thread->setClientId (package->clientId);
        thread->receivePackage (package);     
      }
#if defined(REMOTEXY__DEBUGLOG)
      else {
        RemoteXYDebugLog.write ("Client reject");
      }
#endif 

    }
  }  
  

};



#endif //RemoteXYCloudServer_h