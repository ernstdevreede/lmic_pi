# lmic_pi
Raspberry Pi port LoRaMAC in C / LoRaWAN in C http://www.research.ibm.com/labs/zurich/ics/lrsc/lmic.html

This is a port of IBM's LMIC 1.5 to the Raspberry Pi using the wiringPi library for GPIO/SPI.
It is adapted for TTN (http://thethingsnetwork.org).

It has been tested with an HopeRF RFM95W chip, but should also work with a SX1272 or SX1276.

Some of the changes were taken from or inspired by the arduino-lmic-v1.5 port of tftelkamp (https://github.com/tftelkamp/arduino-lmic-v1.5.git) 

The connections of the pins are defined in the main programs in the examples directory.
Standard connections are:
  WiringPi 6  == nss
  
  not connected == rxtx: not used for RFM95
  
  WiringPi 0 == reset (needed for RFM92/RFM95)
  
  WiringPi 7,4,5 == dio0, dio1, dio2
  
  WiringPi 12 == MOSI
  
  WiringPi 13 == MISO
  
  WiringPi 14 == SCK
  
  GND  == GND
  
  3.3V  == +3.3V
  
The only examples currently implemented are hello (which does nothing) and thethingsnetwork-send-v1 which sends test strings to the TTN network (if a gateway is in reach).
Do not forget to put your own device number in thethingsnetwork-send-v1.cpp!!
