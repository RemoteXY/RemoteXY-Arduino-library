#ifndef RemoteXYType_Notification_h
#define RemoteXYType_Notification_h

#if defined REMOTEXY_HAS_EEPROM

#include "RemoteXYData.h"
#include "RemoteXYType.h"
#include "RemoteXYEeprom.h"
#include "RemoteXYStorege_Heap.h"
#include "RemoteXYNet.h"
#include "RemoteXYHttpRequest.h"

const char * REMOTEXY_NOTIFICATION_URL = "/api/notification";

#define REMOTEXY_NOTIFICATION_STATE_NO 0                      
#define REMOTEXY_NOTIFICATION_STATE_CONNECTING 1                      
#define REMOTEXY_NOTIFICATION_STATE_WAITANSWER 2                                          
#define REMOTEXY_NOTIFICATION_STATE_WAITRECONNECT 9                      
                   

               
#define REMOTEXY_NOTIFICATION_WAITRECONNECT_TIMEOUT 60000  


#define REMOTEXY_NOTIFICATION_HTTP_VERSION 1                      

#pragma pack(push, 1)
struct RemoteXYType_Notification_Head {
  RemoteXYTime time;  
  uint8_t track;
  uint8_t messageId;
};
#pragma pack(pop)

class RemoteXYType_Notification : public RemoteXYType {

  protected:
  RemoteXYStorage_Heap * heap;

  private:
  //CRemoteXYNet * net;
  uint8_t state;
  uint32_t timeOut;
  CRemoteXYHttpRequest * httpRequest;
  uint16_t boardIdEepromAddress;
  uint16_t sendMessageId;
  
  
  public: 
  RemoteXYType_Notification () {
    state = REMOTEXY_NOTIFICATION_STATE_NO;
   // client = NULL;
  }
  
  public:
  void init (CRemoteXYGuiData * _guiData) override  {
    RemoteXYType::init (_guiData);
    CRemoteXYData *data = guiData->data;
    if (data->boardId == NULL) {
      data->boardId = data->eeprom.addItem (REMOTEXY_BOARDID_LENGTH, REMOTEXY_BOARDID_EEPROM_KEY);
    }
  };
          
  
  /*
  public: 
  void findClient () {
    CRemoteXYNet * _net = guiData->nets;
    while (_net) {
      if (_net->configured ()) {
        if (_net->hasInternetAccess ()) break;
      }
    }
    if (_net) {
      client = _net->newClient ();
      if (client != NULL) {
        net = _net;
        httpRequest.init (client);      
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write ("Notification initiated");
#endif  
      }    
    }  
  }
  */
  
  void findHttpRequest () {
    CRemoteXYNet * net = guiData->data->nets;
    while (net) {
      if (_net->configured ()) {
        httpRequest = net->getHttpRequest (); 
        if (httpRequest != NULL) break;
      }
    }
  }
  
  void unuseHttpRequest () {
    httpRequest->unused (); 
    httpRequest = NULL;  
  }

  public: 
  void handler () override {    

    uint8_t * boardId = guiData->data->boardId->data;
    if (boardId == NULL) return; // no board ID
    
    if (state == REMOTEXY_NOTIFICATION_STATE_NO) {
      if (heap->notEmpty ()) {
        if (net->configured ()) {
          if (rxy_bufIsEmpty (boardId, REMOTEXY_BOARDID_LENGTH)) {
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.write ("Notification connects to host");
#endif      
            findHttpRequest ();
            if (httpRequest == NULL) return;

            httpRequest.connect (REMOTEXY_HTTPREQUEST_HOST_REMOTEXY, REMOTEXY_HTTPREQUEST_PORT_REMOTEXY);
            timeOut = millis(); 
            state = REMOTEXY_NOTIFICATION_STATE_CONNECTING;
          }
        }
      }
    }
    else if (state == REMOTEXY_NOTIFICATION_STATE_CONNECTING) {
      if (httpRequest.getState () == REMOTEXY_HTTPREQUEST_CONNECTED) {
        if (heap->notEmpty ()) {

          RemoteXYStorage_Heap_Head head = heap->getFirstHead ();
          sendMessageId = head.id;
          uint16_t messageLength = head.len;
        
          uint16_t contentLength = 
            REMOTEXY_BOARDID_LENGTH + messageLength + 1;
            
          httpRequest.setMethod (REMOTEXY_HTTPREQUEST_METHOD_POST);  
          httpRequest.setUrl (REMOTEXY_NOTIFICATION_URL);  
          httpRequest.sendStartHeaders ();
          httpRequest.sendHeaderContentLength (contentLength);
          httpRequest.sendEndHeaders ();
    
          uint16_t bufLen = REMOTEXY_BOARDID_LENGTH+1;
          if (messageLength > bufLen) bufLen = messageLength;
          uint8_t buf[bufLen];
          uint8_t *p;
          
          // boardId   
          p = buf;
          *p++ = REMOTEXY_NOTIFICATION_HTTP_VERSION;
          rxy_bufCopy (p, boardId, REMOTEXY_BOARDID_LENGTH);
          httpRequest.sendData (buf, REMOTEXY_BOARDID_LENGTH+1);
          
          // message   
          p = buf;                 
          for (uint16_t i = 0; i < messageLength; i++) {
            *p++ = heap->getNextByte();
          }
          httpRequest.sendData (buf, messageLength);
          
          timeOut = millis(); 
          state = REMOTEXY_NOTIFICATION_STATE_WAITANSWER;
#if defined(REMOTEXY__DEBUGLOG)
          RemoteXYDebugLog.write ("Notification answer sent");
#endif      
          return;

        }
      }
      else {
        state = REMOTEXY_NOTIFICATION_STATE_WAITRECONNECT; 
        unuseHttpRequest ();
      }
    }
    else if (state == REMOTEXY_NOTIFICATION_STATE_WAITANSWER) {
      if (httpRequest.getState () == REMOTEXY_HTTPREQUEST_ANSWER_OK) {
        if (heap->notEmpty ()) {
          RemoteXYStorage_Heap_Head head = heap->getFirstHead ();
          if (head.id == sendMessageId) {
            heap->removeFirst ();
          }
        }
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write ("Notification received by host");
#endif      
        state = REMOTEXY_NOTIFICATION_STATE_NO;
        unuseHttpRequest ();
      }
      else if (httpRequest.getState () == REMOTEXY_HTTPREQUEST_ERROR) {
        state = REMOTEXY_NOTIFICATION_STATE_WAITRECONNECT;
        unuseHttpRequest ();
      } 
    }
    else if (state == REMOTEXY_NOTIFICATION_STATE_WAITRECONNECT) {
      if (millis() - timeOut > REMOTEXY_NOTIFICATION_WAITRECONNECT_TIMEOUT) {
        state = REMOTEXY_NOTIFICATION_STATE_NO;
      }
    }
  }
  
  
  private:
  void addHeadToHeap (uint8_t messageId, uint8_t track) {
    RemoteXYType_Notification_Head head;
    guiData->data->getGMTTime (&head.time);
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

class RemoteXYType_Notification_64 : public RemoteXYType_Notification {
  public:
  RemoteXYStorage_Heap_64 heapInstance;    
  RemoteXYType_Notification_64 () : RemoteXYType_Notification() {
    heap = &heapInstance;
  }
};

class RemoteXYType_Notification_128 : public RemoteXYType_Notification {
  public:
  RemoteXYStorage_Heap_128 heapInstance;    
  RemoteXYType_Notification_128 () : RemoteXYType_Notification() {
    heap = &heapInstance;
  }
};

class RemoteXYType_Notification_256 : public RemoteXYType_Notification {
  public:
  RemoteXYStorage_Heap_256 heapInstance;    
  RemoteXYType_Notification_256 () : RemoteXYType_Notification() {
    heap = &heapInstance;
  }
};

class RemoteXYType_Notification_512 : public RemoteXYType_Notification {
  public:
  RemoteXYStorage_Heap_512 heapInstance; 
  RemoteXYType_Notification_512 () : RemoteXYType_Notification() {
    heap = &heapInstance;
  }
};

#endif // REMOTEXY_HAS_EEPROM
#endif // RemoteXYType_Notification_h