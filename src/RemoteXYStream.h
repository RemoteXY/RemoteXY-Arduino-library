#ifndef RemoteXYStream_h
#define RemoteXYStream_h  

#include <inttypes.h> 
#include "RemoteXYDebugLog.h"

#define UNUSED(x) (void)(x)

class CRemoteXYReadByteListener {
  public:
  virtual void readByte (uint8_t byte) = 0;
};

class CRemoteXYOutput {

  public:
  virtual void write (uint8_t byte) = 0; 
  virtual void flush () {};  // send all from output buffer
  
  public:  
  void write (uint8_t * buf, uint16_t size) {
    while (size--) write (*buf++);
  } 
  
  public:
  void write (const char * str) {
    while (*str) write (*str++);
  } 
  
  public:
  void write (const __FlashStringHelper * fstr) {
    PGM_P p = reinterpret_cast<PGM_P>(fstr);
    uint8_t c;
    while (1) {
      c = rxy_pgm_read_byte(p++);
      if (c == 0) break;
      write (c);
    }
  } 

  public:
  void writeP (uint8_t * bgmbuf, uint16_t size) {
    while (size--) write (rxy_pgm_read_byte (bgmbuf++));
  }
  
  public:
  void writeUint16 (uint16_t w) {
    write (w);
    write (w >> 8);
  }

};

class CRemoteXYStream: public CRemoteXYOutput {
  private:
  CRemoteXYReadByteListener * readByteListener;
  
  
  public:
  CRemoteXYStream () {
    readByteListener = NULL;
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
#endif
  }
  
  public:
  void setReadByteListener (CRemoteXYReadByteListener * listener) {
    readByteListener = listener;
  }    
  
  public:
  void notifyReadByteListener (uint8_t byte) {
    if (readByteListener) readByteListener->readByte (byte);
  }   
   
  public:     
  virtual void handler () {};   
  virtual uint8_t connected () {return 1;};  
    

  
  virtual void setBoudRate (uint32_t boudRate) {UNUSED (boudRate);};

      
};





 


#endif //RemoteXYStream_h