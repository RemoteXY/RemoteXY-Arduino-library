#ifndef RemoteXYStream_ArduinoBLE_h
#define RemoteXYStream_ArduinoBLE_h
                          
#if defined(_ARDUINO_BLE_H_) 

#include "RemoteXYStream.h"
                                                         
#define REMOTEXYCOMM_ARDUINOBLE__SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // NORDIC UART service UUID
#define REMOTEXYCOMM_ARDUINOBLE__RX_CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"    
#define REMOTEXYCOMM_ARDUINOBLE__TX_CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"    
#define REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE 20  
#define REMOTEXYCOMM_ARDUINOBLE__RECEIVE_BUFFER_SIZE 1024

class CRemoteXYStream_ArduinoBLE;
CRemoteXYStream_ArduinoBLE * CRemoteXYStream_ArduinoBLE_instance = NULL;
void CRemoteXYStream_ArduinoBLE_onBLEWritten (BLEDevice central, BLECharacteristic characteristic);


class CRemoteXYStream_ArduinoBLE : public CRemoteXYStream {

  private:
  
  uint8_t failed;
  uint8_t connected;
  
  BLEService * service;
  BLECharacteristic * rxCharacteristic;
  BLECharacteristic * txCharacteristic;
  BLEDescriptor * txDescriptor;
    
  uint8_t sendBuffer[REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE];
  uint16_t sendBufferCount;
  uint16_t sendBytesAvailable;
  
  uint8_t receiveBuffer[REMOTEXYCOMM_ARDUINOBLE__RECEIVE_BUFFER_SIZE];
  uint16_t receiveBufferStart;
  uint16_t receiveBufferPos;
  uint16_t receiveBufferCount;
  
  
  public:
  CRemoteXYStream_ArduinoBLE (const char * _bleDeviceName) : CRemoteXYStream () { 
    CRemoteXYStream_ArduinoBLE_instance = this;

#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init Arduino BLE");
#endif
    connected = 0;

    if (!BLE.begin()) {
      failed = 1;
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write("Starting BLE module failed");
#endif    
      return;
    }
    
    receiveBufferCount = 0;        
    receiveBufferStart = 0;
    receiveBufferPos = 0;

    
    BLE.setLocalName(_bleDeviceName);
    BLE.setDeviceName(_bleDeviceName);
    service = new BLEService(REMOTEXYCOMM_ARDUINOBLE__SERVICE_UUID); 
    BLE.setAdvertisedService(*service);
    rxCharacteristic =  new BLECharacteristic(REMOTEXYCOMM_ARDUINOBLE__RX_CHARACTERISTIC_UUID, BLEWriteWithoutResponse, REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE);   
    txCharacteristic =  new BLECharacteristic(REMOTEXYCOMM_ARDUINOBLE__TX_CHARACTERISTIC_UUID, BLENotify, REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE);   
    uint8_t descriptorValue[2] = {00, 00};
    txDescriptor = new BLEDescriptor ("2902", descriptorValue, sizeof (descriptorValue));
    txCharacteristic->addDescriptor (*txDescriptor);
    
    service->addCharacteristic(*rxCharacteristic);
    service->addCharacteristic(*txCharacteristic);
    
    rxCharacteristic->setEventHandler(BLEWritten, CRemoteXYStream_ArduinoBLE_onBLEWritten);

    BLE.addService(*service);

    rxCharacteristic->writeValue((uint8_t)0);
    txCharacteristic->writeValue((uint8_t)0);
    BLE.advertise();

        
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Arduino BLE started");
#endif  
    failed = 0;

  }          
  
    
  void startWrite (uint16_t len) override {
    if (failed !=0) return;
    sendBytesAvailable = len;
    sendBufferCount = 0;
  }    
    
  void write (uint8_t b) override {
    if (failed !=0) return;
    sendBuffer[sendBufferCount++] = b;
    sendBytesAvailable--;
    if ((sendBufferCount == REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE) || (sendBytesAvailable == 0)) {
      if (connected != 0) {
        txCharacteristic->writeValue(sendBuffer, sendBufferCount);
        BLE.poll();     
      }    
      sendBufferCount = 0;      
    }
  }     

  void handler () override {       
    if (failed !=0) return;
    if (BLE.connected()) {
      if (connected == 0) {
        receiveBufferCount = 0;        
        receiveBufferStart = 0;
        receiveBufferPos = 0;
        connected = 1;
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write("BLE client connected");
#endif          
      }
    }
    else {
      if (connected != 0) {
        connected = 0;
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write("BLE client disconnected");
#endif        
      }
    }
    
    uint8_t b;  
    BLE.poll();
    while (receiveBufferCount > 0) {     
      b =  receiveBuffer[receiveBufferStart++];
      if (receiveBufferStart >= REMOTEXYCOMM_ARDUINOBLE__RECEIVE_BUFFER_SIZE) receiveBufferStart=0;
      receiveBufferCount--;
      notifyReadByteListener (b);
    }
    
    /*
    BLE.poll();
    if (rxCharacteristic->written ()) {
      uint8_t buff[REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE];
      uint8_t cnt = rxCharacteristic->readValue (buff, REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE);    
      for (uint8_t i=0; i<cnt; i++) {     
        notifyReadByteListener (buff[i]);
      }
    }
    */
  } 
  
  public:
  void onBLEWritten () {
    uint8_t buff[REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE];
    uint8_t cnt = rxCharacteristic->readValue (buff, REMOTEXYCOMM_ARDUINOBLE__CHARACTERISTIC_SIZE);    

    for (uint8_t i = 0; i < cnt; i++) {                
      receiveBuffer[receiveBufferPos++] =  buff[i];
      if (receiveBufferPos >= REMOTEXYCOMM_ARDUINOBLE__RECEIVE_BUFFER_SIZE) receiveBufferPos=0; 
      if (receiveBufferCount < REMOTEXYCOMM_ARDUINOBLE__RECEIVE_BUFFER_SIZE) receiveBufferCount++;
      else {
        receiveBufferStart++;
        if (receiveBufferStart >= REMOTEXYCOMM_ARDUINOBLE__RECEIVE_BUFFER_SIZE) receiveBufferStart=0;
      }       
    } 
  }   
};


void CRemoteXYStream_ArduinoBLE_onBLEWritten (BLEDevice central, BLECharacteristic characteristic) {
  if (CRemoteXYStream_ArduinoBLE_instance != NULL) {
    CRemoteXYStream_ArduinoBLE_instance->onBLEWritten ();
  }
}



#endif  // _ARDUINO_BLE_H_

#endif //RemoteXYStream_ArduinoBLE_h