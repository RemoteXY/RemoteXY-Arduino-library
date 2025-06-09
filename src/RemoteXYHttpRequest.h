#ifndef RemoteXYType_HttpRequest_h
#define RemoteXYType_HttpRequest_h

//#define REMOTEXY_HTTPREQUEST_USE_HTTP_1_1

#define REMOTEXY_HTTPREQUEST_HEADERS_COUNT 5
#define REMOTEXY_HTTPREQUEST_READ_STR_SIZE 30
#define REMOTEXY_HTTPREQUEST_TIMEOUT 10000 

const char REMOTEXY_HTTPREQUEST_METHOD_GET[] PROGMEM = "GET";
const char REMOTEXY_HTTPREQUEST_METHOD_POST[] PROGMEM = "POST";
const char REMOTEXY_HTTPREQUEST_HOST_REMOTEXY[] PROGMEM = "board.remotexy.com";
#define REMOTEXY_HTTPREQUEST_PORT_REMOTEXY 80                     

#define REMOTEXY_HTTPREQUEST_UNUSED 0
#define REMOTEXY_HTTPREQUEST_USED 1
#define REMOTEXY_HTTPREQUEST_CONNECTING 2
#define REMOTEXY_HTTPREQUEST_READHEADERS 3
#define REMOTEXY_HTTPREQUEST_READCHUNKLEN 4
#define REMOTEXY_HTTPREQUEST_READBODY 5
#define REMOTEXY_HTTPREQUEST_OK 6
#define REMOTEXY_HTTPREQUEST_ERROR 9

// HTTP/1.1 or HTTP/1.0 
#if defined (REMOTEXY_HTTPREQUEST_USE_HTTP_1_1)
const char REMOTEXY_HTTPREQUEST_STR_1[] PROGMEM = " HTTP/1.1\r\nHost: ";  
#else
const char REMOTEXY_HTTPREQUEST_STR_1[] PROGMEM = " HTTP/1.0\r\nHost: ";  
#endif

const char REMOTEXY_HTTPREQUEST_STR_NEXTLINE[] PROGMEM = "\r\n";
const char REMOTEXY_HTTPREQUEST_STR_CONTENT_LENGTH[] PROGMEM = "Content-Length:";
const char REMOTEXY_HTTPREQUEST_STR_REQUIRED_HEADERS[] PROGMEM = "Connection: close\r\nUser-Agent: remotexy.h\r\n";


#pragma pack(push, 1)
class CRemoteXYHttpRequestCompletion {
  public:
  virtual void httpRequestCompletion (uint8_t result, uint16_t code) = 0;
};
  
  

class CRemoteXYHttpRequest: public CRemoteXYReadByteListener {
  public:
  CRemoteXYNet * net;
  
  private:
  CRemoteXYHttpRequestCompletion * completion; 
  CRemoteXYClient * client;
  uint8_t state;

  const __FlashStringHelper * host;
  uint16_t port;
  const __FlashStringHelper * method;
  const __FlashStringHelper * url;
  uint8_t * postData;
  uint16_t postDataLength;
  
  const __FlashStringHelper * headers[REMOTEXY_HTTPREQUEST_HEADERS_COUNT];
  uint8_t headersCount; 
  
  uint32_t timeOut;
  
  char readStrBuf[REMOTEXY_HTTPREQUEST_READ_STR_SIZE];
  uint16_t readStrBufLength;  
  
  uint8_t * answerBuf;
  uint16_t answerBufSize;
  
  uint8_t answerChunked;
  uint16_t answerChunkLength;
  
  public:
  uint16_t answerCode; // 200 is OK
  uint16_t answerContentLength;  // real content length
  uint16_t answerBufLength;      // content length that fits into the buffer
    
  public:
  CRemoteXYHttpRequest (CRemoteXYNet * _net) {
    net = _net;
    client = _net->newClient();
    completion = NULL;
    state = REMOTEXY_HTTPREQUEST_UNUSED;
    answerBufLength = 0;
    headersCount = 0;
  }
  
  public:
  void setCompletion (CRemoteXYHttpRequestCompletion * _completion) {
    completion = _completion;
  }  
  
  public:
  void notifyCompletion (uint8_t _state, uint16_t code) {
    state = _state;
    if (completion) completion->httpRequestCompletion (state == REMOTEXY_HTTPREQUEST_OK, code);
    state = REMOTEXY_HTTPREQUEST_UNUSED;
  }  
  
  
  uint8_t isUnused () {
    if (state == REMOTEXY_HTTPREQUEST_UNUSED) return 1;
    return 0;
  }
  
  void setUsed () {
    state = REMOTEXY_HTTPREQUEST_USED;
  }  
  
  uint16_t getContentLength () {
    return answerContentLength;
  }
 
  void setRequest (const __FlashStringHelper * _host, uint16_t _port, const __FlashStringHelper * _url, const __FlashStringHelper * _method) {
    host = _host;
    port = _port;
    url = _url;
    method = _method;  
    headersCount = 0;
    postDataLength = 0;
  }
    
  void setPostData (const uint8_t * data, uint16_t length) {
    postData = (uint8_t *)data;
    postDataLength = length;   
  }         

  void addHeaderLine (const __FlashStringHelper * headerLine) {
    if (headersCount < REMOTEXY_HTTPREQUEST_HEADERS_COUNT) {
      headers[headersCount++] = headerLine;
    }
  }
  
  void setAnswerBuffer (uint8_t * buf, uint16_t size) {
    answerBuf = buf;
    answerBufSize = size;
  }  
 
  void send () {
    answerBufLength = 0;
    readStrBufLength = 0;
    answerCode = 0;
    readStrBuf[0] = 0;
    answerContentLength = 0;     
    answerChunked = 0;
    answerChunkLength = 0;

    state = REMOTEXY_HTTPREQUEST_CONNECTING; 
    if (client->connect (host, port)) {
      client->setReadByteListener (this);
      timeOut = millis(); 
      return;      
    }

    notifyCompletion (REMOTEXY_HTTPREQUEST_ERROR, 0);
  }
  
  private:
  void sendHeader () {
    char buf[6];
    
    client->write (method);
    client->write (0x20);
    client->write (url);
    client->write (FPSTR(REMOTEXY_HTTPREQUEST_STR_1));
    client->write (host);    
    client->write (FPSTR(REMOTEXY_HTTPREQUEST_STR_NEXTLINE)); 
    client->write (FPSTR(REMOTEXY_HTTPREQUEST_STR_REQUIRED_HEADERS)); 
    
    if (postDataLength > 0) {
      client->write (FPSTR(REMOTEXY_HTTPREQUEST_STR_CONTENT_LENGTH));
      client->write (0x20);
      rxy_intToStr (postDataLength, buf);
      client->write (buf);
      client->write (FPSTR(REMOTEXY_HTTPREQUEST_STR_NEXTLINE)); 
    }
      
    for (uint8_t i = 0; i< headersCount; i++) {
      client->write (headers[i]);
      client->write (FPSTR(REMOTEXY_HTTPREQUEST_STR_NEXTLINE)); 
    }    
    
    client->write (FPSTR(REMOTEXY_HTTPREQUEST_STR_NEXTLINE)); 
    client->flush ();   
  }
 
  private: 
  void sendPostData () {
    if (postDataLength > 0) { 
      client->write (postData, postDataLength);
      client->flush ();
    } 
  }  
  
  public:
  void handler () {
    if ((state == REMOTEXY_HTTPREQUEST_UNUSED) || (state == REMOTEXY_HTTPREQUEST_USED)) return;
    client->handler ();    
    
    if (state == REMOTEXY_HTTPREQUEST_CONNECTING) {
      if (client->connected ()) {
        sendHeader ();
        sendPostData ();
        state = REMOTEXY_HTTPREQUEST_READHEADERS;
      }   
    }    
    
    if ((state >= REMOTEXY_HTTPREQUEST_READHEADERS) || (state <= REMOTEXY_HTTPREQUEST_READBODY)) {
      if (client->connected () == 0) {
        if (answerChunkLength != 0) answerCode = 0;         
        if ((answerCode >= 200) && (answerCode <= 299)) {
          notifyCompletion (REMOTEXY_HTTPREQUEST_OK, answerCode);
        }
        else {
          notifyCompletion (REMOTEXY_HTTPREQUEST_ERROR, 0);
        }
        return;
      }
    }

    // check timout
    if (millis () - timeOut >  REMOTEXY_HTTPREQUEST_TIMEOUT) {
      notifyCompletion (REMOTEXY_HTTPREQUEST_ERROR, 0);
    }
        
  }
  
  public:
  void readByte (uint8_t byte) override { 
    char *s;
#if defined (REMOTEXY_HTTPREQUEST_USE_HTTP_1_1)  // HTTP 1.1 
     
    if ((state == REMOTEXY_HTTPREQUEST_READHEADERS) || (state == REMOTEXY_HTTPREQUEST_READCHUNKLEN)) {
      if (byte == 0x0D) return;
      if (byte == 0x0A) {
        if (readStrBufLength > 0) {  
          readStrBuf[readStrBufLength] = 0;
          if (state == REMOTEXY_HTTPREQUEST_READHEADERS) {
            if (answerCode == 0) {
              if (rxy_strCompareLeft (readStrBuf, F("HTTP/"))) {
                s = rxy_strSearchChar (readStrBuf, 0x20, 0); 
                if (s) answerCode = rxy_strParseInt (s);
              }      
            }
            else {
              s = rxy_strSearchChar (readStrBuf, ':', 0);
              if (s) {
                s++;
                while (*s == 0x20) s++;
                if (rxy_strCompareLeft (readStrBuf, F("Transfer-Encoding:"), 1)) {
                  if (rxy_strCompareLeft (s, F("chunked"), 1)) {
                    answerChunked = 1;
                  }
                  else { // not supported
                    client->stop ();
                    notifyCompletion (REMOTEXY_HTTPREQUEST_ERROR, 0);                
                  }                
                } 
              }               
            }
          }
          
          else {  // REMOTEXY_HTTPREQUEST_READCHUNKLEN
            answerChunkLength = rxy_strParseInt (readStrBuf); 
            state = REMOTEXY_HTTPREQUEST_READBODY; 
          }      
        }
        
        else {  // double enter
          if (answerChunked != 0) state = REMOTEXY_HTTPREQUEST_READCHUNKLEN;   
          else state = REMOTEXY_HTTPREQUEST_READBODY;   
        }
        readStrBufLength = 0;
      } 
      
      else {            
        if (readStrBufLength < REMOTEXY_HTTPREQUEST_READ_STR_SIZE-1) {
          readStrBuf[readStrBufLength++] = (char)byte;
        }
      }
    }        
    
    else if (state == REMOTEXY_HTTPREQUEST_READBODY) {
      
      if ((answerChunkLength > 0) || (answerChunked == 0)) {
        answerContentLength++;
        if (answerBufLength < answerBufSize) {
          answerBuf[answerBufLength++] = byte;
        }
        if (answerChunkLength > 0) {
          answerChunkLength--;
          if (answerChunkLength == 0) state = REMOTEXY_HTTPREQUEST_READCHUNKLEN;
        }
      }
    }
        
#else  // HTTP 1.0

    if (state == REMOTEXY_HTTPREQUEST_READHEADERS) {
      if (byte == 0x0D) return;
      if (byte == 0x0A) {
        if (readStrBufLength > 0) { 
          readStrBuf[readStrBufLength] = 0; 
          if (answerCode == 0) {
            if (rxy_strCompareLeft (readStrBuf, F("HTTP/"))) {
              s = rxy_strSearchChar (readStrBuf, 0x20, 0); 
              if (s) answerCode = rxy_strParseInt (s);
            }      
          }
        }     

        else {  // double enter
          state = REMOTEXY_HTTPREQUEST_READBODY;   
        }
        readStrBufLength = 0;
      } 
      
      else {            
        if (readStrBufLength < REMOTEXY_HTTPREQUEST_READ_STR_SIZE-1) {
          readStrBuf[readStrBufLength++] = (char)byte;
        }
      }
    }        
    
    else if (state == REMOTEXY_HTTPREQUEST_READBODY) {      
      answerContentLength++;
      if (answerBufLength < answerBufSize) {
        answerBuf[answerBufLength++] = byte;
      }
    }

#endif
  }  
     
   
  public:
  static CRemoteXYHttpRequest * getHttpRequest (CRemoteXYNet * nets) {
    CRemoteXYNet * net = nets;
    while (net) {
      if (net->configured ()) {
        if (net->httpRequest == NULL) {
          if (net->hasInternet ()) {
            net->httpRequest = new CRemoteXYHttpRequest (net); 
          }  
        }
        if (net->httpRequest != NULL) {
          net->httpRequest->setUsed ();
          return net->httpRequest;
        }
      }
      net = net->next;
    }    
    return NULL; 
  }  

  
};
#pragma pack(pop)
  
#endif //RemoteXYType_HttpRequest_h