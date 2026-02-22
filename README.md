# Neopixel Blade Controller
This is an Arduino IDE sketch for custom lightsaber blades that are compatible with Galaxy's Edge lightsaber hilts. This project was inspired by the [Dead Bothans Society's custom blade project](https://www.deadbothans.com/2019/08/31/switching-to-m0-trinket-and-custom-connectors/) which utilizes a [Trinket M0](https://www.adafruit.com/product/3500) and [instructions for building a custom blade developed by Wedge](https://1drv.ms/w/s!Asy0Vb60mZ1el7hoTX-i5ShwGGqm4Q?e=9xP2ac) on the [SWGE Discord Server](http://swediscord.com).

This code emulates a stock lightsaber blade behavior with the addition of supporting orange and cyan colors if an appropriate crystal is used with a Savi's Workshop hilt. Ignition timings set by this code mirror the timings of stock blades in order to match the audio cues from the hilt. This is why some colors ignite faster than others.

## Setup
There are several `#define` statements in `config.h`. These define the custom blade setup. Review their values and associated comments and adjust values as needed.

For example, the value of `NUM_LEDS` should match the number of LEDs in your blade. You will likely need to adjust this value. If you're using a single strip of LEDs for both sides of the blade, you'll want to uncommon `#define MIRROR_MODE`. And if you're using a type of LED other than WS2812B in GRB then you'll also want to look at the `_LED_TYPE` defines as well.

## Custom Colors
As of version 1.8, the code supports custom colors. Extinguish and then reignite a blade within 1.5 seconds to switch into color cycle mode. In this mode the blade cycles through colors every 2 seconds. When the blade is on a color you want, extinguish then reignite the blade within 1.5 seconds to lock that color in. The blade will retain that color for as long as it is plugged into the hilt. 

As of version 2.0, while in color cycle mode, trigger a blade clash and you'll be given the option to select a white blade color. If you don't select white by powering the blade off and then on again, it will switch to the next color in the cycle.

## Library and Core Requirements
Adruino libraries needed for this sketch:
* [FastLED](https://github.com/FastLED/FastLED)
* [Adafruit NeoPixel](https://github.com/ruthsarian/Neopixel-GE-Blade-Controller) - an alternative to FastLED
* [ArduinoLowPower](https://www.arduino.cc/en/Reference/ArduinoLowPower) if using a Trinket M0 or other SAMD-based board.
* [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore) if using an AVR-0 series based device.

## Board Compatibility
Boards known to work with this code are:
* Adafruit Trinket M0
* Arduino Nano
* ATtiny806, ATtiny1606

Other Arduino-compatible boards will likely work, but will likely require some modifications to this code.
