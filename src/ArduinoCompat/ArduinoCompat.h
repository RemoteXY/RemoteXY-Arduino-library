#ifndef RemoteXY_ArduinoCompat_h
#define RemoteXY_ArduinoCompat_h


#ifndef ARDUINO

unsigned long millis(void);
void delay(unsigned long ms);

#if defined(__AVR__) 
#include "avr.h"
#endif


class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))

#include "Print.h"
#include "Stream.h"


#endif // ARDUINO




#endif // RemoteXY_ArduinoCompat_h