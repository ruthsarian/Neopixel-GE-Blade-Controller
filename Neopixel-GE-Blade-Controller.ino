/* Galaxy's Edge Lightsaber Compatible NeoPixel Blade Controller : v2.6.IN.LIMBO
 * code by ruthsarian@gmail.com
 *
 * ABOUT
 *  This code is designed to behave as a blade controller for blades containing addressable RGB LEDs (NeoPixels).
 *  It is compatible with Galaxy's Edge lightsaber hilts. 
 *
 *  It was inspired by the Dead Bothans Society's custom blade project which utilizes a Trinket M0
 *    see: https://www.deadbothans.com/2019/08/31/switching-to-m0-trinket-and-custom-connectors/
 *         https://web.archive.org/web/20200922170822/https://deadbothans.com/2019/08/31/switching-to-m0-trinket-and-custom-connectors/
 *    see: https://www.patreon.com/deadbothans
 *    see: https://www.adafruit.com/product/3500
 *
 *  And these instructions for building a custom blade developed by Wedge from the SWGE Discord Server (swgediscord.com)
 *    see: https://1drv.ms/w/s!Asy0Vb60mZ1el7hoTX-i5ShwGGqm4Q?e=9xP2ac
 *
 *  The code has been designed to work with either the FastLED or Adafruit NeoPixel libraries. 
 *  However, FastLED is recommended.
 *
 *  Boards known to work with this code are listed below, but many other boards are likely to also work.
 *   - Adafruit Trinket M0
 *   - Arduino Nano
 *   - ATTiny(8|16)06 (and other tinyAVR MCUs supported by megaTinyCore?)
 *
 * REQUIREMENTS
 *
 *  An Addressable RGB LED Library
 *    FastLED: https://github.com/FastLED/FastLED
 *    Adafruit NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel
 *
 *  Trinket M0 owners using Adafruit NeoPixel should also install Adafruit DotStar
 *    see: https://github.com/adafruit/Adafruit_DotStar
 *
 *  Trinket M0 and other SAMD boards should also install ArduinoLowPower
 *    ArduinoLowPower: https://www.arduino.cc/en/Reference/ArduinoLowPower
 *
 * REFERENCES
 *
 *  TRINKET M0
 *    - Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/SAM-D21DA1-Family-Data-Sheet-DS40001882G.pdf
 *    - Pinout: https://cdn-learn.adafruit.com/assets/assets/000/049/778/original/adafruit_products_Adafruit_Trinket_M0.png
 *    - Schematic: https://cdn-learn.adafruit.com/assets/assets/000/045/723/original/adafruit_products_schem.png
 *    - %LOCALAPPDATA%\Arduino15\packages\adafruit\hardware\samd\1.7.10\cores\arduino\wiring.c
 *
 *  POWER
 *    - https://www.electro-tech-online.com/articles/achieving-low-power-on-adafruit-trinket.830/
 *    - https://forum.arduino.cc/t/mkr-zero-board-sleep-mode/551283/5
 *    - https://forum.arduino.cc/t/question-on-the-arduino-zero-sleep-mode/324112/3
 *    
 *  ATTiny(8|16)06
 *    - Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny806_1606_Data_Sheet_40002029A.pdf
 *    - Pinout Reference: https://github.com/SpenceKonde/megaTinyCore/blob/master/megaavr/extras/ATtiny_x06.gif
 *
 *  CH340 Windows Driver Fix
 *    - https://github.com/SHWotever/FakeCH340DriverFixer
 *
 * NOTES
 *
 *  Power Consumption
 *    - Trinket M0
 *      ; Trinket M0 on its own, in standby, consumes 1.2mA; removing green power LED (or its resistor) only saves about 200uA of current; probably not worth removing
 *      ; Neopixel strip, when off, consumes about 1mA per pixel. this is why a power switch of some sort to the cut power off from the LED strip is necessary
 *    - NeoPixels (WS2812B)
 *      ; consume about 1mA per pixel while off; 91mA w/144 pixels
 *      ; at max brightness (255), 144 pixels consume about 1.3A
 *      ; at lower brightness (64), 144 pixels consume about 500mA
 *
 *  ATTinyX06 / tinyNeoPixel Limitations
 *    Limited memory of the ATTinyX06s cause longer strands of LEDs to not ignite.
 *    
 *    Compile, but don't upload your sketch. If the output window reports less than 60 bytes of
 *    memory available for local variables then you'll need to decrease NUM_LEDs.
 *    
 *    If you notice LEDs are not fully turning off and have a dim, white color to them then the
 *    issue is with timing. Try increasing the clock speed.
 *
 *    1606 @ 10MHz @ 3.3V works for me. 
 *
 *  ATTinyX06 : tinyNeoPixel : tinyNeoPixelStatic
 *        806   58             92
 *       1606   227            264
 *    
 *  ATTinyX06 VQFN20 Pinout Notes
 *    0 = PA4 = PIN 5
 *    2 = PA6 = PIN 7
 *    4 = PB5 = PIN 9
 */

#include "config.h"
#include "hardware.h"
#include "hilt_cmd.h"
#include "blade.h"

// setup() performs one-time initialization steps
void setup() {

  harware_setup();
  cmd_capture_setup();
  blade_setup();
  delay(100);

  // serial output for debug purposes
  #ifdef SERIAL_DEBUG_ENABLE
    Serial.begin(115200);
    delay(1000);
    Serial.println(F("Ready!"));
  #endif
}

// loop() is the main proram loop
void loop() {

  // get commands from the demo
  #ifdef ENABLE_DEMO
    cmd_demo();
  #else
    read_cmd();
  #endif

  // manage the blade
  blade_manager();
}
