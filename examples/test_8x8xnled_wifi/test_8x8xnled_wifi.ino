#include "Led8x8xn.h"
#include "ESP8266.h"
#include "dht11.h"
byte dataIn[] = {4,5};
byte load = 2;
byte clk = 3;
int dir = 0;
int interval = 100;
Led8x8xn leds(dataIn, load, clk);
ESP8266 wifi(Serial, 9600);
char HString[128] = "Hello World.";
bool bRotateString = false;
void setup () {
  Serial.write("AT+CIOBAUD=57600\r\n");
  Serial.end();
  Serial.begin(57600);
  wifi.setSoftAPParam("8x8x2Led", "01234567");
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
  uint32_t len = wifi.recv(&mux_id, buffer, sizeof(buffer), interval);
  if (len > 0)
  {
    buffer[len] = 0;
    if (buffer[0] == '1')
    {
      strcpy(HString, (char*)buffer + 1);
      leds.stringScrollLeft(HString);
      dir = 0;
    }
    else if (buffer[0] == '3')
    {
      bRotateString = !bRotateString;
      leds.stringScrollLeft(HString, true);
    }
    else if (buffer[0] == '4')
      leds.Mirror = !leds.Mirror;
    else if (buffer[0] == '+' && interval < 1000)
      interval += 100;
    else if (buffer[0] == '-' && interval > 100)
      interval -= 100;
  }

  leds.run();

  if (!bRotateString && leds.Finish)
  {
    dir++;
    if (dir >= 2)
      dir = 0;

    if (dir == 0 && !HString[0])
      dir = 2;
    if (dir == 0)
      leds.stringScrollLeft(HString);
    if (dir == 1)
      leds.stringScrollRight();
  }
}
