#ifndef RemoteXYModem_h
#define RemoteXYModem_h

#include <stdarg.h>

#define REMOTEXY_MODEM_RECEIVE_BUFFER_SIZE 64
#define REMOTEXY_MODEM_RESPONSE_PARAM_COUNT 6


#define REMOTEXY_MODEM_RESPONSE_TIMOUT 1000
#define REMOTEXY_MODEM_TEST_TIMEOUT 30000
#define REMOTEXY_MODEM_FIND_LATENCY 2000
#define REMOTEXY_MODEM_RESEIVE_BYTES_TIMEOUT 1000

#define MODEM_STATE_WAIT_RESPONSE    60001
#define MODEM_STATE_FIND_MODEM       60002

const char MODEM_AT_RESPONSE_OK[] PROGMEM = "OK";
const char MODEM_AT_RESPONSE_ERROR[] PROGMEM = "ERROR";
const char MODEM_AT_COMMAND_AT[] PROGMEM = "AT";



class CRemoteXYModem: public CRemoteXYReadByteListener {

  protected:
  CRemoteXYStream * stream;

  char receiveBuffer[REMOTEXY_MODEM_RECEIVE_BUFFER_SIZE];
  uint16_t receiveBufferIndex;

  uint8_t responseParamsCount; 
  char * responseParams[REMOTEXY_MODEM_RESPONSE_PARAM_COUNT];
  uint8_t responseParamsLength[REMOTEXY_MODEM_RESPONSE_PARAM_COUNT];
    
  uint8_t responseTemplatesCount; 
  const __FlashStringHelper * responseTemplatesF[6];
  
  uint16_t readByteCount;
  
  uint16_t commandState; // == 0 no command sent
  
  uint32_t commandTime;
  uint32_t responseTimeOut; // if 0 then REMOTEXY_MODEM_RESPONSE_TIMOUT   
  uint32_t responseTime;
  int8_t responseTemplateId; 
  
  uint8_t modemOnline;  // 0 or 1
  uint8_t prevModemOnline;  // 0 or 1
  uint8_t findModemTrys;
  uint32_t modemLostTime;
  
  uint8_t haveEcho;
  
  uint8_t * sendBuffer;
  uint16_t sendBufferLength;
  


  public:
  CRemoteXYModem (CRemoteXYStream *_stream)  {
    stream = _stream;
    stream->setReadByteListener (this);
    receiveBufferIndex = 0; 
    commandState = 0;
    readByteCount = 0;
    
    responseTimeOut = 0;
    responseTime = 0;
    
    responseTemplateId = -1;  
    modemOnline = 0;
    prevModemOnline = 0;
    modemLostTime = 0;
    findModemTrys = 0;
    haveEcho = 0;
    sendBufferLength = 0;
  }
  
  public:
  virtual void onReceive (uint16_t cmState, uint8_t timeOut) = 0; 
  virtual void onReceiveByte (uint8_t byte) = 0; // not client
  virtual void onModemFound () = 0;
  virtual void onModemLost () = 0;
  
  
  public:
  void streamHandler () {
  
    stream->handler ();  
    
    if (commandState>0) {
      uint32_t timeOut = responseTimeOut;
      if (timeOut == 0) timeOut = REMOTEXY_MODEM_RESPONSE_TIMOUT;
      if (millis() - commandTime > timeOut) {
        responseReceived (1);
      }
    }
    
    if (readByteCount) {      
      if (millis() - commandTime > REMOTEXY_MODEM_RESEIVE_BYTES_TIMEOUT) {         
        readByteCount = 0;
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.writeAdd (F("* read bytes timeout *"));
#endif                 
      }
    }
  } 
  
  public:
  void modemHandler () {
  
    if (modemOnline != prevModemOnline) {
      prevModemOnline = modemOnline;
      if (modemOnline) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write ("Modem found");
#endif 
        onModemFound ();
      }
      else {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write ("Modem lost");
#endif 
        onModemLost ();
      }
    }
    if (modemOnline) {
      modemLostTime = millis ();
    }
    
    streamHandler ();    
    while (readByteCount > 0) streamHandler (); 

    if (((modemOnline == 0) && (millis() - modemLostTime > REMOTEXY_MODEM_FIND_LATENCY)) ||
        ((modemOnline != 0) && (millis() - responseTime > REMOTEXY_MODEM_TEST_TIMEOUT))) {
      if (readyForSendCommand ()) {
        haveEcho = 0;
        sendCommand (MODEM_STATE_FIND_MODEM, FPSTR(MODEM_AT_COMMAND_AT));
      }           
    }


  }
  
  
  public:
  uint8_t readyForSendCommand () {
    if ((commandState == 0) && (readByteCount == 0)) return 1;
    return 0;
  }
  
  public:
  uint8_t waitReadyForSendCommand () {
    uint32_t timeOut = millis ();
    while (!readyForSendCommand ()) {
      streamHandler ();
      if (millis () - timeOut > 10000) return 0;
    }
    return 1;
  }  

  // if templates not set then return 1 if OK
  public:  
  uint8_t waitResponseResult () { 
    if (responseTemplatesCount == 0) {
      setResultTemplates (FPSTR(MODEM_AT_RESPONSE_OK));
    }
    commandState = MODEM_STATE_WAIT_RESPONSE;
    responseTemplateId = -1;
    while (responseTemplateId < 0) {      
      streamHandler ();
    }    
    finishCommand ();   
    return responseTemplateId;
  }
  
  
  
  // set templates to wait for response to next command
  public:  
  void setResultTemplates (
       const __FlashStringHelper* t1 = NULL, 
       const __FlashStringHelper* t2 = NULL, 
       const __FlashStringHelper* t3 = NULL, 
       const __FlashStringHelper* t4 = NULL, 
       const __FlashStringHelper* t5 = NULL, 
       const __FlashStringHelper* t6 = NULL) { 
    responseTemplatesF[0] = t1;
    responseTemplatesF[1] = t2;
    responseTemplatesF[2] = t3;
    responseTemplatesF[3] = t4;
    responseTemplatesF[4] = t5;
    responseTemplatesF[5] = t6;
    responseTemplatesCount = 0;
    while (responseTemplatesF[responseTemplatesCount] != NULL) {
      responseTemplatesCount++;
      if (responseTemplatesCount >= 6) break; 
    }
  }
  
  
 
  // set timeout to wait for response to next command, ms
  public:
  void setTimeOutForNextCommand (uint32_t timeOut) {
    responseTimeOut = timeOut;
  }
  
  public:
  void setSendBytes (uint8_t * _sendBuffer, uint16_t _sendBufferLength) {
    sendBuffer = _sendBuffer;
    sendBufferLength = _sendBufferLength;
  }

  private:  
  void _sendCommand (const __FlashStringHelper * fs, va_list *argptr) {   
    char * arg;
    uint8_t c;
    PGM_P s = reinterpret_cast<PGM_P>(fs);
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.writeOutput ("");
#endif 
    while (1) {    
      c = rxy_pgm_read_byte(s);
      if (c == 0) break;
      if (c == '#') {
        arg = va_arg(*argptr, char*);
        stream->write (arg);
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.writeAdd (arg);
#endif 
      }
      else {  
        stream->write (c);  
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.writeAddChar (c);
#endif 
      }
      s++;
    }  
    stream->write (F("\r\n")); 
    stream->flush ();
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.writeNewString ();
#endif    
    commandTime = millis ();
  }

  // non-blocking function
  // use onReceive(cmState) to listen for the response
  // result: 
  // = 1 command sent; 
  // = 0 modem busy, command did't send
  public:
  uint8_t sendCommand (uint16_t cmState, const __FlashStringHelper * command, ...) {   
    if (!waitReadyForSendCommand ()) return 0;
    va_list argptr;
    va_start (argptr, command);
    _sendCommand (command, &argptr);
    va_end(argptr);   
    commandState = cmState;
    responseTemplatesCount = 0;
    return 1;
  }
  
  
  // blocking function
  // result: response.resultId
  public:
  uint8_t sendCommandForResult (const __FlashStringHelper * command, ...) {  
    if (!waitReadyForSendCommand ()) return 0;
    va_list argptr;
    va_start (argptr, command);
    _sendCommand (command, &argptr);
    va_end(argptr);                   
    return waitResponseResult ();  
  }
  
   
  
  public:
  void readByte (uint8_t byte) override {
  
    if (readByteCount > 0) {
      readByteCount--;             
      onReceiveByte (byte);
      if (readByteCount == 0) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.writeAdd (F(" received* "));
        RemoteXYDebugLog.writeNewString ();
#endif   
      }
      commandTime = millis ();
      return;
    }
    
    if (byte==10) return;
#if defined(REMOTEXY__DEBUGLOG)
    if (byte==13) RemoteXYDebugLog.writeNewString ();
    else RemoteXYDebugLog.writeInputChar (byte);
#endif 
    
    if (byte==13) {
      if (receiveBufferIndex > 0) { 
        receiveBuffer[receiveBufferIndex] = 0;
        receiveBufferIndex = 0;
        responseReceived (0);
      }      
    }
    else {
      if ((byte>0x60) && (byte<=0x7a)) byte-=0x20;
      if (receiveBufferIndex<REMOTEXY_MODEM_RECEIVE_BUFFER_SIZE-1) {
        receiveBuffer[receiveBufferIndex++] = byte;
      }
      if (byte == ' ') {
        if (receiveBufferIndex==1) receiveBufferIndex = 0;
      }
      else if (byte == ':') {
        receiveBuffer[receiveBufferIndex] = 0;
        receiveBufferIndex = 0;
        responseReceived (0); 
      }
      else if ((byte == '>') && (sendBufferLength > 0)) { // need send bytes
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.writeAdd (F(" *send "));
        RemoteXYDebugLog.writeAdd (sendBufferLength);
        RemoteXYDebugLog.writeAdd (F(" bytes *"));
#endif         
        stream->write (sendBuffer, sendBufferLength);
        stream->flush();
        receiveBufferIndex = 0;
      }
    }   
  }
  
  
  private:
  void responseReceived (uint8_t timeOut) {
    if (timeOut == 0) responseTime = millis ();
    if (commandState == MODEM_STATE_WAIT_RESPONSE) {  
      if (responseTemplateId < 0) {
        if (timeOut) responseTemplateId = 0;
        else responseTemplateId = findResponseTemplate ();
        if (responseTemplateId >= 0) return;
      }
    }
    else if (commandState == MODEM_STATE_FIND_MODEM) {
      if (responseReceivedFindModem (timeOut)) return;    
    }
    onReceive (commandState, timeOut);
    if (timeOut) finishCommand ();
  }
  
  private:
  uint8_t responseReceivedFindModem (uint8_t timeOut) {
    if (timeOut) {
      finishCommand (); 
      if (modemOnline) setModemLost ();
#if defined(REMOTEXY__DEBUGLOG)
      else {
        findModemTrys++;
        if (findModemTrys >=20) {
          RemoteXYDebugLog.write (F("Мodem does not respond to AT commands"));       
          findModemTrys = 0; 
        }
      }
#endif  
      return 1; 
    }
    
    else if (equalResponse (FPSTR(MODEM_AT_COMMAND_AT))) { 
      haveEcho = 1;
      return 1; 
    }     
    else if (equalResponse (FPSTR(MODEM_AT_RESPONSE_OK))) {
      finishCommand ();
      if (modemOnline == 0) setModemFound (); 
      else if (haveEcho && modemOnline) setModemLost ();
      return 1; 
    } 
    return 0; 
  }
  
  public:
  void finishCommand () {
    commandState = 0;
    responseTimeOut = 0;
    responseTemplatesCount = 0; 
    sendBufferLength = 0;
  }
  
  public:
  void setModemLost () {
#if defined(REMOTEXY__DEBUGLOG)
    findModemTrys = 0;
#endif                   
    modemOnline = 0;
    finishCommand (); 
    responseTemplateId = 0;
  }
  
  public:
  void setModemFound () {
    modemOnline = 1;
  }
  
  
  public:
  int8_t findResponseTemplate () {
    for (uint8_t k = 0; k < responseTemplatesCount; k++) {
      if (equalResponse (responseTemplatesF[k])) {
        return k+1;  
      }
    }
    return -1;
  }
  
  public:
  uint8_t equalResponse (const __FlashStringHelper* templ) {   
    if (_equalResponse (templ)) {
      for (uint8_t i = 0; i < responseParamsCount; i++) {
        *(responseParams[i] + responseParamsLength[i]) = 0;
      }  
      return 1;  
    }   
    return 0;
  }   
  
  private:
  uint8_t _equalResponse (const __FlashStringHelper * ftempl) {
    PGM_P templ = reinterpret_cast<PGM_P>(ftempl);
    char * p = receiveBuffer;
    responseParamsCount = 0;
    uint8_t c;
    uint8_t any = 0;
        
    while (1) {
      c = pgm_read_byte (templ);
      if (c == *p) {
        if (any) {
          any = 0;
          responseParamsCount++;
        }
        if (c == 0) break;
        templ++;
      }
      else {
        if (c == '#') {
          if (responseParamsCount >= REMOTEXY_MODEM_RESPONSE_PARAM_COUNT) return 0;
          responseParams[responseParamsCount] = p;
          responseParamsLength[responseParamsCount] = 1;
          any = 1;   
          templ++;     
        }
        else if (any) {
          responseParamsLength[responseParamsCount]++; 
        }
        else return 0;
      }
      p++;
    }    
    return 1;
  }
  
  /////////////////////////
  
  public:
  void setReadByteMode (uint16_t len) {
    readByteCount = len;  
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.writeNewString ();
    RemoteXYDebugLog.writeAdd (F(" * "));
    RemoteXYDebugLog.writeAdd (readByteCount);
    RemoteXYDebugLog.writeAdd (F(" bytes "));
#endif
    receiveBufferIndex = 0;
  }
        
};

#endif //RemoteXYModem_h 