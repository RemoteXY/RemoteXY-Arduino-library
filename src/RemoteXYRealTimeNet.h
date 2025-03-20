#ifndef RemoteXYRealTimeNet_h
#define RemoteXYRealTimeNet_h

const char * REMOTEXY_REALTIME_URL = "/board/realtime";


#include "RemoteXYRealTime.h"
#include "RemoteXYNet.h"
#include "RemoteXYHttpRequest.h"

#define REMOTEXY_REALTIMENET_STATE_NO 0
#define REMOTEXY_REALTIMENET_STATE_LATENCY 1 
#define REMOTEXY_REALTIMENET_STATE_REQUEST 2

#define REMOTEXY_REALTIMENET_LATENCY_TIME 10000 

#define REMOTEXY_REALTIMENET_SINCHRONIZE_DAYS 3

class CRemoteXYRealTimeNet : public CRemoteXYRealTimeApp, CRemoteXYHttpRequestCompletion {
  
  public:
  CRemoteXYNet * nets;
  CRemoteXYHttpRequest * httpRequest; // may be NULL   
  uint8_t httpRequestAnswerBuffer[8];
  uint8_t state;
  uint32_t timeout;
  RemoteXYTimeStamp answerTime;

  CRemoteXYRealTimeNet (CRemoteXYNet * _nets) : CRemoteXYRealTimeApp () {
    nets = _nets;
    httpRequest = NULL;
    state = REMOTEXY_REALTIMENET_STATE_NO;
    answerTime.setNull ();
  }
  
  void setState (uint8_t _state) {
    state = _state; 
    if (state != REMOTEXY_REALTIMENET_STATE_REQUEST) httpRequest = NULL;
  }
    
    
  void handler () override {
    CRemoteXYRealTimeBoard::handler ();
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
        RemoteXYTimeStamp dt = boardTime;
        dt.sub (answerTime);
        if ((answerTime.isNull()) || (dt.getDays() > REMOTEXY_REALTIMENET_SINCHRONIZE_DAYS)) {

          // real time needs to be updated
          httpRequest = CRemoteXYHttpRequest::getHttpRequest (nets);
          if (httpRequest != NULL) {
            setState (REMOTEXY_REALTIMENET_STATE_REQUEST);
            httpRequest->setCompletion (this);
            httpRequest->setAnswerBuffer (httpRequestAnswerBuffer, 8);
            httpRequest->setRequest (
                REMOTEXY_HTTPREQUEST_HOST_REMOTEXY, 
                REMOTEXY_HTTPREQUEST_PORT_REMOTEXY, 
                REMOTEXY_REALTIME_URL,
                REMOTEXY_HTTPREQUEST_METHOD_GET);
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.init ();
            RemoteXYDebugLog.write("Send RealTime request...");
#endif 
            httpRequest->send ();            
          }
        }
      }
    }
  }
  
  void httpRequestCompletion () override {
    if (state == REMOTEXY_REALTIMENET_STATE_REQUEST) {
      if (httpRequest->getState () == REMOTEXY_HTTPREQUEST_OK) {
        if (httpRequest->getContentLength () == 8) {
          updateFromBuf (httpRequestAnswerBuffer);
          answerTime = boardTime;          
          setState (REMOTEXY_REALTIMENET_STATE_NO);
          return;
        }
      }
    }
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("RealTime request: ERROR");
#endif 
    setState (REMOTEXY_REALTIMENET_STATE_LATENCY);
    timeout = millis ();
  }

  
};


#endif //RemoteXYRealTimeNet_h