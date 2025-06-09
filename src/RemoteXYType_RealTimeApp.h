#ifndef RemoteXYType_RealTimeApp_h
#define RemoteXYType_RealTimeApp_h

#include "RemoteXYType.h"
#include "RemoteXYTime.h"

#pragma pack(push, 1)
struct RemoteXYType_RealTime_Buffer {
  int32_t days;
  int32_t millis;
};
#pragma pack(pop)

class CRemoteXYTypeInner_RealTimeApp : public CRemoteXYTypeInner {
  public:
  RemoteXYTimeStamp utcOffsetTime;    // utc offset
  RemoteXYTimeStamp appOffsetTime;    // phone offset
  
  public:
  uint8_t* init (uint8_t *conf) override  {    
    utcOffsetTime.setNull ();
    appOffsetTime.setNull ();
    return conf;
  };  
  

  
  public:
  uint8_t getDescriptorLength () override {
    return 0;
  }

  public:
  void getDescriptor (uint8_t * buf) override { 
    return;
  }  
  
  public:
  uint8_t handlePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) override {
    uint16_t i = 2;
    uint8_t * buf;
    while (i+9 <= package->length) {
      buf = &package->buffer[i+1];
      if (package->buffer[i] == 1) updateUtcTimeFromBuf (buf); 
      else if (package->buffer[i] == 2) updateAppTimeFromBuf (buf);
      i+=9; 
    }
    return 0; // will send empty package
  }
  
  protected:
  void setUtcTime (const RemoteXYTimeStamp &time) {
    if (utcOffsetTime.isNull ()) {  // set offsetTime
      utcOffsetTime = time;
      utcOffsetTime.sub (guiData->data->boardTime);
    }
    else {  // adjust board time, utcOffsetTime is constant   
      guiData->data->boardTime = time;
      guiData->data->boardTime.sub (utcOffsetTime);
    }    
  }  
  
  protected:
  void setAppTime (const RemoteXYTimeStamp &time) {
    appOffsetTime = time;
    appOffsetTime.sub (guiData->data->boardTime);
  }  
    
  
  protected:
  void updateUtcTimeFromBuf (uint8_t * buf) {
    RemoteXYType_RealTime_Buffer * rtbuf = (RemoteXYType_RealTime_Buffer*)buf;
    RemoteXYTimeStamp time (rtbuf->days, rtbuf->millis);
    setUtcTime (time);    
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("UTC time updated: "));
    RemoteXYDebugLog.writeAdd(time.getDays());
    RemoteXYDebugLog.writeAdd(F(" days "));
    RemoteXYDebugLog.writeAdd(time.getMillisSinceStartOfDay());
    RemoteXYDebugLog.writeAdd(F(" millis"));
#endif 
  }  
  
  protected:
  void updateAppTimeFromBuf (uint8_t * buf) {
    RemoteXYType_RealTime_Buffer * rtbuf = (RemoteXYType_RealTime_Buffer*)buf;
    RemoteXYTimeStamp time (rtbuf->days, rtbuf->millis);
    setAppTime (time);    
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("App time updated: "));
    RemoteXYDebugLog.writeAdd(time.getDays());
    RemoteXYDebugLog.writeAdd(F(" days "));
    RemoteXYDebugLog.writeAdd(time.getMillisSinceStartOfDay());
    RemoteXYDebugLog.writeAdd(F(" millis"));
#endif 
  }  
  
  
};


#define RemoteXYType_RealTimeApp_inner ((CRemoteXYTypeInner_RealTimeApp*)inner)
#pragma pack(push, 1) 
class RemoteXYType_RealTimeApp : public CRemoteXYType {
  
  public:
  RemoteXYType_RealTimeApp () {
    inner = new CRemoteXYTypeInner_RealTimeApp ();
  }  
    
  public:
  RemoteXYTimeStamp getUtcTimeStamp () {
    RemoteXYTimeStamp time (RemoteXYType_RealTimeApp_inner->utcOffsetTime);
    if (!time.isNull ()) {
      time.add (RemoteXYType_RealTimeApp_inner->guiData->data->boardTime); 
    }
    return time;
  }
  
  public:
  RemoteXYTimeStamp getAppTimeStamp () {
    RemoteXYTimeStamp time (RemoteXYType_RealTimeApp_inner->appOffsetTime);
    if (!time.isNull ()) {
      time.add (RemoteXYType_RealTimeApp_inner->guiData->data->boardTime); 
    }
    return time;
  }
  
  public:
  RemoteXYTimeStamp getTimeStamp (int16_t timeZone) {
    RemoteXYTimeStamp time = getUtcTimeStamp ();
    if (!time.isNull ()) {
      time.applyTimeZone (timeZone); 
    }
    return time;
  }
      
  public:
  RemoteXYTime getUtcTime () {  
    return RemoteXYTime (getUtcTimeStamp ());  
  }  
  
  public:
  RemoteXYTime getAppTime () {  
    return RemoteXYTime (getAppTimeStamp ());  
  }  
  
  public:
  RemoteXYTime getTime (int16_t timeZone) {  
    return RemoteXYTime (getTimeStamp (timeZone));  
  }   
  
};
#pragma pack(pop)
              


#endif // RemoteXYType_RealTimeApp_h