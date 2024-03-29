# CardAccess

A college assignment over 2 weeks using an Arduino, RFID sensor, C, and lots of coffee.

![](https://lh3.googleusercontent.com/dheWG-jtz96s8_H-5NfJWuO4byxPZctENmMABVEkmteeaPkS4fUjJnkUrXSICSomwZc1mgh5tKNBzbojRHgvYgnMSpekoCpdwM7wd6prLilNVT0BlK8xYOzAcYGVkDDbtvMYT54RMg=w1132-h849-no)

This project consists of a console program written in C, and an Arduino sketch, mostly developed in C
using a few C++ libraries for RFID comms. 

The purpose with this project was to create a working RFID card scanning device which caches cards with approved access locally in SRAM, deployed from the computer running the aforementioned C program via USB over Serial.
The device has the ability to symbolically unlock the door via a single LED light, staying lit for 3 seconds to indicate the unlocked state of the hypothetical door.
The light also serves other purposes, such as flashing quickly when a card is scanned and rapidly when card data is transferred over Serial.

## Components used:

* Arduino Uno
* MFRC522 RFID Sensor
* Unbranded steel case



## The PC software

To download the entire project, see **Releases** in this repository. However, you will likely need to specify a different COM port than the one in the source.c file. Open Source.c in an editor and change this string to whatever port you are connecting your Ardunino with:

```c
short main() {

	CARD_INVENTORY card_inventory_state;
	SERIALPORT port = SerialInit("\\\\.\\COM3");
	...
```



## The Arduino software

Use the Arduino IDE to open the .ino file, and make sure that accesscard.c and accesscard.h are in the same directory as this .ino file. 





Special thanks to **Stefan Holmberg** @ https://github.com/ASPcodenet for the *Serial* library used in this project.

 

