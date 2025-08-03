#ifndef RemoteXY_ArduinoCompat_Stream_h
#define RemoteXY_ArduinoCompat_Stream_h


class Stream {

  public:
  virtual void write (uint8_t b) = 0;
  virtual uint8_t available () = 0; // available for read () = 0 or 1
  virtual uint8_t read () = 0;
  virtual void flush () = 0;

};


#endif // RemoteXY_ArduinoCompat_Stream_h