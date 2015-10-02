#include <Led8x8xn.h>
#include <ESP8266.h>
#include <dht11.h>
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
time_t lsecs=0;

time_t ntpUnixTime (void)
{
  time_t time = 0;
  byte buffer[48];
  wifi.registerUDP(4, "pool.ntp.org", 123);

  // Only the first four bytes of an outgoing NTP packet need to be set
  // appropriately, the rest can be whatever.
  const long ntpFirstFourBytes = 0xEC0600E3; // NTP request header
  wifi.send(4,  (byte *)&ntpFirstFourBytes, 48);
  // Wait for response; check every pollIntv ms up to maxPoll times
  const int pollIntv = 150;   // poll every this many ms
  const byte maxPoll = 15;    // poll up to this many times

  for (byte i = 0; i < maxPoll; i++) {
    if (wifi.recv(4, buffer, 48, pollIntv) >= 45)
    {

      for (byte i = 40; i < 44; i++)
        time = time << 8 | buffer[i];

      // Round to the nearest second if we want accuracy
      // The fractionary part is the next byte divided by 256: if it is
      // greater than 500ms we round to the next second; we also account
      // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
      // additionally, we account for how much we delayed reading the packet
      // since its arrival, which we assume on average to be pollIntv/2.
      time += (buffer[44] > (115 - pollIntv / 8));
      time -= 2208988800ul ;   // convert NTP time to Unix time
      time += 8 * 60 * 60; //Taipei Time +8:00
      break;
    }
  }

  wifi.unregisterUDP(4);
  return time;
}
void dateToHString()
{
  tmElements_t tm;
  breakTime(lsecs, tm);
  sprintf(HString, "%04d/%02d/%02d %02d:%02d ", tm.Year+1970, tm.Month, tm.Day, tm.Hour, tm.Minute);
}

void timeToHString()
{
  tmElements_t tm;
  breakTime(lsecs, tm);
  sprintf(HString, "%02d:%02d ", tm.Hour, tm.Minute);
}

char SSID[32] = "DSC_ADSL_TPLINK_ATC";
char PASSWORD[16] = "22988120";

void setup () {
  wifi.begin();
  wifi.setSoftAPParam("8x8x2Led", "01234567");
  if (wifi.setOprToStationSoftAP()) {
    if (wifi.joinAP(SSID, PASSWORD))
    {
      wifi.setDHCP(0, 1);
      wifi.enableMUX();
      wifi.startTCPServer(80);
      setSyncInterval(86400);      
      setSyncProvider(ntpUnixTime);
      timeToHString();
      bShowTime = true;
    }
    else
      sprintf(HString, "Join %s error", SSID);
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
  time_t csec=now();
  if(lsecs!=csec)
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
