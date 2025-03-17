#ifndef RemoteXYRealTime_h
#define RemoteXYRealTime_h


#include "RemoteXYWire.h"
#include "RemoteXYTimeStamp.h"

class CRemoteXYBoardTime {
  public:
  RemoteXYTimeStamp time;   
  uint32_t handlerMillis;
  
  public:
  CRemoteXYBoardTime () {
    handlerMillis = 0;
    time.setNull ();  
  }
   
  void handler () {
    uint32_t t = millis ();
    time.add (t - handlerMillis);
    handlerMillis = t;     
  };  
};


class CRemoteXYRealTime {
  public:
  virtual void handler () = 0;
  virtual void receivePackage (CRemoteXYPackage * package) = 0;
};


class CRemoteXYRealTimeApp : public CRemoteXYRealTime {
  public:
  CRemoteXYBoardTime * boardTime;
  RemoteXYTimeStamp offsetTime; 
  
  public: 
  CRemoteXYRealTimeApp (CRemoteXYBoardTime * _boardTime) {
    boardTime = _boardTime;
    offsetTime.setNull ();
  }
 
  RemoteXYTimeStamp getTime () {
    RemoteXYTimeStamp time (offsetTime);
    if (!time.isNull ()) {
      time.add (boardTime->time); 
    }
    return time;
  }
  
  void handler () override {}

  void receivePackage (CRemoteXYPackage * package) override {
    if (package->length==8) {
      updateFromBuf (package->buffer);
    }      
  }

  public:
  void setRealTime (const RemoteXYTimeStamp &time) {
    RemoteXYTimeStamp offset = time - boardTime->time;
    if (offsetTime.isNull ()) {
      offsetTime = offset;
    }
    else {  // adjust board time
      boardTime->time += offset - offsetTime; 
      offsetTime = time - boardTime->time;
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


