#ifndef RemoteXYConnectionCloud_h
#define RemoteXYConnectionCloud_h

#include "RemoteXYDebugLog.h"
#include "RemoteXYConnection.h"
#include "RemoteXYThread.h"

#define REMOTEXY_CLOUDCLIENT_RETRY_TIMEOUT 20000

class CRemoteXYConnectionCloud: public CRemoteXYConnectionComm { 
  public:
  uint16_t port;
  CRemoteXYCloudServer * cloudServer;
  const char * cloudHost; 
  const char * cloudToken;
  CRemoteXYData * data;
  CRemoteXYClient * client;
  uint32_t timeOut;
    
  public:
  CRemoteXYConnectionCloud (CRemoteXYComm * _comm, const char * _cloudHost, uint16_t _port, const char * _cloudToken) : CRemoteXYConnectionComm (_comm) {
    port = _port;
    cloudHost = _cloudHost;
    cloudToken = _cloudToken;
  }
  
  public:
  void init (CRemoteXYData * _data) override {
    data = _data;
    cloudServer = new CRemoteXYCloudServer (data, cloudToken);
    client = comm->newClient ();
    timeOut = -REMOTEXY_CLOUDCLIENT_RETRY_TIMEOUT;
  }
  
  void handler () override {
    
    if (comm->configured ()) {
      if (cloudServer->running ()) {
        cloudServer->handler();     
        CRemoteXYWire * cloudWire = cloudServer->availableWire ();   
        if (cloudWire) {
          if (CRemoteXYThread::runningCount (data) < REMOTEXY_MAX_CLIENTS) { 
            CRemoteXYThread::startThread (data, this, cloudWire, 0);  
          }
          else {
            cloudWire->stop ();
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.write ("Client reject");
#endif           
          }
        }
        timeOut = millis();      
      }
      else { // not serverRunning
        if (millis() - timeOut > REMOTEXY_CLOUDCLIENT_RETRY_TIMEOUT) {
          timeOut = millis();
#if defined(REMOTEXY__DEBUGLOG)
          RemoteXYDebugLog.write ("Connecting to cloud: ");
          RemoteXYDebugLog.writeAdd (cloudHost);
          RemoteXYDebugLog.writeAdd (" ");
          RemoteXYDebugLog.writeAdd (port);
          RemoteXYDebugLog.writeAdd (" ..");
#endif 
          if (client->connect (cloudHost, port)) {
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.write ("Cloud server connected");
#endif 
            cloudServer->begin (client);            
          }        
#if defined(REMOTEXY__DEBUGLOG)
          else {
            RemoteXYDebugLog.write ("Cloud server not available");
          }
#endif        
        }
      }
    }
    else cloudServer->stop ();
  }  

  
  void handleWire (CRemoteXYWire * wire) override {  
    if (cloudServer->running () && comm->configured ()) wire->handler (); 
    else stopThreadListener (wire);
  }
  
  void stopThreadListener (CRemoteXYWire * wire) override {
    wire->stop ();
  }
  

  
};


#endif // RemoteXYConnectionCloud_h