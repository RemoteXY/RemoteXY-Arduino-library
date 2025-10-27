#ifndef RemoteXYConnectionStream_h
#define RemoteXYConnectionStream_h

#include "RemoteXYDebugLog.h"
#include "RemoteXYStream.h"
#include "RemoteXYConnection.h"
#include "RemoteXYThread.h"

class CRemoteXYConnectionStream: public CRemoteXYConnection { 
  CRemoteXYStream * stream;
    
  public: 
  CRemoteXYConnectionStream (CRemoteXYStream * _stream) {
    stream = _stream;
  }    
  
  public:
  void init (CRemoteXYGuiData * _data) override {
    CRemoteXYWire * wire = new CRemoteXYWire (_data);
    wire->begin (stream); 
    CRemoteXYThread * thread = CRemoteXYThread::getUnusedThread (_data);
    if (thread) {
      thread->begin (this, wire, 0);
      wire->setReceivePackageListener (thread);
    }
  };
  
  public:
  void handleWire (CRemoteXYWire * wire) override {
    wire->handler ();
  }; 
  
  public:
  uint8_t configured () override {
    return stream->connected ();  
  } 
  
};

#endif // CRemoteXYConnectionStream