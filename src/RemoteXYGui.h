#ifndef RemoteXYGui_h
#define RemoteXYGui_h  

#include "RemoteXYFunc.h" 
#include "RemoteXYTime.h"  
#include "RemoteXYData.h"   
#include "RemoteXYTypeIterator.h"  
#include "RemoteXYConnection.h"
#include "RemoteXYStream_Stream.h" 
#include "RemoteXYConnectionStream.h"

class CRemoteXYGui: public CRemoteXYGuiData {

  public:
  CRemoteXYGui *next;
  CRemoteXYTypeIterator complexVarIterator; 

  public:
  CRemoteXYGui (CRemoteXYData * _data, const void * _conf, void * _var, const char * _accessPassword = NULL) {
    data = _data;
    conf = (uint8_t*)_conf;
    inputVar = (uint8_t*)_var;
    setPassword (_accessPassword);   
    
    threads = NULL;
    connections = NULL;     
    
    complexVarCount = 0;    
    
    uint8_t* p = conf;    
    confVersion = rxy_readConfByte (p++);
    
    if (confVersion==0xfe) { 
      // new editor version
      // FE CL CL LV EV IL IL OL OL CVL CVL CVDATA EL EL EDATA CONF
      confLength = rxy_readConfByte (p++);
      confLength |= rxy_readConfByte (p++)<<8;
      uint8_t minLibVersion = rxy_readConfByte (p++); 
      if (minLibVersion > REMOTEXY_LIBRARY_VERSION) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write ("ERROR. RemoteXY.h library needs to be updated");
#endif
      }     
      editorVersion = rxy_readConfByte (p++); 
      inputLength = rxy_readConfByte (p++);
      inputLength |= rxy_readConfByte (p++)<<8;
      outputLength = rxy_readConfByte (p++); 
      outputLength |= rxy_readConfByte (p++)<<8;     
      complexVarCount = rxy_readConfByte (p++);
      complexVarCount |= rxy_readConfByte (p++)<<8; 
      complexVarConf = p;
    
      outputVar = inputVar + inputLength;
      complexVar = outputVar + outputLength;
      
      complexVarIterator.start (this);
      while (complexVarIterator.next()) {
        complexVarIterator.var->init(this);
      }
      connect_flag = (uint8_t*)complexVarIterator.var;     
      p = complexVarIterator.conf;
      
      uint16_t eepromCount = rxy_readConfByte (p++);
      eepromCount |= rxy_readConfByte (p++)<<8;
      while (eepromCount--) {
        uint16_t v = rxy_readConfByte (p++); 
        v |= rxy_readConfByte (p++)<<8; 
        uint16_t s = rxy_readConfByte (p++); 
        s |= rxy_readConfByte (p++)<<8; 
#if defined(REMOTEXY_HAS_EEPROM)
        data->eeprom.addItem (inputVar + v, s, v+(s>>6));
#endif         
      }           
    }
    else {
      if (confVersion==0xff) {  
        // medium editor version
        // FF IL IL OL OL CL CL EV CONF
        inputLength = rxy_readConfByte (p++);
        inputLength |= rxy_readConfByte (p++)<<8;
        outputLength = rxy_readConfByte (p++); 
        outputLength |= rxy_readConfByte (p++)<<8; 
      }
      else {
        // old editor version
        // IL OL CL CL CONF
        inputLength = confVersion;
        outputLength = rxy_readConfByte (p++);    
        confVersion = 0xff;
      }       
      confLength = rxy_readConfByte (p++);
      confLength |= rxy_readConfByte (p++)<<8;  
      conf = p;
      editorVersion = rxy_readConfByte (p);
      
      outputVar = inputVar + inputLength;
      connect_flag = outputVar + outputLength; 
    }
    
    
    rxy_bufClear (inputVar, inputLength+outputLength);    
    *connect_flag = 0;   
            
        
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
    /*
    if (data->httpRequest == NULL) {
      if (net->hasInternetAccess ()) data->httpRequest = new CRemoteXYHttpRequest (net); 
    } 
    */ 
  }
  
  
  public:
  void handler () {
    
    // threads handler
    
    CRemoteXYThread * pt = threads;
    uint8_t cnt = 0;
    while (pt) {   
      pt->handler ();     
      cnt += pt->connect_flag;
      pt = pt->next;
    }
    *connect_flag = cnt;

    
    // connections handler    
    
    CRemoteXYConnectionNet * connection = connections; 
    while (connection) {
      connection->handler (); 
      connection = connection->next;
    }    
      
    // complex variables handler  
        
    complexVarIterator.start (this);
    while (complexVarIterator.next()) {
      complexVarIterator.var->handler();
    }
    
  }



  
};

#endif //RemoteXYGui_h