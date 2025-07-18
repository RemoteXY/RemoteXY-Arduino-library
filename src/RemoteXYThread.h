#ifndef RemoteXYThread_h
#define RemoteXYThread_h



#include "RemoteXYGuiData.h"
#include "RemoteXYConnection.h"
#include "RemoteXYWire.h"
#include "RemoteXYType.h"


#define REMOTEXY_PACKAGE_START_BYTE 0x55
#define REMOTEXY_THREAD_TIMEOUT 8000



class CRemoteXYThread : public CRemoteXYReceivePackageListener {

  public:
  CRemoteXYThread * next;
  
  public:
  CRemoteXYGuiData * guiData;
  CRemoteXYConnection * conn;
  CRemoteXYWire * wire;  
  uint8_t clientId; 
  
  uint32_t timeOut;
  uint8_t stopByTimeOut;
  
  uint8_t inputVarNeedSend;
  uint8_t complexVarNeedSend;
    
  
  public:
  uint8_t connect_flag;

  private:
  CRemoteXYThread () {
    wire = NULL;
  }
  
  private:
  void init (CRemoteXYGuiData * _guiData) {
    guiData = _guiData;    
  }
  


  public:
  void begin (CRemoteXYConnection * _conn, CRemoteXYWire * _wire, uint8_t _stopByTimeOut) {
    conn = _conn;
    wire = _wire;         
    clientId = 0; 
    stopByTimeOut = _stopByTimeOut;
    timeOut = guiData->data->handlerMillis;
    connect_flag = 0;
    inputVarNeedSend = 0;
    complexVarNeedSend = 0;

#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("App client started"));
    //RemoteXYDebugLog.writeAvailableMemory ();
#endif
  }  
    
  void setClientId (uint8_t _clientId) {
    clientId = _clientId;
  }
  
  
  public:
  void stop () {
    if (wire) {
      conn->stopThreadListener (wire);
      //wire->setReceivePackageListener (NULL);
      wire = NULL;  // unused
      connect_flag = 0;
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("App client stoped"));
#endif 
    }
  }  
  
  public:
  uint8_t running () {
    if (wire) return 1;
    return 0;
  }

  public:
  void handler () {
    if (wire) {       
      conn->handleWire (wire);
      if (wire->running ()) {  
        if (guiData->data->handlerMillis - timeOut > REMOTEXY_THREAD_TIMEOUT) {
          timeOut = guiData->data->handlerMillis;
          connect_flag = 0;
          if (stopByTimeOut) stop (); 
        }     
      }  
      else stop ();
    }
  }
  
  public:
  void receivePackage (CRemoteXYPackage * package) override {
    uint16_t i, length;
    uint8_t *p, *kp;
    uint8_t allowAccess; 
    uint8_t b;  
    CRemoteXYData * data = guiData->data;
      
    if (wire == NULL) return;  
    if ((package->command != REMOTEXY_PACKAGE_COMMAND_GETCONF) && (!connect_flag)) return;
    switch (package->command) {  
      case REMOTEXY_PACKAGE_COMMAND_GETCONF: // send configuration or denied    
        allowAccess = 0;
        if (package->length==0) { 
          if (guiData->accessPassword == NULL) allowAccess=1;
          else if (*guiData->accessPassword == 0) allowAccess=1;
        }
        else {
          if (guiData->accessPassword != NULL) {
            allowAccess = rxy_strCompare ((char*)guiData->accessPassword, (char*)package->buffer);
          }                          
        } 
        if (allowAccess) {
          wire->sendConfPackage (REMOTEXY_PACKAGE_COMMAND_GETCONF, clientId);
          connect_flag = 1;
        }
        else {
          uint8_t buf[5];
          p = buf;
          kp = guiData->conf;         
          i=guiData->editorVersion>=5?3:2;
          length = i+2;
          *p++ = REMOTEXY_PACKAGE_VERSION;
          while (i--) *p++ = rxy_readConfByte(kp++);
          *p++ = 0xf0;
          wire->sendPackage (REMOTEXY_PACKAGE_COMMAND_GETCONF, clientId, buf, length);
        }          
        break;   
      case REMOTEXY_PACKAGE_COMMAND_ALLVAR: // send input and output vars 
        wire->sendPackage (REMOTEXY_PACKAGE_COMMAND_ALLVAR, clientId, guiData->inputVar, guiData->inputLength + guiData->outputLength); 
        inputVarNeedSend = 0;
        break;   
      case REMOTEXY_PACKAGE_COMMAND_INPUTVAR: // receive input vars 
        if ((package->length == guiData->inputLength) && (inputVarNeedSend==0)) {
          rxy_bufCopy (guiData->inputVar, guiData->inputVarCopy, package->buffer, guiData->inputLength);
          CRemoteXYThread::notifyInputVarNeedSend (guiData);  // notify other threads
          inputVarNeedSend = 0; // was change in notifyInputVarNeedSend
        }
        wire->sendEmptyPackage (REMOTEXY_PACKAGE_COMMAND_INPUTVAR, clientId);
        break;   
      case REMOTEXY_PACKAGE_COMMAND_OUTPUTVAR: // send output vars        
        b = 0;
        if (inputVarNeedSend != 0) b |= 0x01; 
        if (complexVarNeedSend != 0) b |= 0x02; 
        wire->startPackage (REMOTEXY_PACKAGE_COMMAND_OUTPUTVAR, clientId, guiData->outputLength + 1);
        wire->sendBytePackage (b);
        wire->sendBytesPackage (guiData->outputVar, guiData->outputLength);
        break;  
        
//////////////////////////////////////////////////    
// NEW COMMANDS v 3.2    
        
      
      case REMOTEXY_PACKAGE_COMMAND_BOARDID: // get/set board id    
#if defined (REMOTEXY_HAS_EEPROM)
        CRemoteXYEeprom * eeprom = &data->eeprom;
        CRemoteXYEepromItem * boardId = data->boardId;       
        if (boardId != NULL) {
          if (package->length==0) {
            if (boardId != NULL) {
              wire->sendPackage (REMOTEXY_PACKAGE_COMMAND_BOARDID, clientId, boardId->data, REMOTEXY_BOARDID_LENGTH);
            }
            else {      
              wire->sendEmptyPackage (REMOTEXY_PACKAGE_COMMAND_BOARDID, clientId);
            }
          } 
          else if (package->length==REMOTEXY_BOARDID_LENGTH) {
            if (boardId->data != NULL) {
              rxy_bufCopy (boardId->data, package->buffer, REMOTEXY_BOARDID_LENGTH);
              eeprom->writeItem (boardId);
            }
            wire->sendEmptyPackage (REMOTEXY_PACKAGE_COMMAND_BOARDID, clientId);
          }
        }
        else {
          wire->sendEmptyPackage (REMOTEXY_PACKAGE_COMMAND_BOARDID, clientId);
        }
#else
        wire->sendEmptyPackage (REMOTEXY_PACKAGE_COMMAND_BOARDID, clientId);
#endif        
        break;
                            
      case REMOTEXY_PACKAGE_COMMAND_TIME: { // get board time 
        uint8_t buf[8];        
        uint32_t* pbuf = (uint32_t*)buf;
        *pbuf++ = data->boardTime.getDays ();
        *pbuf = data->boardTime.getMillisSinceStartOfDay ();
        wire->sendPackage (REMOTEXY_PACKAGE_COMMAND_TIME, clientId, buf, 8); 
        break;    
      }            
      case REMOTEXY_PACKAGE_COMMAND_COMPLEXDESC: // send complex var descriptors
        sendComplexVarDescriptorsPacage ();
        complexVarNeedSend = 0;
        break;
                             
      case REMOTEXY_PACKAGE_COMMAND_COMPLEXDATA: // query complex variable
        b = 0;
        if (package->length >= 2) {
          i = *((uint16_t*)package->buffer);
          if (i < guiData->complexVarCount) {
            b = guiData->complexVar[i]->handlePackage (package, wire);
          }
        }
        if (b == 0) {
          wire->sendEmptyPackage (REMOTEXY_PACKAGE_COMMAND_COMPLEXDATA, clientId);
        }
        break;   
      case REMOTEXY_PACKAGE_COMMAND_DISCONNECT:
        wire->sendEmptyPackage (REMOTEXY_PACKAGE_COMMAND_DISCONNECT, clientId);
        stop ();
        break; 
    }  
    timeOut = guiData->data->handlerMillis;  
  }
  
  private:
  void sendComplexVarDescriptorsPacage () {  
    uint16_t length = 0;
    CRemoteXYTypeInner * var;
    for (uint16_t i = 0; i < guiData->complexVarCount; i++) {
      var = guiData->complexVar[i];
      length += var->getDescriptorLength ();
    }
    wire->startPackage (REMOTEXY_PACKAGE_COMMAND_COMPLEXDESC, clientId, length);
    for (uint16_t i = 0; i < guiData->complexVarCount; i++) {
      var = guiData->complexVar[i];
      var->sendDescriptorBytes (wire);
    }
  }
  

  public:
  static CRemoteXYThread * getUnusedThread (CRemoteXYGuiData * guiData) {    
    CRemoteXYThread * pt = guiData->threads;  
    uint8_t cnt = 0;          
    while (pt) {
      if (!pt->running ()) {
         return pt;
      }
      cnt++;
      pt = pt->next;
    }
    if (cnt < REMOTEXY_MAX_CLIENTS) { 
      pt = new CRemoteXYThread ();
      if (pt == NULL) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write (F("Out of RAM for new app client"));
#endif           
        return NULL;  
      }
      pt->init (guiData);
      pt->next = guiData->threads;
      guiData->threads = pt;
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write (F("New app client created"));
#endif      
      return pt; 
    }
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write (F("App client limit exceeded"));
#endif      
    return NULL; 
  }
  
  
  public:
  static void notifyInputVarNeedSend (CRemoteXYGuiData * guiData) {
    CRemoteXYThread * p = guiData->threads;
    while (p) {
      p->inputVarNeedSend = 1;
      p = p->next;
    }
  }
  
  public:
  static void notifyComplexVarNeedSend (CRemoteXYGuiData * guiData) {
    CRemoteXYThread * p = guiData->threads;
    while (p) {
      p->complexVarNeedSend = 1;
      p = p->next;
    }
  }
  
};


#endif //RemoteXYThread_h