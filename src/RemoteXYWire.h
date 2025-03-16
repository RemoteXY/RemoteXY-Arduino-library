#ifndef RemoteXYWire_h
#define RemoteXYWire_h

#include "RemoteXYFunc.h"
#include "RemoteXYStream.h"
#include "RemoteXYGuiData.h"


#define REMOTEXY_PACKAGE_START_BYTE 0x55
#define REMOTEXY_PACKAGE_MIN_LENGTH 6

struct CRemoteXYPackage {
  uint8_t command;
  uint8_t clientId;
  uint8_t * buffer;
  uint16_t length;
};

class CRemoteXYReceivePackageListener {
  public:
  virtual void receivePackage (CRemoteXYPackage * package) = 0;
};


class CRemoteXYWire : public CRemoteXYReadByteListener {

  public:
  CRemoteXYGuiData * guiData;  
  CRemoteXYWire * next;  
  CRemoteXYStream * stream; 
  
  private:
  CRemoteXYReceivePackageListener * receivePackageListener;
  
  private: 
  uint16_t sendCRC;
 
  uint8_t *receiveBuffer;
  uint16_t receiveBufferSize;
  uint16_t receiveIndex;
  uint8_t receiveModified;
  volatile uint8_t receiveLock;  // =1 only add to receive buffer
    
  
  public:
  CRemoteXYWire (CRemoteXYGuiData * _guiData) {
    guiData = _guiData;
    stream = NULL;
    receivePackageListener = NULL;
    receiveBufferSize = guiData->getReceiveBufferSize ();
    receiveBuffer = (uint8_t*)malloc (receiveBufferSize);    
  }

  public:
  CRemoteXYWire (CRemoteXYGuiData * _guiData, uint8_t multiple) {
    guiData = _guiData;
    stream = NULL;
    receivePackageListener = NULL;
    receiveBufferSize = guiData->getReceiveBufferSize () * multiple;
    receiveBuffer = (uint8_t*)malloc (receiveBufferSize);    
  }  
  
  public: 
  void setReceivePackageListener (CRemoteXYReceivePackageListener * listener) {
    receivePackageListener = listener;
  }    
  
  public:
  void notifyReceivePackageListener (CRemoteXYPackage * package) {
    if (receivePackageListener) receivePackageListener->receivePackage (package);
  } 
  
  public:
  void begin (CRemoteXYStream * _stream) {    
    stream = _stream;
    stream->setReadByteListener (this);  
    receiveIndex = 0;  
    receiveModified = 0;
    receiveLock = 0;
  }   
  
  public:    
  void stop () {
    if (stream) {
      stream->setReadByteListener (NULL);
      stream = NULL;
    }
  }
  
  public:
  uint8_t running () {   
    if (stream) return 1;
    else return 0;
  }
 
  
  public:
  void handler () {
    if (stream) {
      stream->handler ();
      receivePackage ();
    }
  }
  
  
  private:  
  inline void sendByteUpdateCRC (uint8_t b) {
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.writeOutputHex (b);  
#endif 
    stream->write (b);
    rxy_updateCRC (&sendCRC, b);   
  } 

  public:
  void startPackage (uint8_t command, uint8_t clientId, uint16_t length) {
    if (stream) {
      length+=6;    
      rxy_initCRC (&sendCRC);
      stream->startWrite (length);
            
      sendByteUpdateCRC (REMOTEXY_PACKAGE_START_BYTE);
      sendByteUpdateCRC (length);
      sendByteUpdateCRC (length>>8);
      sendByteUpdateCRC (command | (clientId << 1));      
    }
  }
  
  public:
  void sendBytePackage (uint8_t b) {
    if (stream) {
      sendByteUpdateCRC (b);
    }
  }
  
  public:
  void endPackage () {
    if (stream) {
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.writeOutputHex (sendCRC);  
      RemoteXYDebugLog.writeOutputHex (sendCRC>>8);  
#endif  
      stream->write (sendCRC);
      stream->write (sendCRC>>8); 
    } 
  }
  
        
  public:
  void sendPackage (uint8_t command, uint8_t clientId, uint8_t *buf, uint16_t length) {
    startPackage (command, clientId, length);
    while (length--) {
      sendBytePackage (*buf++);
    }
    endPackage ();  
  }
  
  public:
  void sendConfPackage (uint8_t command, uint8_t clientId) {
    uint8_t *p = guiData->conf;
    uint16_t length = guiData->confLength;
    uint8_t confVersion = guiData->confVersion;
    startPackage (command, clientId, length);
    for (uint16_t i = 0; i<length; i++) {
      if ((confVersion == 0xfe) && (i == 2)) sendBytePackage (REMOTEXY_LIBRARY_VERSION);
      else sendBytePackage (rxy_readConfByte (p++));
    }
    endPackage ();  
  }
   
  public:
  void sendEmptyPackage (uint8_t command, uint8_t clientId) {
    startPackage (command, clientId, 0);
    endPackage ();  
  }

  public:
  void readByte (uint8_t byte) override {
    uint16_t pi, i;
       
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.writeInputHex (byte);  
#endif   
    if ((receiveIndex==0) && (byte!=REMOTEXY_PACKAGE_START_BYTE)) return; 
    if (receiveIndex >= receiveBufferSize) {
      if (receiveLock) return;      
      pi = 1;
      while (pi < receiveBufferSize) {
        if (receiveBuffer[pi] == REMOTEXY_PACKAGE_START_BYTE) break;
        pi++;
      }      
      receiveIndex = receiveBufferSize - pi;
      i=0;
      while (pi < receiveBufferSize) receiveBuffer[i++] = receiveBuffer[pi++];       
    }
    receiveBuffer[receiveIndex++]=byte;  
    receiveModified = 1; 
  }
  
  private:
  void receivePackage () {
    if (receiveModified) {
      receiveModified = 0;
      
      uint16_t crc; 
      uint16_t si, i;
      uint16_t packageLength;
      si = 0;
      while (si + REMOTEXY_PACKAGE_MIN_LENGTH <= receiveIndex) {   
        if (receiveBuffer[si] == REMOTEXY_PACKAGE_START_BYTE) {
          packageLength = receiveBuffer[si+1]|(receiveBuffer[si+2]<<8);
          if ((packageLength <= receiveIndex - si) && (packageLength >=6)) {
            rxy_initCRC (&crc);
            for (i = si; i < si + packageLength; i++) rxy_updateCRC (&crc, receiveBuffer[i]); 
            if (crc == 0) {
              CRemoteXYPackage package;
              uint8_t cm = receiveBuffer[si+3];
              package.command = cm & 0xf1;
              package.clientId = (cm >> 1) & 0x07;
              package.buffer = receiveBuffer+si+4;
              package.length = packageLength-6;

              receiveLock = 1;
              notifyReceivePackageListener (&package);   
              si += packageLength;
              i = 0;
              while (si < receiveIndex) receiveBuffer[i++] = receiveBuffer[si++];
              receiveIndex = i;
              receiveLock = 0;
              si = 0;
              continue;
            }
          }
        }
        si++; 
      }
    }   
  }
  
};


#endif  //  RemoteXYWire_h