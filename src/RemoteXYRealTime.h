#ifndef RemoteXYRealTime_h
#define RemoteXYRealTime_h


#include "RemoteXYWire.h"
#include "RemoteXYTimeStamp.h"

class CRemoteXYRealTime {
  protected:
  RemoteXYTimeStamp boardTime;   
  uint32_t handlerMillis;
  
  public:
  CRemoteXYRealTime () {
    handlerMillis = millis ();
    boardTime.set (0, handlerMillis);  
  }
  
  RemoteXYTimeStamp getBoardTime () {
    return boardTime;    
  }
  
  virtual void handler () {
    uint32_t t = millis ();
    boardTime.add (t - handlerMillis);
    handlerMillis = t;     
  };  
  
  virtual void receivePackage (CRemoteXYPackage * package) {}
};


class CRemoteXYRealTimeApp : public CRemoteXYRealTime {
  public:
  RemoteXYTimeStamp offsetTime; 
  
  public: 
  CRemoteXYRealTimeApp () : CRemoteXYRealTime () {
    offsetTime.setNull ();
  }
 
  RemoteXYTimeStamp getTime () {
    RemoteXYTimeStamp time (offsetTime);
    if (!time.isNull ()) {
      time.add (boardTime); 
    }
    return time;
  }

  void receivePackage (CRemoteXYPackage * package) override {
    if (package->length==8) {
      updateFromBuf (package->buffer);
    }      
  }

  public:
  void setRealTime (const RemoteXYTimeStamp &time) {
    RemoteXYTimeStamp offset = time - boardTime;
    if (offsetTime.isNull ()) {
      offsetTime = offset;
    }
    else {  // adjust board time
      boardTime += offset - offsetTime; 
      offsetTime = time - boardTime;
    }
  }

  public:
  void updateFromBuf (uint8_t * buf) {
    uint32_t days, millis;
    rxy_bufCopy ((uint8_t*)&days, buf, 4);
    rxy_bufCopy ((uint8_t*)&millis, buf+4, 4);
    RemoteXYTimeStamp time (days, millis);
    setRealTime (time);    
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("RealTime updated: ");
    RemoteXYDebugLog.writeAdd(time.getDays());
    RemoteXYDebugLog.writeAdd(" days ");
    RemoteXYDebugLog.writeAdd(time.getMillis());
    RemoteXYDebugLog.writeAdd(" millis");
#endif     
     
  }
};
            
            
            
                     

#endif //RemoteXYRealTime_h


