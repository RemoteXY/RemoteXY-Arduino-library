#ifndef RemoteXYRealTime_h
#define RemoteXYRealTime_h


#include "RemoteXYWire.h"
#include "RemoteXYTimeStamp.h"


class CRemoteXYRealTime {
  public:
  virtual void handler () = 0;
  virtual RemoteXYTimeStamp getBoardTime () {return RemoteXYTimeStamp ();}
  virtual RemoteXYTimeStamp getRealTime () {return RemoteXYTimeStamp ();}
  virtual void receivePackage (CRemoteXYPackage * package) {};
};


class CRemoteXYRealTimeBoard : public CRemoteXYRealTime {
  public:
  RemoteXYTimeStamp boardTime;   
  uint32_t handlerMillis;
  
  public:
  CRemoteXYRealTimeBoard () {
    handlerMillis = 0;
    boardTime.setNull ();  
  }
   
  void handler () override {
    uint32_t t = millis ();
    boardTime.add (t - handlerMillis);  
    handlerMillis = t;     
  };  
  
  RemoteXYTimeStamp getBoardTime () override {
    return boardTime;
  }
  
};


class CRemoteXYRealTimeApp : public CRemoteXYRealTimeBoard {
  public:
  RemoteXYTimeStamp offsetTime; 
  
  public: 
  CRemoteXYRealTimeApp () : CRemoteXYRealTimeBoard () {
    offsetTime.setNull ();
  }
 
  RemoteXYTimeStamp getRealTime () override {
    RemoteXYTimeStamp time (offsetTime);
    if (!time.isNull ()) {
      time.add (boardTime); 
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
    if (offsetTime.isNull ()) {  // set offsetTime
      offsetTime = time;
      offsetTime.sub (boardTime);
    }
    else {  // adjust board time, no offsetTime is constant
      boardTime = time;
      boardTime.sub (offsetTime);
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


