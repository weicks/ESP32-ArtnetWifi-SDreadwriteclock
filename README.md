# ESP32-ArtnetWifi-SDreadwrite-clock
With this project it´s possible to drive Ws2812 Leds with Artnet over Wifi (with Jinx! or Glediator) and it´s also possible to switch to the SD-card version where animations can be recorded from Artnet and than be played back. Although a Matrix-clock can be switched on and off over the animations.

I am a passionate technician and like to build and create works of art. With the great help of Yves Bazin i got this cool work of art real. For this project you will need the latest artnet library from Yves Bazin which you can find here:
https://github.com/hpwit/artnet

I bought a  Wemos MINI D1 ESP32, ten 16x16 Matrixscreens, a powersupply with 5V/40A and a Real Time Clock (RTC DS1307) Data Log Logger Shield for the WeMos WIFI D1 Mini Board (because it fits perfect in the ESP32 without soldering). All this things you get very cheap on ebay or aliexpress.

![image](https://github.com/weicks/ESP32-ArtnetWifi-SDreadwriteclock/blob/master/pics/back.JPG)


The pin assignment is like in the picture below:

![image](https://github.com/weicks/ESP32-ArtnetWifi-SDreadwriteclock/blob/master/pics/panels.JPG)


If you want the wirelesscontroll with your phone, you have to install Blynk and also to create an account. Download the app of your choise (iOS/Android) https://www.blynk.cc/ The setup is very easy! Only a few buttens and 2 sliders as you can see in the pics:

![image](https://github.com/weicks/ESP32-ArtnetWifi-SDreadwriteclock/blob/master/pics/blynk1.JPG)
![image](https://github.com/weicks/ESP32-ArtnetWifi-SDreadwriteclock/blob/master/pics/blynk.JPG)

To get enough space on my ESP32 (D1 Mini) i had to respace the partition.
These are the settings which worked fine with my controller:
in Arduino >> hardware >>espressif>>esp32>>partitions you have a file default.csv make a copy of that file
then copy that instead inside the "default.csv"

#Name,   Type, SubType, Offset,  Size, Flags

nvs,      data,   nvs,    0x9000,   0x5000,

otadata,  data,   ota,    0xe000,   0x2000,

app0,     app,    ota_0,  0x10000,  0x200000,

app1,     app,    ota_1,  0x210000, 0x101000,

eeprom,   data,   0x99,   0x311000, 0x1000,

spiffs,   data,   spiffs, 0x312000, 0xEE000,

//it will reduce the OTA and the SPIFF size and extend the flash size to  3mo when it will compile

then we need to tell Arduino that you have increase that, open the file

arduino>>hardware>>expressif>> board.txt

find you board name for instance I am using "ESP32 Dev module"

esp32.name=ESP32 Dev Module

esp32.upload.tool=esptool

esp32.upload.maximum_size=2097152  <= ,normally you have 1310720

so instead of 1310720 put 2097152

------------------------------------------------------------------------------------------------------
And here it is, the mounted matrixscreen on the wall. Isn´t it beautiful? :)

![image](https://github.com/weicks/ESP32-ArtnetWifi-SDreadwriteclock/blob/master/pics/wall2.jpg)


I am open for every idea or improvements to the code.
If you wana see more of my artobjects you are welcome to visit my homepage: https://kunstkraft.jimdo.com/
