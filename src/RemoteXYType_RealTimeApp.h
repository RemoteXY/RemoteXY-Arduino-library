#ifndef RemoteXYType_RealTimeApp_h
#define RemoteXYType_RealTimeApp_h

#include "RemoteXYType.h"
#include "RemoteXYTime.h"


class CRemoteXYTypeInner_RealTimeApp : public CRemoteXYTypeInner {
  public:
  int64_t utcOffsetTime;    // utc offset
  int64_t appOffsetTime;    // phone offset
  
  public:
  uint8_t* init (uint8_t *conf) override  {    
    utcOffsetTime = 0;
    appOffsetTime = 0;
    return conf;
  };  
  

  
  public:
  uint8_t getDescriptorLength () override {
    return 0;
  }
  
  public:
  void sendDescriptorBytes (CRemoteXYWire * wire) override {
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
  void setUtcTime (const int64_t &time) {
    if (utcOffsetTime == 0) {  // set offsetTime
      utcOffsetTime = time;
      utcOffsetTime -= guiData->data->boardTime;
    }
    else {  // adjust board time, utcOffsetTime is constant   
      guiData->data->boardTime = time;
      guiData->data->boardTime -= utcOffsetTime;
    }    
  }  
  
  protected:
  void setAppTime (const int64_t &time) {
    appOffsetTime = time;
    appOffsetTime -= guiData->data->boardTime;
  }  
    
  
  protected:
  void updateUtcTimeFromBuf (uint8_t * buf) {
    int64_t * ptime = (int64_t*) buf;
    setUtcTime (*ptime);    
    
#if defined(REMOTEXY__DEBUGLOG)
    int32_t td = *ptime / REMOTEXY_MILLIS_PER_DAY;
    int32_t tm = *ptime % REMOTEXY_MILLIS_PER_DAY;
    RemoteXYDebugLog.write(F("UTC time updated: "));
    RemoteXYDebugLog.writeAdd(td);
    RemoteXYDebugLog.writeAdd(F(" days "));
    RemoteXYDebugLog.writeAdd(tm);
    RemoteXYDebugLog.writeAdd(F(" millis"));
#endif 
  }  
  
  protected:
  void updateAppTimeFromBuf (uint8_t * buf) {
    int64_t * ptime = (int64_t*) buf;
    setAppTime (*ptime);    
    
#if defined(REMOTEXY__DEBUGLOG)
    int32_t td = *ptime / REMOTEXY_MILLIS_PER_DAY;
    int32_t tm = *ptime % REMOTEXY_MILLIS_PER_DAY;
    RemoteXYDebugLog.write(F("App time updated: "));
    RemoteXYDebugLog.writeAdd(td);
    RemoteXYDebugLog.writeAdd(F(" days "));
    RemoteXYDebugLog.writeAdd(tm);
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
  int64_t getUtcTimeStamp () {
    int64_t time = RemoteXYType_RealTimeApp_inner->utcOffsetTime;
    if (time != 0) {
      time += RemoteXYType_RealTimeApp_inner->guiData->data->boardTime; 
    }
    return time;
  }
  
  public:
  int64_t getAppTimeStamp () {
    int64_t time = RemoteXYType_RealTimeApp_inner->appOffsetTime;
    if (time != 0) {
      time += RemoteXYType_RealTimeApp_inner->guiData->data->boardTime; 
    }
    return time;
  }
  
  public:
  int64_t getTimeStamp (int16_t timeZoneMinutes) {
    int64_t time = getUtcTimeStamp ();
    if (time != 0) {
      time += (uint32_t)timeZoneMinutes * (uint32_t)60000; 
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
  RemoteXYTime getTime (int16_t timeZoneMinutes) {  
    return RemoteXYTime (getTimeStamp (timeZoneMinutes));  
  }   
  
};
#pragma pack(pop)
              


#endif // RemoteXYType_RealTimeApp_h