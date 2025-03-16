#ifndef RemoteXYConnection_h
#define RemoteXYConnection_h

#include "RemoteXYGuiData.h"
#include "RemoteXYNet.h"
#include "RemoteXYWire.h"
//#include "RemoteXYCloudServer.h"

#define UNUSED(x) (void)(x)

class CRemoteXYConnection { 
  
  public:
  virtual void init (CRemoteXYGuiData * _data) {UNUSED (_data);};
  
  public:
  virtual void handler () {};
  virtual void handleWire (CRemoteXYWire * wire) {UNUSED (wire);};
  virtual void stopThreadListener (CRemoteXYWire * wire) {UNUSED (wire);};
  
};


class CRemoteXYConnectionNet : public CRemoteXYConnection { 
  public:   
  CRemoteXYConnectionNet * next;  
  CRemoteXYNet * net;
  
  public:
  CRemoteXYConnectionNet (CRemoteXYNet * _net) {
    net = _net;
  }  
  
};

#endif //RemoteXYConnection_h