#include "Led8x8xn.h"
#include "ESP8266.h"
#include "dht11.h"

byte dataIn[] = {4,5};
byte load = 2;
byte clk = 3;
int dir = 0;
int interval = 100;
Led8x8xn leds(dataIn, load, clk);
char HString[128] = "Hello World.";
bool bRotateString = false;
void setup () {
  Serial.begin(115200);
 leds.stringScrollLeft(HString,bRotateString);
 //leds.Mirror=true;
// leds.stringScrollRight(HString,bRotateString);
 /*
 leds.loadChar('a',0);
 leds.loadChar('b',1); 
 leds.scrollLeft('a','b',3,0);
 leds.scrollLeft('b','c',3,1);
 leds.reflash();
 */
}

void loop () {

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
 
  delay(50);
}
