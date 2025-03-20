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
    
    complexVarCount = 0;    
    uint16_t eepromCount = 0;
    
    uint8_t* p = (uint8_t*)_conf;    
    uint8_t confVersion = rxy_readConfByte (p++);
           
    if (confVersion==0xff) {  
      // medium editor version
      // FF IL IL OL OL CL CL EV 
      inputLength = rxy_readConfByte (p++);
      inputLength |= rxy_readConfByte (p++)<<8;
      outputLength = rxy_readConfByte (p++); 
      outputLength |= rxy_readConfByte (p++)<<8; 
      confLength = rxy_readConfByte (p++);
      confLength |= rxy_readConfByte (p++)<<8;  
      conf = p; 
      editorVersion = rxy_readConfByte (p);
    }
    else {
      // old editor version
      // IL OL CL CL CONF
      inputLength = confVersion;
      outputLength = rxy_readConfByte (p++);    
      confLength = rxy_readConfByte (p++);
      confLength |= rxy_readConfByte (p++)<<8;  
      conf = p;
      editorVersion = 0;
    }     
    
    pv += inputLength;
    outputVar = pv;
    pv += outputLength;    
    rxy_bufClear (inputVar, inputLength+outputLength);    

    //complexVar = pv;
    
    if (editorVersion >= 20) {
      // CVL CVL CVDATA EL EL EDATA 
      complexVarCount = rxy_readConfByte (p++);
      complexVarCount |= rxy_readConfByte (p++)<<8; 
      
      // init complex vars
      complexVar = (CRemoteXYType**)malloc (sizeof(CRemoteXYType*) * complexVarCount);
      for (uint16_t i = 0; i < complexVarCount; i++) {
        complexVar[i] = (CRemoteXYType*)pv;
        ((CRemoteXYType*)pv)->setGuiData (this);
        p++;
        p = ((CRemoteXYType*)pv)->init (p);
        pv += ((CRemoteXYType*)pv)->sizeOf ();
      }
      
      eepromCount = rxy_readConfByte (p++);
      eepromCount |= rxy_readConfByte (p++)<<8;
      
      // init eeprom
      uint16_t v, s;
      for (uint16_t i = 0; i < eepromCount; i++) {      
        v  = rxy_readConfByte (p++); 
        v |= rxy_readConfByte (p++)<<8; 
        s = rxy_readConfByte (p++); 
        s |= rxy_readConfByte (p++)<<8; 
#if defined(REMOTEXY_HAS_EEPROM)
        data->eeprom.addItem (inputVar + v, s, v+(s>>6));
#endif         
      }  
    }
    
    connect_flag = pv; 
    *connect_flag = 0;   

    inputVarCopy = (uint8_t*)malloc (inputLength); 
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
    
    // threads handler
    
    CRemoteXYThread * pt = threads;
    uint8_t cntConnectFlag = 0;
    while (pt) {   
      pt->handler ();     
      cntConnectFlag += pt->connect_flag;
      pt = pt->next;
    }
    *connect_flag = cntConnectFlag;

    
    // connections handler    
    
    CRemoteXYConnectionNet * connection = connections; 
    while (connection) {
      connection->handler (); 
      connection = connection->next;
    }    
      
    // complex variables handler  
    
    for (uint16_t i = 0; i < complexVarCount; i++) {
      complexVar[i]->handler ();
    }
    
  }



  
};

#endif //RemoteXYGui_h