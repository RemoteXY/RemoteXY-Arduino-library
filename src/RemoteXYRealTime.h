#ifndef RemoteXYRealTime_h
#define RemoteXYRealTime_h


#include "RemoteXYWire.h"

class CRemoteXYRealTime {
  public:
  virtual void handler () {};
  virtual void receivePackage (CRemoteXYPackage * package) {}
};

class CRemoteXYRealTimeApp : public CRemoteXYRealTime {
  public:
  RemoteXYTime shiftTime;  
  RemoteXYTime realTime; // real GMT time when the controller was started
  uint32_t handlerMillis;
  
  public: 
  CRemoteXYRealTimeApp () {
    handlerMillis = ::millis ();
    shiftTime.set (handlerMillis);
    realTime.setNull ();
  }

  // timeZone - minutes 
  RemoteXYTime getTime () {
    RemoteXYTime time = realTime;
    if (!time.isNull ()) {
      time.add (shiftTime); 
    }
    return time;
  }
  
  void handler () override {   
    uint32_t t = ::millis ();
    shiftTime.add (t - handlerMillis);
    handlerMillis = t;   
  }

  void receivePackage (CRemoteXYPackage * package) override {
    if (package->length==8) {
      updateFromBuf (package->buffer);
    }      
  }

  public:
  void updateFromBuf (uint8_t * buf) {
    uint32_t days, millis;
    rxy_bufCopy ((uint8_t*)&days, buf, 4);
    rxy_bufCopy ((uint8_t*)&millis, buf+4, 4);
    realTime.set(days, millis);
    shiftTime.setNull();
    handlerMillis = ::millis ();     
  }
};
                     

#endif //RemoteXYRealTime_h


