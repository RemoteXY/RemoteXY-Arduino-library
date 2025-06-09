#ifndef RemoteXYWire_h
#define RemoteXYWire_h

#include "RemoteXYFunc.h"
#include "RemoteXYStream.h"
#include "RemoteXYGuiData.h"

#define REMOTEXY_PACKAGE_VERSION 20

#define REMOTEXY_PACKAGE_START_BYTE 0x55
#define REMOTEXY_PACKAGE_MIN_LENGTH 6
#define REMOTEXY_PACKAGE_SEND_FRAGMENT_SIZE_DEF 1024 

#define REMOTEXY_PACKAGE_COMMAND_GETCONF      0x00
#define REMOTEXY_PACKAGE_COMMAND_PING         0x10
#define REMOTEXY_PACKAGE_COMMAND_REGCLOUD     0x11 // for board
#define REMOTEXY_PACKAGE_COMMAND_TIME         0x20
#define REMOTEXY_PACKAGE_COMMAND_BOARDID      0x30
#define REMOTEXY_PACKAGE_COMMAND_ALLVAR       0x40
#define REMOTEXY_PACKAGE_COMMAND_INPUTVAR     0x80
#define REMOTEXY_PACKAGE_COMMAND_COMPLEXDESC  0xA0
#define REMOTEXY_PACKAGE_COMMAND_COMPLEXDATA  0xB0
#define REMOTEXY_PACKAGE_COMMAND_OUTPUTVAR    0xC0  
#define REMOTEXY_PACKAGE_COMMAND_DISCONNECT   0xE0  


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
  
  uint16_t sendFragmentSize;
  
  // for send package
  uint8_t sendCommand; // &clientId  
  uint16_t sendLength; // all bytes
  uint8_t sendFragmentId; 
  uint8_t sendFragmentLastId; 
  uint16_t sendFragmentLength;
  
  public:
  CRemoteXYWire (CRemoteXYGuiData * _guiData) {
    init (_guiData, 1);    
  }

  public:
  CRemoteXYWire (CRemoteXYGuiData * _guiData, uint8_t receiveBufferMultiple) {
    init (_guiData, receiveBufferMultiple);    
  }  
  
  private:
  void init (CRemoteXYGuiData * _guiData, uint8_t receiveBufferMultiple) {
    guiData = _guiData;
    stream = NULL;
    receivePackageListener = NULL;
    receiveBufferSize = guiData->getReceiveBufferSize () * receiveBufferMultiple;
    receiveBuffer = (uint8_t*)malloc (receiveBufferSize); 
    sendFragmentSize = REMOTEXY_PACKAGE_SEND_FRAGMENT_SIZE_DEF;       
  }
  
  public:
  void setSendFragmentSize (uint16_t _sendFragmentSize) {
    sendFragmentSize = _sendFragmentSize;
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
    if (stream) return stream->connected();
    return 0;
  }
 
  
  public:
  void handler () {
    if (running ()) {
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

  private:
  void sendPackageHeader () {
    
    sendFragmentLength = sendLength;
    if (sendFragmentLastId != 0) {
      if (sendFragmentLength > sendFragmentSize - 8 ) {
        sendFragmentLength = sendFragmentSize - 8;
      }
    }
    sendLength -= sendFragmentLength;
    
    if (stream) {   
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.writeNewString ();
#endif 
      rxy_initCRC (&sendCRC); 
      uint16_t length = sendFragmentLength + 6;
      uint8_t command = sendCommand;
      if (sendFragmentLastId != 0) {
        length += 2;  
        command |= 0x01;
      }
      //stream->startWrite (length);
      sendByteUpdateCRC (REMOTEXY_PACKAGE_START_BYTE);
      sendByteUpdateCRC (length);
      sendByteUpdateCRC (length>>8);
      sendByteUpdateCRC (command);  
      if (sendFragmentLastId != 0) {
        sendByteUpdateCRC (sendFragmentLastId);
        sendByteUpdateCRC (sendFragmentId);      
      }
    }    
  }


  public:
  void startPackage (uint8_t command, uint8_t clientId, uint16_t length) {
    sendCommand = command | ((clientId & 0x07) << 1);
    sendLength = length;      
    if ((length + 6 <= sendFragmentSize) || ((command & 0xf0) == 0x10)) {
      sendFragmentLastId = 0;
    } 
    else {
      sendFragmentLastId = length / (sendFragmentSize - 8);
    }  
    sendFragmentId = 0;
    sendPackageHeader ();
    if (length == 0) endPackage ();
  }
  
  public:
  void sendBytePackage (uint8_t b) {
    if (stream) sendByteUpdateCRC (b);
    sendFragmentLength--;
    if (sendFragmentLength == 0) {
      endPackage ();
      if (sendFragmentId < sendFragmentLastId) {
        sendFragmentId++;
        sendPackageHeader ();
      }
    }
  }
  
  public:
  void sendBytesPackage (uint8_t * buf, uint16_t len) {
    while (len--) sendBytePackage (*buf++);
  }
  
  private:
  void endPackage () {
    if (stream) {
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.writeOutputHex (sendCRC);  
      RemoteXYDebugLog.writeOutputHex (sendCRC>>8);  
#endif  
      stream->write (sendCRC);
      stream->write (sendCRC>>8); 
      stream->flush ();
    } 
  }
  
        
  public:
  void sendPackage (uint8_t command, uint8_t clientId, uint8_t *buf, uint16_t length) {
    startPackage (command, clientId, length);
    sendBytesPackage (buf, length);
  }
  
  public:
  void sendConfPackage (uint8_t command, uint8_t clientId) {
    uint8_t *p = guiData->conf;
    uint16_t length = guiData->confLength;
    startPackage (command, clientId, length+1); 
    sendBytePackage (REMOTEXY_PACKAGE_VERSION);
    while (length--) {
      sendBytePackage (rxy_readConfByte (p++));
    }
  }
   
  public:
  void sendEmptyPackage (uint8_t command, uint8_t clientId) {
    startPackage (command, clientId, 0);
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
      // remove previous package    
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
              return;  // do not continue, the other thread should also work
            }
          }
        }
        si++; 
      }
      receiveModified = 0;   
    }
  }
  
};


#endif  //  RemoteXYWire_h