#ifndef RemoteXYNet_ModemESP8266_h
#define RemoteXYNet_ModemESP8266_h

#include "RemoteXYNet_Modem.h"


#define REMOREXY_MODEM_ESP8266_RESET_TIMEOUT 3000
#define REMOREXY_MODEM_ESP8266_STATION_CONNECT_TIMEOUT 30000
#define REMOREXY_MODEM_ESP8266_POINT_CREATE_TIMEOUT 5000
#define REMOREXY_MODEM_ESP8266_CLIENT_SEND_BYTE_TIMEOUT 15000


#define REMOTEXY_MODEM_ESP8266_CLIENT_COUNT 4

#define REMOTEXY_MODEM_ESP8266_STATE_RESET 1
#define REMOTEXY_MODEM_ESP8266_STATE_INIT 2
#define REMOTEXY_MODEM_ESP8266_STATE_WIFICONNECT 3
#define REMOTEXY_MODEM_ESP8266_STATE_CONFIGURED 4

const char REMOTEXY_MODEM_ESP8266_RESPONSE_BUSY[] PROGMEM = "BUSY #";

#define UNUSED(x) (void)(x)

class CRemoteXYNet_ModemESP8266_Generic : public CRemoteXYNet_Modem {
  
  protected:
  uint8_t state;
  uint32_t timeout;

  public:
  CRemoteXYNet_ModemESP8266_Generic (CRemoteXYStream * stream) : CRemoteXYNet_Modem (stream) {
    state = 0;
    setMaxClientCount (REMOTEXY_MODEM_ESP8266_CLIENT_COUNT);  
  }
  
  virtual uint8_t initModem () = 0;
  
  
  void onModemFound () override {
    state = REMOTEXY_MODEM_ESP8266_STATE_RESET;
    timeout = millis ();
    if (sendCommandForResult (F("AT+RST")) != 1) {
      setModemLost ();
    } 
  }
  
  void onModemLost () override {
    state = 0;  
  }  
  
  
  void handler () override { 
    CRemoteXYNet_Modem::handler ();
    if (state == REMOTEXY_MODEM_ESP8266_STATE_RESET) {
      if (millis () - REMOREXY_MODEM_ESP8266_RESET_TIMEOUT > timeout) {
        state = REMOTEXY_MODEM_ESP8266_STATE_INIT; 
      }
    }
    else if (state == REMOTEXY_MODEM_ESP8266_STATE_INIT) {
      if (initModem () == 0) {
        setModemLost ();
      }      
    }     
  } 
         
  public:     
  uint8_t startServer (uint16_t port) override {
    char sport[6];    
    rxy_intToStr (port, sport);
    if (sendCommandForResult (F("AT+CIPSERVER=1,#"), sport) == 0) return 0;  
    if (sendCommandForResult (F("AT+CIPSTO=30")) == 0) return 0;  
    return 1;
  }    
  
  public:
  uint8_t stopServer () override {
    return sendCommandForResult (F("AT+CIPSERVER=0"));  
  }
  

  public:
  uint8_t doConnectClient (uint8_t clientId, const char *host, uint16_t port) override {
    char sport[6];    
    char sclientId[4];  
    rxy_intToStr (port, sport);   
    rxy_intToStr (clientId, sclientId);  
    doDisconnectClient (clientId);
    setTimeOutForNextCommand (REMOTEXY_NET_MODEM_CLIENT_CONNECT_TIMEOUT);   
    setResultTemplates (FPSTR(MODEM_AT_RESPONSE_OK), FPSTR(MODEM_AT_RESPONSE_ERROR), FPSTR(REMOTEXY_MODEM_ESP8266_RESPONSE_BUSY));
    if (sendCommandForResult (F("AT+CIPSTART=#,\"TCP\",\"#\",#"), sclientId, host, sport) != 1) return 0; 
    return 1;
  }   
 
  public:
  uint8_t doDisconnectClient (uint8_t clientId) override {
    char sclientId[4];  
    rxy_intToStr (clientId, sclientId);  
    setResultTemplates (FPSTR(MODEM_AT_RESPONSE_OK), FPSTR(MODEM_AT_RESPONSE_ERROR), FPSTR(REMOTEXY_MODEM_ESP8266_RESPONSE_BUSY));
    if (sendCommandForResult (F("AT+CIPCLOSE=#"),sclientId) != 1) return 0; 
    return 1;
  }       
  
  public:
  uint8_t sendBytes (uint8_t clientId, uint8_t * buf, uint16_t len) override {
    char sclientId[4];  
    char slen[6];    
    rxy_intToStr (clientId, sclientId);  
    rxy_intToStr (len, slen);  
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.writeNewString ();
#endif   
    setSendBytes (buf, len); 
    setTimeOutForNextCommand (REMOREXY_MODEM_ESP8266_CLIENT_SEND_BYTE_TIMEOUT);   
    setResultTemplates (F("SEND OK"), F("SEND FAIL"), FPSTR(MODEM_AT_RESPONSE_ERROR), FPSTR(REMOTEXY_MODEM_ESP8266_RESPONSE_BUSY));
    if (sendCommandForResult (F("AT+CIPSEND=#,#"), sclientId, slen) != 1) {
      return 0; 
    }
    return 1;
  }
  
  
  void onReceive (uint16_t cmState, uint8_t timeOut) override {
     
    if (equalResponse (F("+IPD,#,#:"))) {
      uint16_t clientId = rxy_strParseInt (responseParams[0]);
      uint16_t len = rxy_strParseInt (responseParams[1]);
      setReadByteClientId (clientId, len);
    } 
    else if (equalResponse (F("READY"))) { 
      if (cmState) finishCommand ();
      if (state == REMOTEXY_MODEM_ESP8266_STATE_RESET) {
        state = REMOTEXY_MODEM_ESP8266_STATE_INIT;     
      }
      else {
        setModemLost ();
      }
    }
    else if (equalResponse (F("#,CONNECT"))) {
      clientConnected (rxy_strParseInt (responseParams[0]));
    }     
    else if (equalResponse (F("#,CLOSED")) || equalResponse (F("#,CONNECT FAIL"))) {
      markClientDisconnected (rxy_strParseInt (responseParams[0]));
    }
    UNUSED (timeOut);
  }
    
  
  public:                   
  uint8_t configured () override {
    return state == REMOTEXY_MODEM_ESP8266_STATE_CONFIGURED;
  }                
  
    
};

#define CRemoteXYComm_ESP8266 CRemoteXYNet_ModemESP8266  
class CRemoteXYNet_ModemESP8266 : public CRemoteXYNet_ModemESP8266_Generic {
  const char * wifiSsid;
  const char * wifiPassword;

  public:
  CRemoteXYNet_ModemESP8266 (CRemoteXYStream * stream, const char * _wifiSsid, const char * _wifiPassword) : CRemoteXYNet_ModemESP8266_Generic (stream) {
    wifiSsid = _wifiSsid;
    wifiPassword = _wifiPassword;  
  }


  private:
  uint8_t initModem () override {
    if (sendCommandForResult (F("ATE0")) == 0) return 0;   
    if (sendCommandForResult (F("AT+CWMODE=1")) == 0) return 0;     
    if (sendCommandForResult (F("AT+CWQAP")) == 0) return 0; 
    if (sendCommandForResult (F("AT+CWDHCP=1,1")) == 0) return 0; 
    if (sendCommandForResult (F("AT+CIPMODE=0")) == 0) return 0;
    if (sendCommandForResult (F("AT+CIPMUX=1")) == 0) return 0;
    setTimeOutForNextCommand (REMOREXY_MODEM_ESP8266_STATION_CONNECT_TIMEOUT);
    state = REMOTEXY_MODEM_ESP8266_STATE_WIFICONNECT;
    sendCommand (10, F("AT+CWJAP=\"#\",\"#\""), wifiSsid, wifiPassword); 
    return 1;
  }
  
  public:
  void onReceive (uint16_t cmState, uint8_t timeOut) override {
    CRemoteXYNet_ModemESP8266_Generic::onReceive (cmState, timeOut);
    
    // messages
    if (equalResponse (F("WIFI DISCONNECT"))) { 
      if (state == REMOTEXY_MODEM_ESP8266_STATE_CONFIGURED)  {
#if defined(REMOTEXY__DEBUGLOG)                 
        RemoteXYDebugLog.write (F("WiFi disconnected")); 
#endif        
        setModemLost ();
      }
    }
    
    // commands
    else if (cmState == 10) {  // AT+CWJAP="#","#"
      if (timeOut) {
        finishCommand ();
        setModemLost ();
      }
      else if (equalResponse (FPSTR(MODEM_AT_RESPONSE_OK))) {
        finishCommand ();
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write (F("WiFi connected"));
        sendCommandForResult (F("AT+CIPSTA?"));
#endif
        state = REMOTEXY_MODEM_ESP8266_STATE_CONFIGURED;
      }
      else if (equalResponse (F("FAIL"))) {
        finishCommand ();
        setModemLost ();
      }      
    }    
  }     
  
  
  uint8_t hasInternet () override {
    return 1;
  }  
};


#define CRemoteXYComm_ESP8266Point CRemoteXYNet_ModemESP8266_Point
class CRemoteXYNet_ModemESP8266_Point : public CRemoteXYNet_ModemESP8266_Generic {
  const char * wifiSsid;
  const char * wifiPassword;
  
  public:
  CRemoteXYNet_ModemESP8266_Point (CRemoteXYStream * stream, const char * _wifiSsid, const char * _wifiPassword) : CRemoteXYNet_ModemESP8266_Generic (stream) {
    wifiSsid = _wifiSsid;
    wifiPassword = _wifiPassword;  
  }
                
  private:
  uint8_t initModem () override {
  
    if (sendCommandForResult (F("ATE0")) == 0) return 0;   
    if (sendCommandForResult (F("AT+CWMODE=2")) == 0) return 0;    
    if (sendCommandForResult (F("AT+CWDHCP=0,1")) == 0) return 0;
    
    char crypt[2] = {*wifiPassword?'4':'0',0};
    setTimeOutForNextCommand (REMOREXY_MODEM_ESP8266_POINT_CREATE_TIMEOUT);
    if (sendCommandForResult (F("AT+CWSAP=\"#\",\"#\",10,#"), wifiSsid, wifiPassword, crypt) == 0) return 0;     
    if (sendCommandForResult (F("AT+CIPMODE=0")) == 0) return 0;
    if (sendCommandForResult (F("AT+CIPMUX=1")) == 0) return 0;
    state = REMOTEXY_MODEM_ESP8266_STATE_CONFIGURED;
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("WiFi point created"));    
#endif
    return 1;
  }
  

};  

#endif //RemoteXYNet_ModemESP8266_h