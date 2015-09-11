#include "Led8x8.h"
#include "ESP8266.h"
#include "dht11.h"
int dataIn = 2;
int load = 3;
int clk = 4;
int dir = 0;
Led8x8 leds(dataIn, load, clk);
ESP8266 wifi(Serial, 9600);
char HString[128] = "Hello World.";
char VString[128] = "Hassen Project.";
void setup () {
  Serial.write("AT+CIOBAUD=57600\r\n");
  Serial.end();
  Serial.begin(57600);
  wifi.setSoftAPParam("ESP8266", "01234567");
  if (wifi.setOprToSoftAP()) {
    wifi.setDHCP(0, 1);
    wifi.enableMUX();
    wifi.startTCPServer(80);
  }
  leds.stringScrollLeft(HString);
}

void loop () {

  uint8_t mux_id;
  uint8_t buffer[128] ;
  uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
  if (len > 0) {
    buffer[len] = 0;
    if (buffer[0] == '1')
    {
      strcpy(HString, (char*)buffer + 1);
      leds.stringScrollLeft(HString);
      dir = 0;
    }
    else
    {
      strcpy(VString, (char*)buffer + 1);
      leds.stringScrollUp(VString);
      dir = 2;
    }
  }
  //  if(Serial.available())
  // {
  //   Serial.read();
  leds.run();
  //  }

  if (leds.Finish)
  {
    dir++;
    if (dir >= 4)
      dir = 0;
    switch (dir)
    {
      case 0:
        leds.stringScrollLeft(HString);
        break;
      case 1:
        leds.stringScrollRight();
        break;
      case 2:
        leds.stringScrollUp(VString);
        break;
      case 3:
        leds.stringScrollDown();
        break;
    }
  }

}
