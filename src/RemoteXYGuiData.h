#ifndef RemoteXYGuiData_h
#define RemoteXYGuiData_h

class CRemoteXYData;
class CRemoteXYThread;
class CRemoteXYStream;
class CRemoteXYNet;
class CRemoteXYConnectionNet;
class CRemoteXYType;
class CRemoteXYTypeInner;
class CRemoteXYEeprom;

// receive buffer len
// > PASSWORD_LENGTH 
// > REMOTEXY_AES_KEY_SIZE+1 
// > REMOTEXY_BOARDID_LENGTH+1 
#define REMOTEXY_RECEIVE_BUFFER_MIN_LENGTH 33 

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
  uint8_t appConnectFlag;
  uint8_t *connect_flag;   // old version supported

  CRemoteXYTypeInner **complexVar;  // array of pointer
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