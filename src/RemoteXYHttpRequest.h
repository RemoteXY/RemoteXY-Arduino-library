#ifndef RemoteXYType_HttpRequest_h
#define RemoteXYType_HttpRequest_h


#define REMOTEXY_HTTPREQUEST_HEADERS_COUNT 5
#define REMOTEXY_HTTPREQUEST_READ_STR_SIZE 30
#define REMOTEXY_HTTPREQUEST_TIMEOUT 10000 

const char * REMOTEXY_HTTPREQUEST_METHOD_GET = "GET";
const char * REMOTEXY_HTTPREQUEST_METHOD_POST = "POST";
const char * REMOTEXY_HTTPREQUEST_HOST_REMOTEXY = "remotexy.com";
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
const char * REMOTEXY_HTTPREQUEST_STR_1 = " HTTP/1.1\r\nHost: ";  
const char * REMOTEXY_HTTPREQUEST_STR_NEXTLINE = "\r\n";
const char * REMOTEXY_HTTPREQUEST_STR_CONTENT_LENGTH = "Content-Length:";
const char * REMOTEXY_HTTPREQUEST_STR_REQUIRED_HEADERS = "Connection: close\r\n";

//#include "RemoteXYNet.h" 

//class CRemoteXYRealTime;

class CRemoteXYHttpRequestCompletion {
  public:
  virtual void httpRequestCompletion () = 0;
};
  
  

class CRemoteXYHttpRequest: public CRemoteXYReadByteListener {
  public:
  CRemoteXYNet * net;
  
  private:
  CRemoteXYHttpRequestCompletion * completion; 
  CRemoteXYClient * client;
  uint8_t state;

  char * host;
  uint16_t port;
  char * method;
  char * url;
  uint8_t * postData;
  uint16_t postDataLength;
  
  char * headers[REMOTEXY_HTTPREQUEST_HEADERS_COUNT]; 
  
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
    client = NULL;
    completion = NULL;
    state = REMOTEXY_HTTPREQUEST_UNUSED;
    answerBufLength = 0;
  }
  
  public:
  void setCompletion (CRemoteXYHttpRequestCompletion * _completion) {
    completion = _completion;
  }  
  
  public:
  void notifyCompletion (uint8_t _state) {
    state = _state;
    if (completion) completion->httpRequestCompletion ();
    state = REMOTEXY_HTTPREQUEST_UNUSED;
  }  
  
  uint8_t getState () {
    return state;
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
 
  void setRequest (const char * _host, uint16_t _port, const char * _url, const char * _method) {
    host = (char *)_host;
    port = _port;
    url = (char *)_url;
    method = (char *)_method;  
    for (uint8_t i = 0; i< REMOTEXY_HTTPREQUEST_HEADERS_COUNT; i++) headers[i] = NULL;
    postDataLength = 0;
  }
    
  void setPostData (const uint8_t * data, uint16_t length) {
    postData = (uint8_t *)data;
    postDataLength = length;   
  }         

  void addHeaderLine (const char * headerLine) {
    for (uint8_t i = 0; i< REMOTEXY_HTTPREQUEST_HEADERS_COUNT; i++) {
      if (headers[i] == NULL) {
        headers[i] = (char *)headerLine;
        return;
      } 
    }
  }
  
  void setAnswerBuffer (uint8_t * buf, uint16_t size) {
    answerBuf = buf;
    answerBufSize = size;
  }  
 
  void send () {
    if (client == NULL) {
      client = net->newClient();
    }
    if (client == NULL) {
      notifyCompletion (REMOTEXY_HTTPREQUEST_ERROR);
      return;
    }
    client->setReadByteListener (this);

    answerBufLength = 0;
    readStrBufLength = 0;
    answerCode = 0;
    readStrBuf[0] = 0;
    answerContentLength = 0;
    answerChunked = 0;
    answerChunkLength = 0;
    
    client->stop ();
    client->connect (host, port);
    timeOut = millis(); 
    state = REMOTEXY_HTTPREQUEST_CONNECTING; 
  }
  
  private:
  void sendHeader () {
    char contentLength[22];
    uint16_t crLen = 2;
    uint16_t contentLengthLen = 0;
    uint16_t methodLen = rxy_strLength (method);
    uint16_t urlLen = rxy_strLength (url);
    uint16_t h1Len = rxy_strLength (REMOTEXY_HTTPREQUEST_STR_1);
    uint16_t hostLen = rxy_strLength (host);
    uint16_t requiredHeadersLen = rxy_strLength (REMOTEXY_HTTPREQUEST_STR_REQUIRED_HEADERS);
    
    if (postDataLength > 0) {
      char * p = rxy_strCopy (contentLength, REMOTEXY_HTTPREQUEST_STR_CONTENT_LENGTH);
      *p++ = 0x20;
      p = rxy_intToStr (postDataLength, p); 
      contentLengthLen = rxy_strLength (contentLength) + crLen;
    }
            
    uint16_t headerSize = methodLen + 1 + urlLen + h1Len + hostLen + crLen + requiredHeadersLen + contentLengthLen + crLen;

    for (uint8_t i = 0; i< REMOTEXY_HTTPREQUEST_HEADERS_COUNT; i++) {
      if (headers[i] != NULL) headerSize += rxy_strLength (headers[i]) + crLen;
    }    
    
    client->startWrite (headerSize); 
    client->writeBuf ((uint8_t*)method, methodLen);
    client->write (0x20);
    client->writeBuf ((uint8_t*)url, urlLen);
    client->writeBuf ((uint8_t*)REMOTEXY_HTTPREQUEST_STR_1, h1Len);
    client->writeBuf ((uint8_t*)host, hostLen);    
    client->writeBuf ((uint8_t*)REMOTEXY_HTTPREQUEST_STR_NEXTLINE, crLen); 
    client->writeBuf ((uint8_t*)REMOTEXY_HTTPREQUEST_STR_REQUIRED_HEADERS, requiredHeadersLen); 
    
    if (contentLengthLen > 0) {
      client->writeBuf ((uint8_t*)contentLength, contentLengthLen);
      client->writeBuf ((uint8_t*)REMOTEXY_HTTPREQUEST_STR_NEXTLINE, crLen); 
    }
      
    for (uint8_t i = 0; i< REMOTEXY_HTTPREQUEST_HEADERS_COUNT; i++) {
      if (headers[i] != NULL) {
        client->writeBuf ((uint8_t*)headers[i], rxy_strLength (headers[i]));
        client->writeBuf ((uint8_t*)REMOTEXY_HTTPREQUEST_STR_NEXTLINE, crLen); 
      }
    }    
    
    client->writeBuf ((uint8_t*)REMOTEXY_HTTPREQUEST_STR_NEXTLINE, crLen);    
  }
 
  private: 
  void sendPostData () {
    if (postDataLength > 0) {
      client->startWrite (postDataLength);   
      client->writeBuf (postData, postDataLength);
    } 
  }  
  
  public:
  void handler () {
    if ((state == REMOTEXY_HTTPREQUEST_UNUSED) || (state == REMOTEXY_HTTPREQUEST_USED)) return;
    if (client == NULL) return;
    
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
        client->stop ();
        if (((answerCode >= 200) || (answerCode <= 299)) && (answerChunkLength == 0)) {
          notifyCompletion (REMOTEXY_HTTPREQUEST_OK);
        }
        else {
          notifyCompletion (REMOTEXY_HTTPREQUEST_ERROR);
        }
        return;
      }
    }

    // check timout
    if (millis () - timeOut >  REMOTEXY_HTTPREQUEST_TIMEOUT) {
      client->stop ();
      notifyCompletion (REMOTEXY_HTTPREQUEST_ERROR);
    }
        
  }
  
  public:
  void readByte (uint8_t byte) override { 
    if ((state == REMOTEXY_HTTPREQUEST_READHEADERS) || (state == REMOTEXY_HTTPREQUEST_READCHUNKLEN)) {
      if (byte == 0x0D) return;
      if (byte == 0x0A) {
        if (readStrBufLength > 0) {  
          if (state == REMOTEXY_HTTPREQUEST_READHEADERS) {
            char *s = rxy_strSearchChar (readStrBuf, 0x20, 0);            
            if (s != NULL) {
              while (*s == 0x20) s++;
              if (rxy_strCompareLeft (readStrBuf, "HTTP/", 0)) {
                rxy_strParseInt (s, &answerCode);
              }        
              else if (rxy_strCompareLeft (readStrBuf, "Transfer-Encoding:", 1)) {
                if (rxy_strCompareLeft (s, "chunked", 1)) {
                  answerChunked = 1;
                }
                else { // not supported
                  client->stop ();
                  notifyCompletion (REMOTEXY_HTTPREQUEST_ERROR);                
                }
              }
            }  
          }
          else {  // REMOTEXY_HTTPREQUEST_READCHUNKLEN
            rxy_strParseInt (readStrBuf, &answerChunkLength); 
            state = REMOTEXY_HTTPREQUEST_READBODY; 
          }      
        }
        else {  // double enter
          if (answerChunked != 0) state = REMOTEXY_HTTPREQUEST_READCHUNKLEN;   
          else state = REMOTEXY_HTTPREQUEST_READBODY;   
        }
        readStrBufLength = 0;
        readStrBuf[0] = 0;
      } 
      else {            
        if (readStrBufLength < REMOTEXY_HTTPREQUEST_READ_STR_SIZE-1) {
          readStrBuf[readStrBufLength++] = (char)byte;
          readStrBuf[readStrBufLength] = 0;
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

  
#endif //RemoteXYType_HttpRequest_h