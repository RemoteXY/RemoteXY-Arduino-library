#ifndef RemoteXYFunc_h
#define RemoteXYFunc_h

#include <inttypes.h> 



// INT

char rxy_intToHexChar (uint8_t c) {
  return c < 10 ? c + '0' : c + 'A' - 10;
}

uint8_t rxy_uint32StrDigits (uint8_t base) {
  uint8_t d = 0;
  while (base) {
    d++;
    base = base >> 1;
  }
  if (d == 0) d = 1;
  return 32 / d;
}

char* rxy_intToFixedStr (uint32_t i, char* s, uint8_t digits, char lead = '0', uint8_t base = 10) {
  uint8_t m = digits;
  if (base < 2) base = 10;
  while (m--) {
    if (i == 0) s[m] = (m == digits-1) ? '0' : lead;
    else {
      s[m] = rxy_intToHexChar (i%base);
      i/=base;
    }
  }
  s[digits]=0;
  return s+digits;
}


char* rxy_intToStr (uint32_t i, char* s, uint8_t base = 10) {   
  uint8_t bufLen = rxy_uint32StrDigits (base) + 1;
  char buf[bufLen];
  rxy_intToFixedStr (i, buf, bufLen-1, 0x20, base);
  char *p = buf;
  while (*p) {
    if (*p != 0x20) *s++ = *p;  
    p++;
  }
  *s = 0;
  return s;  
}

// CHAR

uint8_t rxy_hexCharToInt (char c) {
  uint8_t b;
  if (c<='9') b=c-'0';
  else if (c<='f') b=c-'A'+10; 
  else b=c-'a'+10;
  return b&0x0f;
}

char rxy_toLowerCase (char c) {
  if ((c >= 'a') && (c <= 'z')) c -= 0x20;
  return c;
}

// BUFFERS

// from -> to
void rxy_bufCopy (uint8_t *to, uint8_t *from, uint16_t len) {
  while (len--) *to++ = *from++;
}

void rxy_bufCopy (uint8_t *to1, uint8_t *to2, uint8_t *from, uint16_t len) {
  while (len--) {
    *to1++ = *to2++ = *from++; 
  }
}

uint8_t rxy_bufCompare (uint8_t *p1, uint8_t *p2, uint16_t len) {
  while (len--) {
    if (*p1++ != *p2++) return 0;
  }
  return 1;
}

uint8_t rxy_bufIsEmpty (uint8_t *p, uint16_t len) {
  while (len--) {
    if (*p++ != 0) return 0;
  }
  return 1;
}

void rxy_bufClear (uint8_t *p, uint16_t len) {
  while (len--) *p++ = 0;
}    

int32_t rxy_int32FromBuf (uint8_t *p) {
  int32_t v;
  rxy_bufCopy ((uint8_t*)&v, p, 4);
  return v;
}




// STRINGS


uint16_t rxy_strLength (const char* s) {
  uint16_t len = 0;
  while (*s++) len++; 
  return len; 
}

uint16_t rxy_strLength (const __FlashStringHelper* fs) {
  PGM_P s = reinterpret_cast<PGM_P>(fs);
  uint16_t len = 0;
  while (pgm_read_byte (s++)) len++;
  return len; 
}

char* rxy_strCopy (char* to, const char* from) {
  while (*from) *to++ = *from++; 
  *to = *from;  // zero
  return to;  // pointer to zero
}

char* rxy_strCopy (char* to, const __FlashStringHelper* ffrom) {
  PGM_P from = reinterpret_cast<PGM_P>(ffrom);
  uint8_t c;
  while (1) {
    c = pgm_read_byte (from++); 
    *to++ = c; 
    if (c == 0) break;
  }
  return to-1;  // pointer to zero
}

uint8_t rxy_strCompare (const char* s1, const char* s2, uint8_t unsensitive = 0, uint8_t left = 0) {
  uint8_t c1, c2;
  while (1) {
    c2 = *s2++;
    if ((c2 == 0) && (left)) break;
    c1 = *s1++;
    if (unsensitive) {
      c1 = rxy_toLowerCase (c1);
      c2 = rxy_toLowerCase (c2);
    }
    if (c1 != c2) return 0;
    if (c1 == 0) break;
  }
  return 1;
}

uint8_t rxy_strCompare (const char* s1, const __FlashStringHelper* fs2, uint8_t unsensitive = 0, uint8_t left = 0) {
  PGM_P s2 = reinterpret_cast<PGM_P>(fs2);
  uint8_t c1, c2;
  while (1) {
    c2 = pgm_read_byte (s2++);
    if ((c2 == 0) && (left)) break;
    c1 = *s1++;
    if (unsensitive) {
      c1 = rxy_toLowerCase (c1);
      c2 = rxy_toLowerCase (c2);
    }
    if (c1 != c2) return 0;
    if (c1 == 0) break;
  }
  return 1;
}

// compares only the first characters of string 1 which should match string 2
uint8_t rxy_strCompareLeft (const char* s1, const char* s2, uint8_t unsensitive = 0) {
  return rxy_strCompare (s1, s2, unsensitive, 1);
}

uint8_t rxy_strCompareLeft (const char* s1, const __FlashStringHelper* fs2, uint8_t unsensitive = 0) {
  return rxy_strCompare (s1, fs2, unsensitive, 1);
}


char* rxy_strSearchChar (const char* s, char c, uint8_t unsensitive) {
  char c1;
  if (*s == 0) return NULL;
  if (unsensitive) {
    c = rxy_toLowerCase (c);
  }
  while (1) {    
    c1 = *s;
    if (c1 == 0) break;
    if (unsensitive) {
      c1 = rxy_toLowerCase (c1);
    }
    if (c1 == c) return (char*)s;
    s++;
  }
  return NULL;
}

char* rxy_strSearch (const char* s, const char* needle, uint8_t unsensitive) {
  uint8_t c = *needle;
  if (c == 0) return NULL;
  while (1) {
    s = rxy_strSearchChar (s, c, unsensitive);
    if (s == NULL) break;
    if (rxy_strCompareLeft (s, needle, unsensitive)) return (char*)s;
    s++;
  }
  return NULL;
}

char* rxy_strSearch (const char* s, const __FlashStringHelper * fneedle, uint8_t unsensitive) {
  PGM_P needle = reinterpret_cast<PGM_P>(fneedle);
  uint8_t c = pgm_read_byte (needle);
  if (c == 0) return NULL;
  while (1) {
    s = rxy_strSearchChar (s, c, unsensitive);
    if (s == NULL) break;
    if (rxy_strCompareLeft (s, fneedle, unsensitive)) return (char*)s;
    s++;
  }
  return NULL;
}

uint16_t rxy_strSearchCount (const char* s, const __FlashStringHelper * fneedle, uint8_t unsensitive) {
  uint16_t cnt = 0;
  while (1) {
    s = rxy_strSearch (s, fneedle, unsensitive);
    if (s == NULL) break;
    cnt++;
    s++;
  } 
  return cnt;
}

char* rxy_strParseInt (const char* s, uint16_t * val) {
  *val = 0;
  uint8_t f = 0;
  char c;
  while (*s != 0) {
    c = *s++;
    if ((c == 0x20) && (f == 0)) continue; // left spases
    if ((c >= 0x30) && (c <= 0x39)) {
      *val *= 10;
      *val += c - 0x30;
      f=1;
    }
    else break;
  }
  return (char*)s;
}

uint16_t rxy_strParseInt (const char* s) {
  uint16_t res;
  rxy_strParseInt (s, &res); 
  return res;
}

void rxy_strParseMacAddr (const char* s, uint8_t* m) {
  uint8_t i, b;
  for (i=6; i>0; i--) {
    b=rxy_hexCharToInt (*s++)<<4;
    b|=rxy_hexCharToInt (*s++);
    *m++=b;
    s++;
  }    
}


// CRC
#define REMOTEXY_INIT_CRC 0xffff

void rxy_initCRC (uint16_t *crc) {
  *crc = REMOTEXY_INIT_CRC;
}

void rxy_updateCRC (uint16_t *crc, uint8_t b) {
  *crc ^= b;
  for (uint8_t i=0; i<8; ++i) {
    if ((*crc) & 1) *crc = ((*crc) >> 1) ^ 0xA001;
    else *crc >>= 1;
  }
} 



#endif //RemoteXYFunc_h