#include <Led8x8xn.h>
#include <ESP8266.h>
#include <Time.h>
byte dataIn[] = {4, 5};
byte load = 2;
byte clk = 3;
int dir = 0;

Led8x8xn leds(dataIn, load, clk);
ESP8266 wifi(Serial);

char HString[128] ;
bool bRotateString = false;
bool bShowTime = false;
bool bShowDate = false;
int interval = 100;
time_t lsecs = 0;

time_t ntpUnixTime (void)
{
  return wifi.ntpTime(4);
}
void dateToHString()
{
  tmElements_t tm;
  breakTime(lsecs, tm);
  sprintf(HString, "%04d-%02d-%02d %02d:%02d ", tm.Year + 1970, tm.Month, tm.Day, tm.Hour, tm.Minute);
}

void timeToHString()
{
  tmElements_t tm;
  breakTime(lsecs, tm);
  sprintf(HString, "%02d:%02d ", tm.Hour, tm.Minute);
}

char SSID[32] = "SSID";
char PASSWORD[16] = "PASSWD";

void setup () {
  wifi.begin();
  wifi.setSoftAPParam("8x8x2Led", "01234567");
  if (wifi.setOprToStationSoftAP()) {
    wifi.joinAP(SSID, PASSWORD);
    wifi.setDHCP(0, 1);
    wifi.enableMUX();
    wifi.startTCPServer(80);
    setSyncInterval(86400);
    setSyncProvider(ntpUnixTime);
    timeToHString();
    bShowTime = true;
  }
  else
    sprintf(HString, "Wifi error");
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
      bShowTime = false;
      bShowDate = false;
    }
    else if (buffer[0] == 'i')
    {
      strcpy(HString, wifi.getLocalIP().c_str());
      int i = 0;
      while (HString[i])
      {
        if (HString[i] == '\r' || HString[i] == '\n')
          HString[i] = ' ';
        i++;
        leds.stringScrollLeft(HString);
        dir = 0;
        bShowTime = false;
        bShowDate = false;
      }
    }
    else if (buffer[0] == 'a')
    {
      strcpy(SSID, (char*)&buffer[1]);
    }
    else if (buffer[0] == 'p')
    {
      bShowTime = false;
      bShowDate = false;
      strcpy(PASSWORD, (char*)&buffer[1]);
      //wifi.stopTCPServer();
      wifi.leaveAP();
      if (wifi.joinAP(SSID, PASSWORD))
      {
        wifi.setDHCP(0, 1);
        wifi.enableMUX();
        wifi.startTCPServer(80);
        sprintf(HString, "Join %s OK", SSID);
      }
      else
        sprintf(HString, "Join %s error", SSID);
    }
    else if (buffer[0] == 't' || buffer[0] == 'd')
    {
      if (buffer[1])
      {
        tmElements_t tm;
        setSyncProvider(NULL);
        int Year, Month, Day, Hour, Minute;
        sscanf((char*)&buffer[1], "%d,%d,%d %d:%d", &Year, &Month, &Day, &Hour, &Minute);
        tm.Year = Year - 1970;
        tm.Month = Month;
        tm.Day = Day;
        tm.Hour = Hour;
        tm.Minute = Minute;
        tm.Second = 0;
        setTime(makeTime(tm));
      }
      else
        setSyncProvider(ntpUnixTime);
      if (buffer[0] == 't')
      {
        bShowTime = true;
        bShowDate = false;
        timeToHString();
      }
      else
      {
        bShowTime = false;
        bShowDate = true;
        dateToHString();
      }
      leds.stringScrollLeft(HString);
    }
    else if (buffer[0] == '3')
    {
      bRotateString = !bRotateString;
      leds.stringScrollLeft(HString, true);
    }
    else if (buffer[0] == '4')
      leds.Mirror = !leds.Mirror;
    else if (buffer[0] == 's')
    {
      interval = constrain(atoi((char*)&buffer[1]), 50, 1000);
    }
    else if (buffer[0] == 'b' && interval < 1000)
    {
      byte brightness = constrain(atoi((char*)&buffer[1]), 0, 15);
      leds.setBrightness(brightness);
    }
  }

  leds.run();
  time_t csec = now();
  if (lsecs != csec)
  {
    lsecs = csec;
    if (bShowTime)
      timeToHString();
    if (bShowDate)
      dateToHString();
  }

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
      leds.stringScrollRight(HString);
  }
}
