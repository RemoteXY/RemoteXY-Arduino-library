#ifndef RemoteXYType_Notification_h
#define RemoteXYType_Notification_h

#include "RemoteXYData.h"
#include "RemoteXYType.h"
#include "RemoteXYEeprom.h"
#include "RemoteXYStorege_Heap.h"
#include "RemoteXYNet.h"
#include "RemoteXYHttpRequest.h"

#if defined(REMOTEXY_HAS_EEPROM)
#define REMOTEXY_NOTIFICATION_USE_SERVER_REQUEST
#endif

const char * REMOTEXY_NOTIFICATION_URL = "/board/notification";

#define REMOTEXY_NOTIFICATION_STATE_NO 0                      
#define REMOTEXY_NOTIFICATION_STATE_LATENCY 1                                          
#define REMOTEXY_NOTIFICATION_STATE_REQUEST 2                                           
                   
               
#define REMOTEXY_NOTIFICATION_LATENCY_TIME 60000  
                   

#pragma pack(push, 1)
struct RemoteXYType_Notification_Head {
  RemoteXYTimeStamp time;  
  uint8_t track;
  uint8_t messageId;
};
#pragma pack(pop)


class CRemoteXYType_Notification : public CRemoteXYType, CRemoteXYHttpRequestCompletion {

  protected:
  CRemoteXYStorage_Heap * heap;

  private:
  CRemoteXYData *data;
  
#if defined(REMOTEXY_NOTIFICATION_USE_SERVER_REQUEST)
  CRemoteXYHttpRequest * httpRequest; // may be NULL  
  uint8_t httpRequestState;
  uint32_t httpRequestTimeout;
  uint8_t httpRequestAnswerBuffer[8];
  uint8_t *httpRequestPostData;
  uint16_t httpRequestPostDataLen;
  uint16_t httpRequestMessageId;
#endif  
 
  
  
  public: 
  CRemoteXYType_Notification () {
#if defined(REMOTEXY_NOTIFICATION_USE_SERVER_REQUEST)
    httpRequest = NULL;
    httpRequestState = REMOTEXY_NOTIFICATION_STATE_NO;
#endif  
  }
  
  public:
  void init (CRemoteXYGuiData * _guiData) override  {
    CRemoteXYType::init (_guiData);
    data = guiData->data;
#if defined(REMOTEXY_NOTIFICATION_USE_SERVER_REQUEST)
    if (data->boardId == NULL) {
      data->boardId = data->eeprom.addItem (REMOTEXY_BOARDID_LENGTH, REMOTEXY_BOARDID_EEPROM_KEY);      
    }
#endif
  };

#if defined(REMOTEXY_NOTIFICATION_USE_SERVER_REQUEST)

  void setState (uint8_t state) {
    httpRequestState = state; 
    if (state != REMOTEXY_NOTIFICATION_STATE_REQUEST) httpRequest = NULL;
    if (state == REMOTEXY_NOTIFICATION_STATE_LATENCY) httpRequestTimeout = millis ();
  }

  public: 
  void handler () override {    

    if (httpRequestState == REMOTEXY_NOTIFICATION_STATE_LATENCY) {
      if (millis () - httpRequestTimeout >= REMOTEXY_NOTIFICATION_LATENCY_TIME) {
        setState (REMOTEXY_NOTIFICATION_STATE_NO);
      }
    }

    if (httpRequest != NULL) {
      httpRequest->handler ();
    }
    else {
      if (httpRequestState == REMOTEXY_NOTIFICATION_STATE_NO) {
        if (heap->notEmpty ()) {
          uint8_t * boardId = data->boardId->data; 
          if (rxy_bufIsEmpty (boardId, REMOTEXY_BOARDID_LENGTH)) {       
            httpRequest = CRemoteXYHttpRequest::getHttpRequest (data->nets);
            if (httpRequest != NULL) {    
                    
              RemoteXYStorage_Heap_Head head = heap->getFirstHead ();
              httpRequestMessageId = head.id;
              uint16_t messageLength = head.len;
              httpRequestPostDataLen = REMOTEXY_BOARDID_LENGTH+1+messageLength+sizeof (RemoteXYStorage_Heap_Head);
              httpRequestPostData = (uint8_t*)malloc (httpRequestPostDataLen);
              if (httpRequestPostData == NULL) {
#if defined(REMOTEXY__DEBUGLOG)
                RemoteXYDebugLog.write("Out of RAM to send Notification request");              
#endif 
                setState (REMOTEXY_NOTIFICATION_STATE_LATENCY);
                return;
              }
              uint8_t *p = httpRequestPostData;
              *p++ = REMOTEXY_LIBRARY_VERSION;
              rxy_bufCopy (p, boardId, REMOTEXY_BOARDID_LENGTH);
              p+=REMOTEXY_BOARDID_LENGTH;
              
              uint8_t *ph = (uint8_t*)&head;
              for (uint16_t i = 0; i < sizeof (RemoteXYStorage_Heap_Head); i++) {
                *p++ = *ph++;
              }            
              p+=sizeof (RemoteXYStorage_Heap_Head);
                         
              for (uint16_t i = 0; i < messageLength; i++) {
                *p++ = heap->getNextByte();
              }                       
            
              setState (REMOTEXY_NOTIFICATION_STATE_REQUEST);
              httpRequest->setCompletion (this);
              httpRequest->setAnswerBuffer (httpRequestAnswerBuffer, 8);
              httpRequest->setRequest (
                  REMOTEXY_HTTPREQUEST_HOST_REMOTEXY, 
                  REMOTEXY_HTTPREQUEST_PORT_REMOTEXY, 
                  REMOTEXY_NOTIFICATION_URL,
                  REMOTEXY_HTTPREQUEST_METHOD_POST);
              httpRequest->setPostData (httpRequestPostData, httpRequestPostDataLen);    
#if defined(REMOTEXY__DEBUGLOG)
              RemoteXYDebugLog.write("Send Notification request...");
#endif 
              httpRequest->send ();            
            }
          }
        }
      }
    }  
  }

  
  void httpRequestCompletion () override {
    if (httpRequestState == REMOTEXY_NOTIFICATION_STATE_REQUEST) {
      free (httpRequestPostData);
      if (httpRequest->getState () == REMOTEXY_HTTPREQUEST_OK) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write("Notification request: OK");
#endif 
        if (heap->notEmpty ()) {
          RemoteXYStorage_Heap_Head head = heap->getFirstHead ();
          if (head.id == httpRequestMessageId) {
            heap->removeFirst ();
          }
        }
        setState (REMOTEXY_NOTIFICATION_STATE_NO);   
        return;
      }
    }
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Notification request: ERROR");
#endif 
    setState (REMOTEXY_NOTIFICATION_STATE_LATENCY);
  } 
#endif  // REMOTEXY_NOTIFICATION_USE_SERVER_REQUEST
  
  
  private:
  void addHeadToHeap (uint8_t messageId, uint8_t track) {
    RemoteXYType_Notification_Head head;
    if (data->realTime != NULL) {
      head.time = ((CRemoteXYRealTimeApp*)(data->realTime))->getTime ();
    }
    head.track = track;
    head.messageId = messageId;
    uint8_t *p = (uint8_t*)&head;
    for (uint8_t i = 0; i < sizeof(RemoteXYType_Notification_Head); i++) {
      heap->putNextByte (*p++);    
    }
  }
  
  public:
  uint8_t send (char * message, uint8_t track) {
    uint16_t len = rxy_strLength (message); 
    if (len == 0) return 0;
    if (heap->allocateNew (len + sizeof(RemoteXYType_Notification_Head))) {
      addHeadToHeap (0, track);
      while (len--) {                    
        heap->putNextByte (*message++);
      } 
      return 1;
    }
    return 0;
  }
  
  uint8_t send (char * message) {
    return send (message, 0);
  }
  
  
  public:
  uint8_t send (uint8_t messageId, uint8_t track) {
    if (heap->allocateNew (sizeof(RemoteXYType_Notification_Head))) {
      addHeadToHeap (messageId, track);
      return 1;
    }
    return 0;
  }  
  
  uint8_t send (uint8_t messageId) {
    return send (messageId, 0);
  }
  
  
};

class RemoteXYType_Notification_64 : public CRemoteXYType_Notification {
  public:
  CRemoteXYStorage_Heap_64 heapInstance;    
  RemoteXYType_Notification_64 () : CRemoteXYType_Notification() {
    heap = &heapInstance;
  }
};

class RemoteXYType_Notification_128 : public CRemoteXYType_Notification {
  public:
  CRemoteXYStorage_Heap_128 heapInstance;    
  RemoteXYType_Notification_128 () : CRemoteXYType_Notification() {
    heap = &heapInstance;
  }
};

class RemoteXYType_Notification_256 : public CRemoteXYType_Notification {
  public:
  CRemoteXYStorage_Heap_256 heapInstance;    
  RemoteXYType_Notification_256 () : CRemoteXYType_Notification() {
    heap = &heapInstance;
  }
};

class RemoteXYType_Notification_512 : public CRemoteXYType_Notification {
  public:
  CRemoteXYStorage_Heap_512 heapInstance; 
  RemoteXYType_Notification_512 () : CRemoteXYType_Notification() {
    heap = &heapInstance;
  }
};

#endif // RemoteXYType_Notification_h