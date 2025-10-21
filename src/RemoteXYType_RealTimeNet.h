#ifndef RemoteXYType_RealTimeNet_h
#define RemoteXYType_RealTimeNet_h

#include "RemoteXYType_RealTime.h"
#include "RemoteXYServerRequest.h"

#define REMOTEXY_REALTIMENET_STATE_NO 0
#define REMOTEXY_REALTIMENET_STATE_LATENCY 1 
#define REMOTEXY_REALTIMENET_STATE_REQUEST 2

#define REMOTEXY_REALTIMENET_LATENCY_TIME 60000 

#define REMOTEXY_REALTIMENET_SINCHRONIZE_MS REMOTEXY_MILLIS_PER_DAY

const char REMOTEXY_REALTIME_COMMAND[] PROGMEM = "realtime";


class CRemoteXYTypeInner_RealTimeNet : public CRemoteXYTypeInner_RealTime, CRemoteXYServerRequestListener {

  public:
  CRemoteXYServerRequest * serverRequest; // may be NULL   
  uint8_t requestReceiveBuffer[sizeof (int64_t)];
  uint8_t requestState;
  uint32_t requestTimeout;
  int64_t synchronizeTime;  

  public:
  uint8_t* init (uint8_t *conf) override  {
    serverRequest = NULL;
    requestState = REMOTEXY_REALTIMENET_STATE_NO;
    requestTimeout = 0;
    synchronizeTime = 0;
    return conf;
  };
      
  
  void handler () override {
    if (requestState == REMOTEXY_REALTIMENET_STATE_LATENCY) {
      if (millis () - requestTimeout >= REMOTEXY_REALTIMENET_LATENCY_TIME) {
        requestState = REMOTEXY_REALTIMENET_STATE_NO;
      }
    }

    
    if (serverRequest != NULL) {
      serverRequest->handler ();
    }
    else {
      if (requestState == REMOTEXY_REALTIMENET_STATE_NO) {
        if (synchronizeTime <= guiData->data->boardTime) {
          
          // real time needs to be updated
          serverRequest = CRemoteXYServerRequest::getServerRequest (guiData->data->nets);
          if (serverRequest != NULL) {
            requestState = REMOTEXY_REALTIMENET_STATE_REQUEST;
            serverRequest->setListener (this);
            serverRequest->setReceiveBuffer (requestReceiveBuffer, sizeof (int64_t));
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.init ();
            RemoteXYDebugLog.write(F("Send RealTime request..."));
#endif 
            serverRequest->send (FPSTR(REMOTEXY_REALTIME_COMMAND), 0);
          }
        }
      }
    }
  }
  
  private:
  void closeServerRequest (uint8_t state) {
    serverRequest->close ();
    serverRequest = NULL;
    requestState = state;  
  }
  
  
  void serverRequestCompletion (uint8_t result) override {
    if (serverRequest != NULL) {
      if (result && (requestState == REMOTEXY_REALTIMENET_STATE_REQUEST)) {
        if (serverRequest->getReceiveDataLength () == sizeof (int64_t)) {
          updateUtcTimeFromBuf (requestReceiveBuffer);
          synchronizeTime = guiData->data->boardTime + REMOTEXY_REALTIMENET_SINCHRONIZE_MS; 
          closeServerRequest (REMOTEXY_REALTIMENET_STATE_NO);         
          return;
        }
      }   
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("RealTime request: ERROR"));
#endif 
      closeServerRequest (REMOTEXY_REALTIMENET_STATE_LATENCY); 
      requestTimeout = millis ();
    }
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