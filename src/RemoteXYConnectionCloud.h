#ifndef RemoteXYConnectionCloud_h
#define RemoteXYConnectionCloud_h

#include "RemoteXYDebugLog.h"
#include "RemoteXYConnection.h"
#include "RemoteXYCloudServer.h"
#include "RemoteXYThread.h"

#define REMOTEXY_CLOUDCLIENT_RETRY_TIMEOUT 20000

class CRemoteXYConnectionCloud: public CRemoteXYConnectionNet { 
  public:
  uint16_t port;
  CRemoteXYCloudServer * cloudServer;
  const char * cloudHost; 
  const char * cloudToken;
  CRemoteXYGuiData * data;
  CRemoteXYClient * client;
  uint32_t timeOut;
    
  public:
  CRemoteXYConnectionCloud (CRemoteXYNet * _net, const char * _cloudHost, uint16_t _port, const char * _cloudToken) : CRemoteXYConnectionNet (_net) {
    port = _port;
    cloudHost = _cloudHost;
    cloudToken = _cloudToken;
    client = _net->newClient ();
  }
  
  public:
  void init (CRemoteXYGuiData * _data) override {
    data = _data;
    cloudServer = new CRemoteXYCloudServer (data, this, cloudToken);
    timeOut = millis() - REMOTEXY_CLOUDCLIENT_RETRY_TIMEOUT;
  }
  
  void handler () override {
    
    if (net->configured ()) {
      if (cloudServer->running ()) {
        cloudServer->handler();     
        timeOut = millis();      
      }
      else { // not serverRunning
        if (millis() - timeOut > REMOTEXY_CLOUDCLIENT_RETRY_TIMEOUT) {
#if defined(REMOTEXY__DEBUGLOG)
          RemoteXYDebugLog.write (F("Connecting to cloud: "));
          RemoteXYDebugLog.writeAdd (cloudHost);
          RemoteXYDebugLog.writeAdd (F(" "));
          RemoteXYDebugLog.writeAdd (port);
          RemoteXYDebugLog.writeAdd (F(" .."));
#endif     
          if (client->connect (cloudHost, port)) {
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.write (F("Cloud server connected"));
#endif 
            cloudServer->begin (client);            
          }        
#if defined(REMOTEXY__DEBUGLOG)
          else {
            RemoteXYDebugLog.write (F("Cloud server not available"));
          }
#endif     
          timeOut = millis();   
        }
      }
    }
    else {
      cloudServer->stop ();
      timeOut = millis() - REMOTEXY_CLOUDCLIENT_RETRY_TIMEOUT;
    }
    
    if (!cloudServer->running ()) { 
      if (client) {  
        if (client->connected ()) {
          client->stop ();
        } 
      }
    }
  }  

  
};


#endif // RemoteXYConnectionCloud_h