/*
  RemoteXY example. 
  Smartphone connect over Wi-Fi access point from Arduino Nano 33 IoT
  (WiFiNINA module).

  This shows an example of using the library RemoteXY.
  In the example you can control the BUILTIN_LED using the button on the 
  smartphone. The example uses the SPI and WiFiNINA library.
  
  Download the mobile app from the 
  website: http://remotexy.com/download/ for connect this sketch.
  
  Use the website http://remotexy.com/ to create your own management 
  interface your arduino with your smartphone or tablet.
  You can create different management interfaces. Use buttons, 
  switches, sliders, joysticks (g-sensor) all colors and sizes 
  in its interface. Next, you will be able to get the sample 
  code for arduino to use your interface for control from a 
  smartphone or tablet. You will not need to re-install the 
  android app, as it will determine which interface you have 
  downloaded the arduino.
  
*/

///////////////////////////////////////////// 
//        RemoteXY include library         // 
///////////////////////////////////////////// 

/* RemoteXY select connection mode and include library */ 
#include <SPI.h>      // SPI standard library
#include <WiFiNINA.h> // WiFiNINA module library v1.8.7

#define REMOTEXY_MODE__WIFI_POINT   	// data transfer using wifi and open server with access point
//#define REMOTEXY__DEBUGLOGS Serial	// Enable RemoteXY debug message
#include <RemoteXY.h>               	// RemoteXY library v2.4.6 (modified)

/* RemoteXY connection settings */ 
#define REMOTEXY_WIFI_SSID "RemoteXY" 
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

/* RemoteXY configurate  */ 
unsigned char RemoteXY_CONF[] = 
  { 1,0,11,0,1,5,1,0,21,2
  ,59,59,2,88,0 }; 
   
/* this structure defines all the variables of your control interface */ 
struct { 
    /* input variable */
  unsigned char button_1; /* =1 if button pressed, else =0 */

    /* other variable */
  unsigned char connect_flag;  /* =1 if wire connected, else =0 */

} RemoteXY; 

///////////////////////////////////////////// 
//           END RemoteXY include          // 
///////////////////////////////////////////// 


void setup()  
{ 
  RemoteXY_Init ();  
   
  digitalWrite(LED_BUILTIN, LOW);	// init. built-in led off
  pinMode(LED_BUILTIN, OUTPUT);	// conf. built-in led dout
   
  // TODO you setup code 
} 

void loop()  
{  
  RemoteXY_Handler (); 
   
  digitalWrite(LED_BUILTIN, (RemoteXY.button_1==0)?LOW:HIGH);
   
  // TODO you loop code 
  // use the RemoteXY structure for data transfer 
}
