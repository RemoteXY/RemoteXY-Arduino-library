#ifndef RemoteXYGui_h
#define RemoteXYGui_h  

#include "RemoteXYFunc.h" 
#include "RemoteXYTime.h"  
#include "RemoteXYData.h"   
#include "RemoteXYConnection.h"
#include "RemoteXYStream_Stream.h" 
#include "RemoteXYConnectionStream.h"
#include "RemoteXYType.h"   

class CRemoteXYGui: public CRemoteXYGuiData {

  public:
  CRemoteXYGui *next;

  public:
  CRemoteXYGui (CRemoteXYData * _data, const void * _conf, void * _var, const char * _accessPassword = NULL) {

    data = _data;

    uint8_t* pv = (uint8_t*)_var;    
    inputVar = pv;
    
    threads = NULL;
    connections = NULL;     
    
    connect_flag = NULL; 
    
    complexVarCount = 0;    
    uint16_t eepromCount = 0;
    
    uint8_t* p = (uint8_t*)_conf;    
    editorVersion = 0;
    uint8_t confVersion = rxy_readConfByte (p++);
           
    if (confVersion >= 0xfe)   {  
      // medium editor version
      // FF/FE IL IL OL OL 
      inputLength = rxy_readConfByte (p++);
      inputLength |= rxy_readConfByte (p++)<<8;
      outputLength = rxy_readConfByte (p++); 
      outputLength |= rxy_readConfByte (p++)<<8; 
    }
    else {
      // old editor version
      // IL OL 
      inputLength = confVersion;
      outputLength = rxy_readConfByte (p++);    
    }     
    
    pv += inputLength;
    outputVar = pv;
    pv += outputLength;    
        
    if (confVersion == 0xfe) {
      // CVL CVL CVDATA EL EL EDATA 
      complexVarCount = rxy_readConfByte (p++);
      complexVarCount |= rxy_readConfByte (p++)<<8; 
      // init complex vars
      complexVar = (CRemoteXYTypeInner**)malloc (sizeof(CRemoteXYTypeInner*) * complexVarCount);
      for (uint16_t i = 0; i < complexVarCount; i++) {
        CRemoteXYTypeInner * inner = ((CRemoteXYType*)pv)->inner;
        complexVar[i] = inner;
        inner->setGuiData (this);
        p = inner->init (p);
        pv += ((CRemoteXYType*)pv)->getTypeSize();
      }
      
      eepromCount = rxy_readConfByte (p++);
      eepromCount |= rxy_readConfByte (p++)<<8;
      
      // init eeprom
      uint16_t v, s, b;
      for (uint16_t i = 0; i < eepromCount; i++) {      
        v = rxy_readConfByte (p++); 
        s = rxy_readConfByte (p++); 
        b = rxy_readConfByte (p++); 
        v |= (b & 0x3f) << 8; 
        s |= (b & 0xc0) << 2; 
#if defined(REMOTEXY_HAS_EEPROM)
        data->eeprom.addItem (inputVar + v, s, v+(s>>6));
#endif         
      }  

    }
    
    //CL CL CONF
    confLength = rxy_readConfByte (p++);
    confLength |= rxy_readConfByte (p++)<<8;  
    conf = p; 
    
    if (confVersion >= 0xfe) {
      // EV
      editorVersion = rxy_readConfByte (p);
    }
    
    
    appConnectFlag = 0;
    if (confVersion != 0xfe) {
      connect_flag = pv; 
      *connect_flag = appConnectFlag;  
    }

    inputVarCopy = (uint8_t*)malloc (inputLength); 
    rxy_bufClear (inputVar, inputLength+outputLength);    
    rxy_bufCopy (inputVarCopy, inputVar, inputLength);
     
    setPassword (_accessPassword);   
  }
 
 
  public:  
  void addConnection (CRemoteXYStream * stream) {  
    CRemoteXYConnection * conn = new CRemoteXYConnectionStream (stream);
    conn->init (this);           
  } 


  public:  
  void addConnection (Stream * stream) {  
    CRemoteXYStream_Stream * comm = new CRemoteXYStream_Stream (stream);
    CRemoteXYConnection * conn = new CRemoteXYConnectionStream (comm);
    conn->init (this);           
  }
   
  public:  
  void addConnection (CRemoteXYConnectionNet * conn) {
    addNet (conn->net);   
    conn->next = connections;
    connections = conn; 
    conn->init (this);    
  } 
  
  private:
  void addNet (CRemoteXYNet * net) {
    CRemoteXYNet * p = data->nets;
    while (p) {
      if (p == net) return;
      p = p->next;
    }
    net->next = data->nets;
    data->nets = net;
  }
  
  
  public:
  void handler () {
    
    if (!rxy_bufCompare (inputVarCopy, inputVar, inputLength)) {
      rxy_bufCopy (inputVarCopy, inputVar, inputLength);
      CRemoteXYThread::notifyInputVarNeedSend (this); // notify all threads that user change input vars
    }
    
    // complex variables handler     
    for (uint16_t i = 0; i < complexVarCount; i++) {
      complexVar[i]->handler ();
    }
    
    // threads handler
    CRemoteXYThread * pt = threads;
    appConnectFlag = 0;
    while (pt) {   
      pt->handler ();     
      appConnectFlag += pt->connect_flag;
      pt = pt->next;
    }
    if (connect_flag) {
      *connect_flag = appConnectFlag;
    }
    
    // connections handler    
    CRemoteXYConnectionNet * connection = connections; 
    while (connection) {
      connection->handler (); 
      connection = connection->next;
    }    
    
  }


  public:
  uint8_t appConnected () {
    return appConnectFlag;
  }
  
};

#endif //RemoteXYGui_h