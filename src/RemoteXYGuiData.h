#ifndef RemoteXYGuiData_h
#define RemoteXYGuiData_h

class CRemoteXYData;
class CRemoteXYThread;
class CRemoteXYStream;
class CRemoteXYNet;
class CRemoteXYConnectionNet;
class CRemoteXYType;
class CRemoteXYEeprom;

inline uint8_t rxy_readConfByte (uint8_t * p) {
  return pgm_read_byte_near (p);
}

class CRemoteXYGuiData {
  
  public:
  CRemoteXYData *data; 
    
  uint8_t editorVersion;
  uint8_t *conf;
  uint8_t *inputVar;
  uint8_t *outputVar;
  uint8_t *accessPassword;
  uint16_t outputLength;
  uint16_t inputLength;
  uint16_t confLength;
  uint8_t *connect_flag;

  CRemoteXYType **complexVar;  // array of pointer
  uint16_t complexVarCount;
  
  CRemoteXYThread * threads;  
  CRemoteXYConnectionNet * connections;  
  
  uint8_t *inputVarCopy;  
  
  public:
  uint16_t getReceiveBufferSize () {
    uint16_t receiveBufferSize = inputLength;
    if (receiveBufferSize < REMOTEXY_RECEIVE_BUFFER_MIN_LENGTH) receiveBufferSize = REMOTEXY_RECEIVE_BUFFER_MIN_LENGTH; 
    receiveBufferSize +=6;     
    return receiveBufferSize;
  }  
  
  public:
  void setPassword (const char * _accessPassword) {
    accessPassword = NULL;  
    if (_accessPassword != NULL) {
      accessPassword = (uint8_t*)_accessPassword;
    }   
  }
  
  
};


#endif //RemoteXYGuiData_h 