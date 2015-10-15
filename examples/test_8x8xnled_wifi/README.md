# Led8x8xn + ESP8266 Demo
Arduino Led8x8xn (Max7219) Display Driver

Demo Video:<br>
  https://www.youtube.com/watch?v=a5YFZLzCmDs<br>
<br>
Need Libraries:<br>
	1. Time : http://www.pjrc.com/teensy/td_libs_Time.html<br>
	2. Hassen's ESP8266 With NTP : https://github.com/HassenLin/Arduino-Wifi-ESP8266<br>
	3. Led8x8xn : https://github.com/HassenLin/Arduino-Led8x8<br>
<br>
Using:<br>
 1. Join AP : "8x8x2Led", password: "01234567"<br>
 2. Connect to IP "192.168.4.1"<br>
 3. Send CMD:<br>
 		1+[string] : Show String.<br>
 		3 : Switch between rotare and scroll string.<br>
 		4 : Toggle mirror mode.<br>
 		i : Show IP.<br>
 		a+[SSID name] : set new SSID. <br>
 		p+[password] : set new password and join new AP.<br>
 		t : Get NTP Time and show time.<br>
 		s+[n] : Scroll speed (20~1000).<br>
 		b+[n] : brightness (0~15).<br>

Max7219 Arduino Library modify form Adruino Playground.<br>
8x8 font :  https://github.com/dhepper/font8x8 <br>
Thanks for dhepper.<br>
