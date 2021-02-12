#ifndef RemoteXYWireCloud_h
#define RemoteXYWireCloud_h

#include "RemoteXYWire.h"


#define REMOTEXYWIRECLOUD_FREE_ID 0xff

class CRemoteXYCloudServer_Proto  {
  public:
  CRemoteXYWireStream * wire;
};

class CRemoteXYWireCloud: public CRemoteXYWire {

  public:
  CRemoteXYWireCloud * next;
  uint8_t id;
  uint8_t newConnection;

  private:
  CRemoteXYCloudServer_Proto * cloudServer;
  
  public:
  CRemoteXYWireCloud (CRemoteXYCloudServer_Proto * _cloudServer) : CRemoteXYWire () {
    cloudServer = _cloudServer;
    id = REMOTEXYWIRECLOUD_FREE_ID;
    newConnection = 0; 
  }

  public:
  uint8_t init (uint8_t _id) {
    id = _id;
    newConnection = 1;
  }

  public:
  uint8_t begin () {
    newConnection = 0; 
  }
  
  public:    
  void stop () override {
    setReceivePackageListener (NULL);
    id = REMOTEXYWIRECLOUD_FREE_ID;
    newConnection = 0;
  }
  
  public:
  uint8_t running () override {
    if (id == REMOTEXYWIRECLOUD_FREE_ID) return 0;
    return 1;
  }

  public:
  uint8_t isNewConnection () {
    return newConnection;
  }
  
        
  public:
  void sendPackage (uint8_t command, uint8_t *buf, uint16_t length, uint8_t fromPgm) override {
    cloudServer->wire->sendPackage (command, buf, length, fromPgm);
  }
  

  public:
  void receivePackage (CRemoteXYPackage * package) {
    notifyReceivePackageListener (package);
  }

};

#endif //RemoteXYWireCloud_h