#include "Led8x8.h"
int dataIn = 2;
int load = 3;
int clk = 4;
int dir = 0;
Led8x8 leds(dataIn, load, clk);


void setup () {

  leds.stringScrollLeft("Hello World.");
}

void loop () {

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
        leds.stringScrollLeft("Hello World.");
        break;
      case 1:
        leds.stringScrollRight();
        break;
      case 2:
        leds.stringScrollUp("Hassen.");
        break;
      case 3:
        leds.stringScrollDown();
        break;
    }
  }

  delay(100);

}
