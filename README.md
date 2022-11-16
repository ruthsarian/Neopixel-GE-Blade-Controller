# Neopixel Blade Controller
This is an Arduino IDE sketch for custom lightsaber blades that are compatible with Galaxy's Edge lightsaber hilts. This project was inspired by the [Dead Bothans Society's custom blade project](https://www.deadbothans.com/2019/08/31/switching-to-m0-trinket-and-custom-connectors/) which utilizes a [Trinket M0](https://www.adafruit.com/product/3500) and [instructions for building a custom blade developed by Wedge](https://1drv.ms/w/s!Asy0Vb60mZ1el7hoTX-i5ShwGGqm4Q?e=9xP2ac) on the [SWGE Discord Server](http://swediscord.com).

This code emulates a stock lightsaber blade behavior with the addition of supporting orange and cyan colors if an appropriate crystal is used with a Savi's Workshop hilt. Ignition timings set by this code mirror the timings of stock blades in order to match the audio cues from the hilt. This is why some colors ignite faster than others.

Adruino libraries needed for this sketch:
* [FastLED](https://github.com/FastLED/FastLED)
* [ArduinoLowPower](https://www.arduino.cc/en/Reference/ArduinoLowPower) if using a Trinket M0 or other SAMD-based board.
* [ATTinyCore](https://github.com/SpenceKonde/ATTinyCore) if using a DigiSpark or other ATTiny board. 

Boards known to work with this code are:
* Adafruit Trinket M0
* Arduino Nano
* DigiSpark

Other Arduino-compatible boards will likely work, but will likely require some modifications to this code.
