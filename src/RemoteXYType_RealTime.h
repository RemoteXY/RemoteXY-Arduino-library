#ifndef RemoteXYType_RealTime_h
#define RemoteXYType_RealTime_h

#include "RemoteXYType.h"
#include "RemoteXYTime.h"

#define UNUSED(x) (void)(x)

class CRemoteXYTypeInner_RealTime : public CRemoteXYTypeInner {
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
    UNUSED (wire);
  }

  
  public:
  uint8_t handlePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) override {
    uint8_t cm;   
    uint16_t cnt = package->length - 2;
    uint8_t * buf = package->buffer + 2;
    while (cnt >= 9) {
      cm = *buf++;      
      if (cm == 1) updateUtcTimeFromBuf (buf); 
      else if (cm == 2) updateAppTimeFromBuf (buf);
      cnt -= 9; 
      buf += 8;
    }
    UNUSED (wire);
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
    int64_t t = rxy_int64FromBuf (buf);
    setUtcTime (t);    
    
#if defined(REMOTEXY__DEBUGLOG)
    int32_t td = t / REMOTEXY_MILLIS_PER_DAY;
    int32_t tm = t % REMOTEXY_MILLIS_PER_DAY;
    RemoteXYDebugLog.write(F("UTC time updated: "));
    RemoteXYDebugLog.writeAdd(td);
    RemoteXYDebugLog.writeAdd(F(" days "));
    RemoteXYDebugLog.writeAdd(tm);
    RemoteXYDebugLog.writeAdd(F(" millis"));
#endif           
  }  
  
  protected:
  void updateAppTimeFromBuf (uint8_t * buf) {  
    int64_t t = rxy_int64FromBuf (buf);
    setAppTime (t);    
             
#if defined(REMOTEXY__DEBUGLOG)
    int32_t td = t / REMOTEXY_MILLIS_PER_DAY;
    int32_t tm = t % REMOTEXY_MILLIS_PER_DAY;
    RemoteXYDebugLog.write(F("App time updated: "));
    RemoteXYDebugLog.writeAdd(td);
    RemoteXYDebugLog.writeAdd(F(" days "));
    RemoteXYDebugLog.writeAdd(tm);
    RemoteXYDebugLog.writeAdd(F(" millis"));
#endif      
  }  
  
  
};


#define RemoteXYType_RealTime_inner ((CRemoteXYTypeInner_RealTime*)inner)
#pragma pack(push, 1) 
class RemoteXYType_RealTime : public CRemoteXYType {
  
  public:
  RemoteXYType_RealTime () {
    inner = new CRemoteXYTypeInner_RealTime ();
  }  
    
  public:
  int64_t getUtcTimeStamp () {
    int64_t time = RemoteXYType_RealTime_inner->utcOffsetTime;
    if (time != 0) {
      time += RemoteXYType_RealTime_inner->guiData->data->boardTime; 
    }
    return time;
  }
  
  public:
  int64_t getAppTimeStamp () {
    int64_t time = RemoteXYType_RealTime_inner->appOffsetTime;
    if (time != 0) {
      time += RemoteXYType_RealTime_inner->guiData->data->boardTime; 
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
              


#endif // RemoteXYType_RealTime_h