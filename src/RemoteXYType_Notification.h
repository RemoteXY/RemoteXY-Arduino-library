#ifndef RemoteXYType_Notification_h
#define RemoteXYType_Notification_h

#include "RemoteXYData.h"
#include "RemoteXYTypeUseHeap.h"
#include "RemoteXYEeprom.h"
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
  uint8_t messageId;
};
#pragma pack(pop)

class RemoteXYType_Notification : public CRemoteXYTypeUseHeap, CRemoteXYHttpRequestCompletion {

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
  uint16_t sizeOf () override {
    return sizeof (RemoteXYType_Notification);
  }
  
  uint8_t* init (uint8_t *conf) override  {
#if defined(REMOTEXY_NOTIFICATION_USE_SERVER_REQUEST)
    httpRequest = NULL;
    httpRequestState = REMOTEXY_NOTIFICATION_STATE_NO;
    if (data->boardId == NULL) {
      data->boardId = data->eeprom.addItem (REMOTEXY_BOARDID_LENGTH, REMOTEXY_BOARDID_EEPROM_KEY);      
    }
#endif
    return CRemoteXYTypeUseHeap::init (conf);
  };


#if defined(REMOTEXY_NOTIFICATION_USE_SERVER_REQUEST)

  void setState (uint8_t state) {
    httpRequestState = state; 
    if (state != REMOTEXY_NOTIFICATION_STATE_REQUEST) httpRequest = NULL;
    if (state == REMOTEXY_NOTIFICATION_STATE_LATENCY) httpRequestTimeout = millis ();
  }

  public: 
  void handler () override {    
    RemoteXYStorageHeap_Head heapHead;
    CRemoteXYData *data = guiData->data;
    
    if (data->realTime == NULL) data->realTime = new CRemoteXYRealTimeBoard ();

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
        if (heap.getFirst (&heapHead)) {
          uint8_t * boardId = data->boardId->data; 
          if (rxy_bufIsEmpty (boardId, REMOTEXY_BOARDID_LENGTH)) {       
            httpRequest = CRemoteXYHttpRequest::getHttpRequest (data->nets);
            if (httpRequest != NULL) {    
                    
              httpRequestMessageId = heapHead.id;
              uint16_t messageLength = heapHead.len;
              httpRequestPostDataLen = REMOTEXY_BOARDID_LENGTH+1+messageLength+sizeof (RemoteXYStorage_Heap_Head)+sizeof (RemoteXYTimeStamp);
              httpRequestPostData = (uint8_t*)malloc (httpRequestPostDataLen);
              if (httpRequestPostData == NULL) {
#if defined(REMOTEXY__DEBUGLOG)
                RemoteXYDebugLog.write("Out of RAM to send Notification request");              
#endif 
                setState (REMOTEXY_NOTIFICATION_STATE_LATENCY);
                return;
              }
              uint8_t *p = httpRequestPostData;
              
              // lib version
              *p++ = REMOTEXY_LIBRARY_VERSION;
              
              // board ID
              rxy_bufCopy (p, boardId, REMOTEXY_BOARDID_LENGTH);
              p+=REMOTEXY_BOARDID_LENGTH;
              
              // board time
              RemoteXYTimeStamp boardTime = data->realTime->getBoardTime (); 
              rxy_bufCopy (p, (uint8_t*)&boardTime, sizeof (RemoteXYTimeStamp));
              p+=sizeof (RemoteXYTimeStamp);

              // head
              rxy_bufCopy (p, (uint8_t*)&head, sizeof (RemoteXYStorage_Heap_Head));
              p+=sizeof (RemoteXYStorage_Heap_Head);   
               
              // message head and message          
              for (uint16_t i = 0; i < messageLength; i++) {
                *p++ = heap.getNextByte();
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
    RemoteXYStorageHeap_Head heapHead;
    if (httpRequestState == REMOTEXY_NOTIFICATION_STATE_REQUEST) {
      free (httpRequestPostData);
      if (httpRequest->getState () == REMOTEXY_HTTPREQUEST_OK) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write("Notification request: OK");
#endif 
        if (heap.getFirst (&heapHead)) {
          if (heapHead.id == httpRequestMessageId) {
            heap.removeFirst ();
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
  RemoteXYType_Notification_Head getHead (uint8_t messageId) {
    RemoteXYType_Notification_Head head;
    CRemoteXYData *data = guiData->data;
    if (data->realTime != NULL) {
      head.time = data->realTime->getBoardTime ();
    }
    else {
      head.time.setNull ();
    }
    head.messageId = messageId;
    return head;
  }
  
  
  public:
  uint8_t send (char * message) {
    uint16_t len = rxy_strLength (message); 
    if (len == 0) return 0;
    RemoteXYType_Notification_Head head = getHead (0);
    return addToHeap ((uint8_t*)&head, sizeof(RemoteXYType_Notification_Head), (uint8_t*)message, len);
  }
  
  
  public:
  uint8_t send (uint8_t messageId) {
    RemoteXYType_Notification_Head head = getHead (messageId);
    return addToHeap ((uint8_t*)&head, sizeof(RemoteXYType_Notification_Head));
  }  

  
};


#endif // RemoteXYType_Notification_h