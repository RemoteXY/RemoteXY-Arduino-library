#ifndef RemoteXYType_RealTimeNet_h
#define RemoteXYType_RealTimeNet_h

#include "RemoteXYType_RealTime.h"
#include "RemoteXYHttpRequest.h"

#define REMOTEXY_REALTIMENET_STATE_NO 0
#define REMOTEXY_REALTIMENET_STATE_LATENCY 1 
#define REMOTEXY_REALTIMENET_STATE_REQUEST 2

#define REMOTEXY_REALTIMENET_LATENCY_TIME 10000 

#define REMOTEXY_REALTIMENET_SINCHRONIZE_MILLIS REMOTEXY_MILLIS_PER_DAY

const char REMOTEXY_REALTIME_URL[] PROGMEM = "/realtime";


class CRemoteXYTypeInner_RealTimeNet : public CRemoteXYTypeInner_RealTime, CRemoteXYHttpRequestListener {

  public:
  CRemoteXYHttpRequest * httpRequest; // may be NULL   
  uint8_t httpRequestAnswerBuffer[8];
  uint8_t state;
  uint32_t timeout;
  int64_t answerTime;

  public:
  uint8_t* init (uint8_t *conf) override  {
    httpRequest = NULL;
    state = REMOTEXY_REALTIMENET_STATE_LATENCY;
    timeout = 0;
    answerTime = 0;
    return conf;
  };
         
  void setState (uint8_t _state) {
    state = _state; 
    if (state != REMOTEXY_REALTIMENET_STATE_REQUEST) httpRequest = NULL;
  }  
  
  void handler () override {
    if (state == REMOTEXY_REALTIMENET_STATE_LATENCY) {
      if (millis () - timeout >= REMOTEXY_REALTIMENET_LATENCY_TIME) {
        setState (REMOTEXY_REALTIMENET_STATE_NO);
      }
    }

    
    if (httpRequest != NULL) {
      httpRequest->handler ();
    }
    else {
      if (state == REMOTEXY_REALTIMENET_STATE_NO) {
        int32_t dt = (int32_t)(guiData->data->boardTime - answerTime);
        if ((answerTime == 0) || (dt >= REMOTEXY_REALTIMENET_SINCHRONIZE_MILLIS)) {

          // real time needs to be updated
          httpRequest = CRemoteXYHttpRequest::getHttpRequest (guiData->data->nets);
          if (httpRequest != NULL) {
            setState (REMOTEXY_REALTIMENET_STATE_REQUEST);
            httpRequest->setListener (this);
            httpRequest->setAnswerBuffer (httpRequestAnswerBuffer, 8);
            httpRequest->setRequest (
                FPSTR(REMOTEXY_HTTPREQUEST_HOST_REMOTEXY), 
                REMOTEXY_HTTPREQUEST_PORT_REMOTEXY, 
                FPSTR(REMOTEXY_REALTIME_URL),
                FPSTR(REMOTEXY_HTTPREQUEST_METHOD_GET));
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.init ();
            RemoteXYDebugLog.write(F("Send RealTime request..."));
#endif 
            httpRequest->send ();            
          }
        }
      }
    }
  }
  
  void httpRequestCompletion (uint8_t result) override {
    if (state == REMOTEXY_REALTIMENET_STATE_REQUEST) {
      if (result && (httpRequest!=NULL)) {
        if (httpRequest->getContentLength () == 8) {
          updateUtcTimeFromBuf (httpRequestAnswerBuffer);
          answerTime = guiData->data->boardTime;          
          setState (REMOTEXY_REALTIMENET_STATE_NO);
          return;
        }
      }
    }
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("RealTime request: ERROR"));
#endif 
    setState (REMOTEXY_REALTIMENET_STATE_LATENCY);
    timeout = millis ();
  }

  

};

#pragma pack(push, 1) 
class RemoteXYType_RealTimeNet : public RemoteXYType_RealTime {

  public:
  RemoteXYType_RealTimeNet () {
    inner = new CRemoteXYTypeInner_RealTimeNet ();
  }  
  
};
#pragma pack(pop)              


#endif // RemoteXYType_RealTimeNet_h