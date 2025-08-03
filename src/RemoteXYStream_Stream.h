#ifndef RemoteXYStream_Stream_h
#define RemoteXYStream_Stream_h

//#include <Stream.h>

// This class allows you to connect 
// any communication method that is inherited from the stream class (stream.h)
// Your stream class should define methods:
//   int available ()
//   int read ()
//   write (uint8_t)
// You must initialize and configure the stream yourself

#include "RemoteXYNet.h"


class CRemoteXYStream_Stream : public CRemoteXYStream {
  public:
  Stream * stream;  
  
  public:
  CRemoteXYStream_Stream (Stream * _stream) : CRemoteXYStream () { 
    stream = _stream;
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Init stream"));
#endif
  } 
  
  protected:
  CRemoteXYStream_Stream () : CRemoteXYStream () { 
    // use setStream to set stream
  }           
          
  
  protected: 
  void setStream (Stream * _stream) { 
    stream = _stream;
  }   
  
  
  void handler () override {   
    while (stream->available ()) notifyReadByteListener (stream->read ());
  }
  
  void write (uint8_t byte) override {
    stream->write (byte);
  }

  void flush () override {
    stream->flush ();
  }
  
  
};



#endif //RemoteXYStream_Stream_h