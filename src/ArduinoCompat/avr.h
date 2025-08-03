#ifndef RemoteXY_ArduinoCompat_avr_h
#define RemoteXY_ArduinoCompat_avr_h

#if defined (__AVR__)

#include <avr/pgmspace.h>

#if defined(__GNUC__)

void *operator new(size_t size) {
  return malloc(size);
}

void *operator new[](size_t size) {
  return malloc(size);
}

void * operator new(size_t size, void * ptr) noexcept {
  (void)size;
  return ptr;
}

void operator delete(void * ptr) {
  free(ptr);
}

void operator delete[](void * ptr) {
  free(ptr);
}

#endif // __GNUC__

#endif // __AVR__

#endif // RemoteXY_ArduinoCompat_avr_h