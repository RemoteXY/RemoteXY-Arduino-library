#ifndef RemoteXYConnectionServer_h
#define RemoteXYConnectionServer_h

#include "RemoteXYDebugLog.h"
#include "RemoteXYConnection.h"
#include "RemoteXYWire.h"
#include "RemoteXYThread.h"



class CRemoteXYConnectionServer: public CRemoteXYConnectionNet, public CRemoteXYClientAvailableListener { 
  public:
  uint16_t port;
  CRemoteXYGuiData * data;
  CRemoteXYServer * server;
  CRemoteXYWire * wires;
  uint8_t serverRunning;
  
  
  CRemoteXYConnectionServer (CRemoteXYNet * _net, uint16_t _port = 0) : CRemoteXYConnectionNet (_net) {
    port = _port;
    serverRunning = 0;
    wires = NULL;     
    server = net->createServer (port); 
    if (!server) {
#if defined(REMOTEXY__DEBUGLOG) 
      RemoteXYDebugLog.write (F("Server was not created"));
#endif    
      return;
    }                      
    server->setClientAvailableListener (this); 
  }
  
  void init (CRemoteXYGuiData * _data) {
    data = _data;
  }
  
  public:  
  void handler () override {
    if (server == NULL) return;
    if (net->configured ()) {
      if (serverRunning) {
        server->handler (); 
      }
      else {  
        if (server->begin ()) {
          serverRunning=1;          
#if defined(REMOTEXY__DEBUGLOG)
          RemoteXYDebugLog.write (F("Server opened on port "));
          RemoteXYDebugLog.writeAdd (port);
#endif                
        }
#if defined(REMOTEXY__DEBUGLOG)
        else {
          RemoteXYDebugLog.write (F("Server was not started"));
        }
#endif                
      }
    }
    else {
      if (serverRunning) {      
        server->stop ();
        serverRunning =0;
      }
    }                 
  }
  
  void clientAvailable (CRemoteXYClient * client) override {  

    CRemoteXYThread * thread = CRemoteXYThread::getUnusedThread (data);
    if (thread) {
      CRemoteXYWire * wire = wires;
      while (wire) {         
        if (!wire->running()) break;  
        wire = wire->next;
      }
      if (!wire) {
        wire = new CRemoteXYWire (data);
        wire->next = wires;
        wires = wire;        
      }
      wire->begin (client);
      thread->begin (this, wire, 1);
      wire->setReceivePackageListener (thread);  
    }
    else {
      client->stop ();  
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write (F("Client reject"));
#endif  
    }
      
  }
  
  void handleWire (CRemoteXYWire * wire) override {    
    if (wire->running() && serverRunning && net->configured ()) wire->handler (); 
    else stopThreadListener (wire);
  }

  void stopThreadListener (CRemoteXYWire * wire) override {
    CRemoteXYClient * client = (CRemoteXYClient*)wire->stream;
    if (client) {
      client->stop ();  
      wire->stop ();
    }
  }
  

  
};


#endif // RemoteXYConnectionServer_h