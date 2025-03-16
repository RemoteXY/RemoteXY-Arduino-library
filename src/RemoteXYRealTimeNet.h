#ifndef RemoteXYRealTimeNet_h
#define RemoteXYRealTimeNet_h

const char * REMOTEXY_REALTIME_URL = "/board/realtime";


#include "RemoteXYRealTime.h"
#include "RemoteXYNet.h"
#include "RemoteXYHttpRequest.h"

#define REMOTEXYREALTIMENET_STATE_NO 0
#define REMOTEXYREALTIMENET_STATE_LATENCY 1 
#define REMOTEXYREALTIMENET_STATE_REQUEST 2

#define REMOTEXYREALTIMENET_LATENCY_TIME 10000 

class CRemoteXYRealTimeNet : public CRemoteXYRealTimeApp, CRemoteXYHttpRequestCompletion {
  
  public:
  CRemoteXYNet * nets;
  CRemoteXYHttpRequest * httpRequest; // may be NULL   
  uint8_t buf[8];
  uint8_t state;
  uint32_t timeout;
  

  CRemoteXYRealTimeNet (CRemoteXYNet * _nets) : CRemoteXYRealTimeApp () {
    nets = _nets;
    httpRequest = NULL;
    state = REMOTEXYREALTIMENET_STATE_NO;
  }
  
  void setState (uint8_t _state) {
    state = _state; 
    if (state != REMOTEXYREALTIMENET_STATE_REQUEST) httpRequest = NULL;
  }
    
    
  void handler () override {

    CRemoteXYRealTimeApp::handler ();
    
    if (state == REMOTEXYREALTIMENET_STATE_LATENCY) {
      if (millis () - timeout >= REMOTEXYREALTIMENET_LATENCY_TIME) {
        setState (REMOTEXYREALTIMENET_STATE_NO);
      }
    }

    
    if (httpRequest != NULL) {
      httpRequest->handler ();
    }
    else {
      if (state == REMOTEXYREALTIMENET_STATE_NO) {
        if ((realTime.isNull()) || (shiftTime.getDays() > 7)) {

          // real time needs to be updated
          httpRequest = CRemoteXYHttpRequest::getHttpRequest (nets);
          if (httpRequest != NULL) {
            setState (REMOTEXYREALTIMENET_STATE_REQUEST);
            httpRequest->setCompletion (this);
            httpRequest->setAnswerBuffer (buf, 8);
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
    if (state == REMOTEXYREALTIMENET_STATE_REQUEST) {
      if (httpRequest->getState () == REMOTEXY_HTTPREQUEST_OK) {
        if (httpRequest->getContentLength () == 8) {
          updateFromBuf (buf);
#if defined(REMOTEXY__DEBUGLOG)
          RemoteXYDebugLog.write("Get RealTime answer: ");
          RemoteXYDebugLog.writeAdd(realTime.getDays());
          RemoteXYDebugLog.writeAdd(" days ");
          RemoteXYDebugLog.writeAdd(realTime.getMillis());
          RemoteXYDebugLog.writeAdd(" millis");
#endif 
          setState (REMOTEXYREALTIMENET_STATE_NO);
          return;
        }
      }
    }
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("RealTime request error");
#endif 
    setState (REMOTEXYREALTIMENET_STATE_LATENCY);
    timeout = millis ();
  }

  
};


#endif //RemoteXYRealTimeNet_h