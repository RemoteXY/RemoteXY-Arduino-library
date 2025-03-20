#ifndef RemoteXYNet_ESP8266_h
#define RemoteXYNet_ESP8266_h

#include "RemoteXYDebugLog.h"
#include "RemoteXYNet_AT.h"


#define REMOREXYNET_ESP8266__SERVER_TIMEOUT 10000
#define REMOREXYNET_ESP8266__WIFICONNECT_TIMEOUT 30000

#define REMOREXYNET_ESP8266__ID_WIFICONNECT 1

#define REMOREXYNET_ESP8266__MAX_SEND_BYTE_SIZE 2048      

const char * AT_MESSAGE_WIFI_DISCONNECT = "WIFI DISCONNECT";


class CRemoteXYNet_ESP8266_Proto : public CRemoteXYNet_AT {

  protected:  
  const char * wifiSsid;
  const char * wifiPassword;
  enum { Search, Reset, SearchAfterReset, Init, WaitWiFi, WaitReconnect, Configured } state;
  uint32_t timeOut;  
  
  public:
  CRemoteXYNet_ESP8266_Proto (CRemoteXYStream *_serial, const char * _wifiSsid, const char * _wifiPassword) : CRemoteXYNet_AT (_serial, REMOREXYNET_ESP8266__MAX_SEND_BYTE_SIZE) {
    wifiSsid = _wifiSsid;
    wifiPassword = _wifiPassword;
    state = Search;
    findModule ();
  }
  
  
  void moduleFound () override {       
    if ((state == Reset) || (state == SearchAfterReset)) moduleReset ();
    else {
      state = Reset;
      timeOut = millis ();
      sendATCommandForResult ("AT+RST",NULL);   // reset module  
    }  
  }
  
  protected:
  void moduleLost () override {         
    state = Search;
    findModule ();    
  }
  
  
  
  uint8_t configured () override {      
    return (state == Configured);
  }; 
  
  virtual CRemoteXYServer * createServer (uint16_t _port) override {  
    return new CRemoteXYServer_AT (this, _port); 
  } 
  
  public:
  CRemoteXYClient * newClient () override {  
    return new CRemoteXYClient_AT (this);
  }
  
};

#define CRemoteXYComm_ESP8266 CRemoteXYNet_ESP8266
class CRemoteXYNet_ESP8266 : public CRemoteXYNet_ESP8266_Proto {

  public:
  CRemoteXYNet_ESP8266 (CRemoteXYStream *_serial, const char * _wifiSsid, const char * _wifiPassword) : CRemoteXYNet_ESP8266_Proto (_serial, _wifiSsid, _wifiPassword) {
  }

  protected:
  void moduleReset () override {        
    if (!initModule ()) {
      state = Search;
      findModule ();
    }
  }
  


  private:
  uint8_t initModule () {
    state = Init;
    if (sendATCommandForResult ("ATE0",NULL) != AT_RESULT_OK) return 0;   
    if (sendATCommandForResult ("AT+CWMODE=1",NULL) != AT_RESULT_OK) return 0;     
    if (sendATCommandForResult ("AT+CWQAP",NULL) != AT_RESULT_OK) return 0; 
    if (sendATCommandForResult ("AT+CWDHCP=1,1",NULL) != AT_RESULT_OK) return 0; 
    if (sendATCommandForResult ("AT+CIPMODE=0",NULL) != AT_RESULT_OK) return 0;
    if (sendATCommandForResult ("AT+CIPMUX=1",NULL) != AT_RESULT_OK) return 0;
    beginWiFi ();
    return 1;
  }
  
  private:
  void beginWiFi () {
    state = Init;
    setATTimeOut (REMOREXYNET_ESP8266__WIFICONNECT_TIMEOUT);
    if (sendATCommand (REMOREXYNET_ESP8266__ID_WIFICONNECT, "AT+CWJAP=\"",wifiSsid,"\",\"",wifiPassword,"\"",NULL)) state = WaitWiFi;  
  }
  
  
  protected:
  void commandATListener (uint8_t identifier, uint8_t result) override  {
    if (identifier == REMOREXYNET_ESP8266__ID_WIFICONNECT) {
      if (result == AT_RESULT_OK) {
#if defined(REMOTEXY__DEBUGLOG)
        sendATCommandForResult ("AT+CIPSTA?",NULL);
#endif
        state = Configured;
      }
      else {
        state = WaitReconnect; 
        timeOut = millis();
      }
    }  
  }  
    
  virtual void handler () override {   // override CRemoteXYNet_AT  
    CRemoteXYNet_AT::handler ();
        
    if (state == Reset) {
      if (millis() - timeOut > 5000) {
        state = SearchAfterReset;
        findModule ();
      }
    }
    else if (state == Init) beginWiFi ();
    else if (state == WaitReconnect) {
      if (millis() - timeOut > REMOREXYNET_ESP8266__WIFICONNECT_TIMEOUT) beginWiFi ();
    }
  }
  
  uint8_t handleATMessage () override {    // override CRemoteXYNet_AT  
    if (strcmpReceiveBuffer (AT_MESSAGE_WIFI_DISCONNECT)==0) {
      if (state == Configured) state = Init;
      return 1;
    }
    return 0;
  }
  
  uint8_t hasInternet () override {
    return 1;
  }
  

};       


#define CRemoteXYComm_ESP8266Point CRemoteXYNet_ESP8266Point
class CRemoteXYNet_ESP8266Point : public CRemoteXYNet_ESP8266_Proto {

  public:
  CRemoteXYNet_ESP8266Point (CRemoteXYStream *_serial, const char * _wifiSsid, const char * _wifiPassword) : CRemoteXYNet_ESP8266_Proto (_serial, _wifiSsid, _wifiPassword) {
  }
  
  protected:
  void moduleReset () override {        
    initModule ();
    if (state != Configured) {
      state = Search;
      findModule ();
    }
  }
  
  private:
  void initModule () {
    state = Init;
    if (sendATCommandForResult ("ATE0",NULL) != AT_RESULT_OK) return;   
    if (sendATCommandForResult ("AT+CWMODE=2",NULL) != AT_RESULT_OK) return;    
    if (sendATCommandForResult ("AT+CWDHCP=0,1",NULL) != AT_RESULT_OK) return;
    
    char crypt[2] = {*wifiPassword?'4':'0',0};
    setATTimeOut (5000);
    if (sendATCommandForResult ("AT+CWSAP=\"",wifiSsid,"\",\"",wifiPassword,"\",10,",crypt,NULL) != AT_RESULT_OK) return;     
    if (sendATCommandForResult ("AT+CIPMODE=0",NULL) != AT_RESULT_OK) return;
    if (sendATCommandForResult ("AT+CIPMUX=1",NULL) != AT_RESULT_OK) return;
    state = Configured; 
  }
  
  virtual void handler () override {
    CRemoteXYNet_AT::handler ();
    if (state == Reset) {
      if (millis() - timeOut > 5000) {
        state = SearchAfterReset;
        findModule ();
      }
    }
  }
  
  
};      




#endif // RemoteXYNet_ESP8266_h
