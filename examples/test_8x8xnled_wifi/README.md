# Led8x8xn + ESP8266 Demo
Arduino Led8x8xn (Max7219) Display Driver

Demo Video:
  https://www.youtube.com/watch?v=a5YFZLzCmDs

Need Libraries:
	1. Time : http://www.pjrc.com/teensy/td_libs_Time.html
	2. Hassen's ESP8266 With NTP : https://github.com/HassenLin/Arduino-Wifi-ESP8266
	3. Led8x8xn : https://github.com/HassenLin/Arduino-Led8x8

Using:
 1. Join AP : "8x8x2Led", password
 2. Connect to IP "192.168.4.1"
 3. Send CMD:
 		1+[string] : Show String.
 		3 : Switch between rotare and scroll string.
 		4 : Toggle mirror mode.
 		i : Show IP.
 		a+[SSID name] : set new SSID. 
 		p+[password] : set new password and join new AP.
 		t : Get NTP Time and show time.
 		s+[n] : Scroll speed (20~1000).
 		b+[n] : brightness (0~15).

Max7219 Arduino Library modify form Adruino Playground.<br>
8x8 font :  https://github.com/dhepper/font8x8 <br>
Thanks for dhepper.<br>
