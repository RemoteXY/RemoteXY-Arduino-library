#ifndef RemoteXYThread_h
#define RemoteXYThread_h



#include "RemoteXYGuiData.h"
//#include "RemoteXYNet.h"
#include "RemoteXYConnection.h"
#include "RemoteXYWire.h"


#define REMOTEXY_INIT_CRC 0xffff
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
  
  uint8_t *inputVar;  
  
  uint32_t timeOut;
  uint8_t stopByTimeOut;
  
  uint8_t inputVarNeedSend;
    
  
  public:
  uint8_t connect_flag;

  private:
  CRemoteXYThread () {
    wire = NULL;
  }
  
  private:
  uint8_t init (CRemoteXYGuiData * _guiData) {
    guiData = _guiData;    
    inputVar = (uint8_t*)malloc (guiData->inputLength); 
    if (inputVar == NULL) return 0;
    copyInputVars ();  
    return 1;
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
        RemoteXYDebugLog.write ("Out of RAM for new app client");
#endif           
        return NULL;  
      }
      if (pt->init (guiData) == 0) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write ("Out of RAM for new app client");
#endif      
        return NULL;  
      }
      pt->next = guiData->threads;
      guiData->threads = pt;
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write ("New app client created");
#endif      
      return pt; 
    }
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write ("App client limit exceeded, see definition REMOTEXY_MAX_CLIENTS");
#endif      
    return NULL; 
  }

  public:
  void begin (CRemoteXYConnection * _conn, CRemoteXYWire * _wire, uint8_t _stopByTimeOut) {
    conn = _conn;
    wire = _wire;         
    clientId = 0; 
    stopByTimeOut = _stopByTimeOut;
    timeOut = millis ();
    connect_flag = 0;

#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("App client started");
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
      RemoteXYDebugLog.write("App client stoped");
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
        if (millis () - timeOut > REMOTEXY_THREAD_TIMEOUT) {
          timeOut = millis ();
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
    CRemoteXYData * data = guiData->data;
      
    if (wire == NULL) return;  
    if ((package->command != 0x00) && (!connect_flag)) return;
    switch (package->command) {  
      case 0x00:      
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
        if (allowAccess!=0) {
          wire->sendConfPackage (0x00, clientId);
          connect_flag = 1;
        }
        else {
          uint8_t buf[4];
          p = buf;
          kp = guiData->conf;         
          i=guiData->editorVersion>=5?3:2;
          length = i+1;
          while (i--) *p++ = rxy_readConfByte(kp++);
          *p++ = 0xf0;
          wire->sendPackage (0x00, clientId, buf, length);
        }          
        break;   
      case 0x40:  
        copyInputVars ();
        wire->sendPackage (0x40, clientId, guiData->inputVar, guiData->inputLength + guiData->outputLength); 
        break;   
      case 0x80:  
        checkInputVars ();       
        if ((package->length == guiData->inputLength) && (inputVarNeedSend==0)) {
          rxy_bufCopy (guiData->inputVar, inputVar, package->buffer, guiData->inputLength);
        }
        wire->sendEmptyPackage (0x80, clientId);
        break;   
      case 0xC0: 
        checkInputVars ();
        uint8_t c;
        if (inputVarNeedSend==0) c = 0xC0; 
        else c = 0xC1; 
        wire->sendPackage (c, clientId, guiData->outputVar, guiData->outputLength);
        break;  
        
//////////////////////////////////////////////////    
// NEW COMMANDS v 3.2    
        
      // get/set board id
      case 0x01:     
#if defined (REMOTEXY_HAS_EEPROM)
        RemoteXYEeprom * eeprom = &data->eeprom;
        RemoteXYEepromItem * boardId = data->boardId;       
        if (boardId != NULL) {
          if (package->length==0) {
            if (boardId != NULL) {
              wire->sendPackage (0x01, clientId, boardId->data, REMOTEXY_BOARDID_LENGTH);
            }
            else {      
              wire->sendEmptyPackage (0x01, clientId);
            }
          } 
          else if (package->length==REMOTEXY_BOARDID_LENGTH) {
            if (boardId->data != NULL) {
              rxy_bufCopy (boardId->data, package->buffer, REMOTEXY_BOARDID_LENGTH);
              eeprom->writeItem (boardId);
            }
            wire->sendEmptyPackage (0x01, clientId);
          }
        }
        else {
          wire->sendEmptyPackage (0x01, clientId);
        }
#else
        wire->sendEmptyPackage (0x01, clientId);
#endif        
        break;
        
      // set gmt time               
      case 0x02:
        if (data->realTime != NULL) {
          data->realTime->receivePackage (package);
        }
        wire->sendEmptyPackage (0x02, clientId);
        break;    
        
           
    }  
    timeOut = millis ();  
  }
  
  private:
  void copyInputVars () {
    inputVarNeedSend = 0;
    rxy_bufCopy (inputVar, guiData->inputVar, guiData->inputLength);
  }

  private:
  void checkInputVars () {
    if (inputVarNeedSend) return;
    if (!rxy_bufCompare (inputVar, guiData->inputVar, guiData->inputLength)) inputVarNeedSend = 1;
  }


  
};


#endif //RemoteXYThread_h