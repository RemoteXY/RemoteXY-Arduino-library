#ifndef RemoteXYRealTime_h
#define RemoteXYRealTime_h


#include "RemoteXYWire.h"
#include "RemoteXYTimeStamp.h"


class CRemoteXYRealTime {
  public:
  virtual void handler () = 0;
  virtual RemoteXYTimeStamp getBoardTime () = 0;
  virtual RemoteXYTimeStamp getRealTime () = 0;
  virtual void receivePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) = 0;
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
  
  RemoteXYTimeStamp getRealTime () override {
    return RemoteXYTimeStamp ();
  }
  
  void getBoardTimeBuf (uint8_t * buf) {
    uint32_t days = boardTime.getDays ();
    uint32_t millis = boardTime.getMillis ();
    rxy_bufCopy (buf, (uint8_t*)&days, 4);
    rxy_bufCopy (buf+4, (uint8_t*)&millis, 4);
  }
  
  void receivePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) override {
    uint8_t buf[8];
    getBoardTimeBuf (buf);
    wire->sendPackage (0x02, package->clientId, buf, 8);    
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
  
  void receivePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) override {
    if (package->length==8) {
      updateFromBuf (package->buffer);
    }  
    CRemoteXYRealTimeBoard::receivePackage (package, wire);    
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


