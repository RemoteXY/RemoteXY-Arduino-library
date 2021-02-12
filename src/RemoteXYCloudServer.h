#ifndef RemoteXYCloudServer_h
#define RemoteXYCloudServer_h  
 

#include "RemoteXYApiData.h"
#include "RemoteXYComm.h"
#include "RemoteXYWire.h"
#include "RemoteXYWireStream.h"
#include "RemoteXYWireCloud.h"

#define REMOTEXY_CLOUD_CONNECT_TIMEOUT 10000
#define REMOTEXY_CLOUD_ECHO_TIMEOUT 30000


class CRemoteXYCloudServer : public CRemoteXYCloudServer_Proto, public CRemoteXYReceivePackageListener  {
  public:
  CRemoteXYWireCloud * cloudWires;
  
  uint8_t cloudRegistPackage[38];
  enum { Registring, Working, Stopped } state;
  uint32_t timeOut;

  
  public:
  CRemoteXYCloudServer (CRemoteXYData * data, const char * _cloudToken) {
    
    
    uint8_t i;
    uint8_t *p = cloudRegistPackage;
    *p++ = data->getConfByte(data->conf+0);
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
    *len = data->receiveBufferSize;

    wire = new CRemoteXYWireStream (data);
    cloudWires = NULL;
    state = Stopped;        
    
  }
  
  public:
  void begin (CRemoteXYClient * client) {
    wire->begin(client);
    wire->setReceivePackageListener (this);
    state = Registring;        
    wire->sendPackage (0x11, cloudRegistPackage, 38, 0);
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
      CRemoteXYClient * client = wire->getClient ();
      wire->handler ();
      if (!client->connected ()) stop ();
      if (wire->running ()) {
        
        if (state == Registring) {
          if (millis() - timeOut > REMOTEXY_CLOUD_CONNECT_TIMEOUT) stop ();
        }
        else if (state == Working) {
          if (millis() - timeOut > REMOTEXY_CLOUD_ECHO_TIMEOUT) {
            stop ();
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.write("Cloud server timed out");
#endif
           }
        }
      }
      else stop ();
    }
  }  

  public:
  CRemoteXYWire * availableWire () {

    CRemoteXYWireCloud * wp = cloudWires;
    while (wp) {
      if (wp->isNewConnection ()) {
        wp->begin ();
        return wp;
      }
      wp=wp->next;
    }

    return NULL;
    
  }
  
  
  public:
  void sendPackage (uint8_t command, uint8_t *buf, uint16_t length, uint8_t fromPgm) {
    wire->sendPackage (command, buf, length, fromPgm);
  }
  
  public:
  void receivePackage (CRemoteXYPackage * package) override {
    timeOut = millis ();
    if (package->command == 0x10) {
      wire->sendPackage (0x10, 0, 0, 0);
    }
    else if (package->command == 0x11) {
      CRemoteXYWireCloud * pw = getFreeWireCloud ();
      pw->init (0);  
      
      state = Working;
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write ("Cloud server registration successfully");
#endif   
    }
    else if (package->command == 0x12) {
      if (package->length == 1) {
        uint8_t id = (*package->buffer) & 0x07;
        CRemoteXYWireCloud * pw = getFreeWireCloud ();
        pw->init (id);          
      }
    }
    else {
      uint8_t id = package->command & 0x07;
      package->command &= 0xf8;
      CRemoteXYWireCloud * pw = cloudWires;   
      while (pw) {
        if ((pw->running ()) && (pw->id == id)) {    
          pw->receivePackage (package);
          break;
        }
        pw = pw->next;
      }
    }
  }  
  
  private:
  CRemoteXYWireCloud * getFreeWireCloud () {
    CRemoteXYWireCloud * pw = cloudWires;
    while (pw) {
      if (!pw->running ()) return pw;
      pw = pw->next;
    }
    pw = new CRemoteXYWireCloud (this);
    pw->next = cloudWires;
    cloudWires = pw;
    return pw;
  }
};



#endif //RemoteXYCloudServer_h