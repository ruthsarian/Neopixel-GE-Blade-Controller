/* Galaxy's Edge Lightsaber Compatible Neopixel Blade Controller (for Trinket M0) : v1.7
 * code by ruthsarian@gmail.com
 *
 * REQUIREMENTS
 *
 *  Libraries required for all boards:
 *    FastLED: https://github.com/FastLED/FastLED
 *
 *  Libraries required for Trinket M0 and other SAMD boards
 *    ArduinoLowPower: https://www.arduino.cc/en/Reference/ArduinoLowPower
 *
 *  If using a DigiSpark or other ATTiny boards, use the ATTinyCore core
 *    ATTinyCore: https://github.com/SpenceKonde/ATTinyCore
 *    Will need to upload a new bootloader with 8MHz clock selected; upgrade will not work, must be via ISP
 *    https://github.com/micronucleus/micronucleus
 *
 *
 * ABOUT
 * 
 *  this project was inspired by the Dead Bothans Society's custom blade project which utilizes a Trinket M0
 *    see: https://www.deadbothans.com/2019/08/31/switching-to-m0-trinket-and-custom-connectors/
 *    see: https://www.patreon.com/deadbothans
 *    see: https://www.adafruit.com/product/3500
 * 
 *  and instructions for building a custom blade developed by Wedge on the SWGE Discord Server (swgediscord.com)
 *    see: https://1drv.ms/w/s!Asy0Vb60mZ1el7hoTX-i5ShwGGqm4Q?e=9xP2ac
 * 
 *  boards known to work with this code are listed below; other Arduino-compatible boards will likely also work
 *   - Adafruit Trinket M0
 *   - Arduino Nano
 *   - DigiSpark
 *
 *
 * DIGISPARK aka TINY85 aka ATTINY85 DEV BOARD
 *
 *  Using a DigiSpark
 *    - The DigiSpark (and its many clones) is an ATTiny85-based board that is small and inexpensive
 *    - The Arduino core ATTinyCore will need to be installed in order to program the DigiSpark
 *      see: https://github.com/SpenceKonde/ATTinyCore
 *    - Due to the DigiSpark's bootloader, it takes too long for it to come out of sleep to catch the first ignite command.
 *      As a result a define is set (DO_NOT_SLEEP_PWR_DOWN) if an ATTiny85 is detected in order to keep the ATTiny85 out of
 *      power down sleep mode. This causes it to consume 2mA more than it would in power down mode.
 *    - If you wish to put the ATTiny85 into power down sleep mode, comment out the DO_NOT_SLEEP_PWR_DOWN define, but be
 *      aware that ignite commands will be missed. UNLESS you program your ATTiny85 without a bootloader and flash 
 *      appriate fuse values.
 *
 *  Programming the DigiSpark without a Bootloader
 *    0. An ISP programmer is needed. It's possible to program another Arduino with the Arduino as ISP sketch and use it as an ISP programmer.
 *       see: https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP/
 *    1. With ATTinyCore installed and an ISP programmer, select the appropriate board from the TOOLS menu
 *       -> Tools -> Board -> ATTinyCore -> ATTiny24/45/85 (No Bootloader)
 *    2. Under the Tools menu select the appropriate clock for the board
 *       -> Tools -> Clock -> 8 MHz (No USB)
 *    3. Attach your DigiSpark to the ISP programmer. If using an Arduino as ISP connect like this:
 *        (ISP) GND -> (DigiSpark) GND
 *        (ISP) 5V  -> (DigiSpark) 5V
 *        (ISP) D10 -> (DigiSpark) P5
 *        (ISP) D11 -> (DigiSpark) P0
 *        (ISP) D12 -> (DigiSpark) P1
 *        (ISP) D13 -> (DigiSpark) P2
 *    5. Plug in your ISP programmer (if you haven't already) and select the COM port it is attached to
 *       -> Tools -> Port -> COMx
 *    6. Select the programmer
 *       -> Tools -> Programmer -> Arduino as ISP
 *    7. Upload the sketch as you normally would
 *    8. Update the fuses on the DigiSpark to reflect the correct clock and start-up using AVRDUDE, a command-line utility bundled with the Arduino IDE
 *       -> avrdude -c stk500v1 -P COM12 -b 19200 -p attiny85 -v -U lfuse:w:0xc2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -B 20
 *
 *  Where is AVRDUDE?
 *    1. Enable verbose output for upload within the Arduino IDE
 *       -> File -> Preferences -> Show verbose output during: -> upload (check)
 *    2. Try to upload a sketch. Scroll through output area (by default at the bottom of the Arduino IDE). 
 *       You will see references to AVRDUDE in there which should include the full path to AVRDUDE.
 *    3. As an example, on a Windows machine AVRDUDE might be installed here:
 *       %LOCALAPPDATA%\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino18\bin
 *
 *
 * ISSUES
 * 
 *  - sometimes the hilt does not switch on even if the switch is set to the on position. not currently certain what is causing this. 
 *    i suspect it's my testing setup, but i've noticed some hilts seem unaffected by this issue.
 *    some time is needed to investigate this further. 
 * 
 * 
 * NOTES
 *  
 *  Power Consumption
 *    - Trinket M0
 *      ; Trinket M0 on its own, in standby, consumes 1.2mA; removing green power LED (or its resistor) only saves about 200uA of current; probably not worth removing
 *      ; Neopixel strip, when off, consumes about 1mA per pixel. this is why a power switch of some sort to the cut power off from the LED strip is necessary
 *    - DigiSpark
 *      ; DigiSpark consumes 7mA in sleep due to onboard LED and voltage regulator
 *      ; Without power LED, 4mA; and without 78L05 regulator 1.18mA (comparable to un-modified trinket m0)
 *      ; if 78L05 is removed attach hilt power to '5V' pin not 'VIN' pin. MCU will operate at lower (hilt battery) voltage, but should still work
 *      ; Bootloader's presence on DigiSpark does not impact current draw
 *    - NeoPixels (WS2812B)
 *      ; consume about 1mA per pixel while off; 91mA w/144 pixels
 *      ; at max brightness (255), 144 pixels consume about 1.3A
 *      ; at my brightness (64), 144 pixels consume about 500mA
 *
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
 *  DIGISPARK
 *    - ATTiny85 Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf
 *    - Schematic: https://s3.amazonaws.com/digistump-resources/files/97a1bb28_DigisparkSchematic.pdf
 *    - Pinout: https://static.ericdraken.com/files/attiny85-pinout.png
 *    - Fuse Calculator: https://wwe2w.engbedded.com/fusecalc/
 *    - Fuse Resetting: https://ericdraken.com/hvsp/
 *
 *  MICRONUCLEUS BOOTLOADER
 *    - https://github.com/micronucleus/micronucleus
 *    - Upgrading Micronucleus: https://koen.vervloesem.eu/blog/how-to-upgrade-the-micronucleus-bootloader-on-the-digispark/
 *    - MSYS2: https://www.msys2.org/
 *      ; pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-libusb-win32 mingw-w64-x86_64-avr-toolchain mingw-w64-x86_64-avrdude git
 *
 *  AVRDUDE
 *    Write Firmware: avrdude -c stk500v1 -P COM10 -b 19200 -p attiny85 -v -U flash:w:t85_default.hex
 *    Read Fuses:     avrdude -c stk500v1 -P COM10 -b 19200 -p attiny85 -v -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i
 *    Write Fuses:    avrdude -c stk500v1 -P COM12 -b 19200 -p attiny85 -v -U lfuse:w:0xc2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -B 20
 *
 */

#define NUM_LEDS            13    // number of LEDs in the strip
#define MAX_BRIGHTNESS      64    // default brightness; lower value = lower current draw
#define HILT_DATA_PIN       2     // digital pin the hilt's data line is connected to
#define LED_DATA_PIN        4     // digital pin the LED strip is attached to
#define LED_PWR_SWITCH_PIN  0     // this pin is held low until the blade turns on, at which point it will be pushed high
                                  // this can be used to control a switch to connect and disconnect a battery switch; see: https://www.pololu.com/product/2811
                                  // 
                                  // if not using a switch, comment out this define
//#define MIRROR_MODE             // uncomment to enable mirror mode
                                  // mirror mode treats the strip of LEDs as a single strip, folded in half, to create the blade
                                  // as such, when igniting both the first and last LEDs in the strip will turn on and the next in turn
                                  // until the last LED to turn on is the one in the middle of the strip
#define DOTSTAR_DATPIN      7     // data pin for Trinket M0's onboard DOTSTAR
#define DOTSTAR_CLKPIN      8     // clock pin for Trinket M0's onboard DOTSTAR
#define SLEEP_AFTER         60000 // how many ms to wait, after turning off, before going to sleep to conserve power
                                  // sleep will also stop the COM port of your microcontroller from appearing on your computer
                                  // set this as a large value while doing development, then lower it to 60000 or less for a
                                  // 'production' environment.
#define SERIAL_DEBUG_ENABLE       // enable debug messages over serial
#define VALID_BIT_CUTOFF    4000  // any HIGH period on the data line longer than this value, in microseconds, is considered an invalid bit of data and causes a reset of the data capture
#define VALID_BIT_ONE       1600  // any HIGH period longer than this value, in microseconds, but less than VALID_BIT_CUTOFF is treated as a valid 1 bit
                                  // any HIGH period shorter than this value, in microseconds, is treated as a valid 0 bit
                                  // if blade is not registering commands correctly, this value likely needs to be tweaked
                                  // typically a 1 bit is about 2000uS (Legacy) or 2400uS (Savi) long and a 0 is 1200uS long. 
                                  // this value is set between 2000 and 1200 to accomadate delays in timing/processing

// it appears a bug requires FASTLED_FORCE_SOFTWARE_PINS be defined for SAMD devices (like the Trinket M0)
// in the future this may not be necessary, but for now it's left in
#ifdef ARDUINO_ARCH_SAMD
  #define FASTLED_FORCE_SOFTWARE_PINS   // see https://github.com/FastLED/FastLED/issues/1363; https://github.com/FastLED/FastLED/issues/1354
#endif
#include <FastLED.h>

// power consumption considerations
#ifdef ARDUINO_ARCH_AVR
  #include <avr/sleep.h>
  #include <avr/power.h>
#elif defined ARDUINO_ARCH_SAMD
  #include "ArduinoLowPower.h"    // see https://www.arduino.cc/en/Reference/ArduinoLowPower
  #define VERY_LOW_POWER          // for the Arduino SAMD core; helps reduce power consumption
#endif

// program space considerations
#ifdef ARDUINO_AVR_ATTINYX5

  // if defined, ATTiny (digispark) will not use the power down sleep mode
  // this will cause it to consume an additional 2mA in sleep mode, however it will not miss the 
  // ignite command when waking from sleep
  //
  // if trying to minimize power consumption, comment this out
  #define DO_NOT_SLEEP_PWR_DOWN

  // defining SPACE_SAVER will enable some space-saving steps within the code to help it fit into smaller program space areas
  #define SPACE_SAVER
#endif

// help save program space
#ifdef SPACE_SAVER

  // remove all uses of Serial() as it requires a good chunk of program space
  #undef SERIAL_DEBUG_ENABLE

  // store lightsaber blade timings as 8-bit values instead of 16-bit values
  #define TIME_ENCODE(t)  ( uint8_t)((t & 0x3FF) >> 2)
  #define TIME_DECODE(t)  (uint16_t)(t << 2)
  typedef uint8_t blade_timing_t;
#else
  #define TIME_ENCODE(t)  t
  #define TIME_DECODE(t)  t
  typedef uint16_t blade_timing_t;
#endif

// stock blade colors; these values are based on PWM duty cycles measured from a stock blade controller
#define CRGB_BLADE_WHITE         CRGB(102, 102, 102)
#define CRGB_BLADE_RED           CRGB(255,   0,   0)
#define CRGB_BLADE_ORANGE        CRGB(231,  77,   0)
#define CRGB_BLADE_YELLOW        CRGB(154, 154,   0)
#define CRGB_BLADE_GREEN         CRGB(  0, 255,   0)
#define CRGB_BLADE_CYAN          CRGB(  0, 154, 154)
#define CRGB_BLADE_BLUE          CRGB(  0,   0, 255)
#define CRGB_BLADE_PURPLE        CRGB(154,   0, 154)
#define CRGB_BLADE_DARK_PURPLE   CRGB( 26,   0,  13)
#define CRGB_BLADE_CLASH_YELLOW  CRGB(255, 255,   0)
#define CRGB_BLADE_CLASH_ORANGE  CRGB(255,  64,   0)
#define CRGB_BLADE_CLASH_WHITE   CRGB(128, 128, 128)
#define CRGB_BLADE_OFF           CRGB(  0,   0,   0)

// how many lightsabers per lightsaber table
#define LIGHTSABER_TABLE_LEN    16

// color table index
#define INDEX_BLADE_WHITE       0
#define INDEX_BLADE_RED         1
#define INDEX_BLADE_ORANGE      2
#define INDEX_BLADE_YELLOW      3
#define INDEX_BLADE_GREEN       4
#define INDEX_BLADE_CYAN        5
#define INDEX_BLADE_BLUE        6
#define INDEX_BLADE_PURPLE      7
#define INDEX_BLADE_DARK_PURPLE 8
#define INDEX_COLOR_TABLE_COLOR 0
#define INDEX_COLOR_TABLE_CLASH 1

// all possible stock blade colors and their corresponding clash color
CRGB color_table[][2] = {
  {CRGB_BLADE_WHITE,        CRGB_BLADE_CLASH_YELLOW},
  {CRGB_BLADE_RED,          CRGB_BLADE_CLASH_ORANGE},
  {CRGB_BLADE_ORANGE,       CRGB_BLADE_CLASH_WHITE},
  {CRGB_BLADE_YELLOW,       CRGB_BLADE_CLASH_WHITE},
  {CRGB_BLADE_GREEN,        CRGB_BLADE_CLASH_YELLOW},
  {CRGB_BLADE_CYAN,         CRGB_BLADE_CLASH_YELLOW},
  {CRGB_BLADE_BLUE,         CRGB_BLADE_CLASH_YELLOW},
  {CRGB_BLADE_PURPLE,       CRGB_BLADE_CLASH_YELLOW},
  {CRGB_BLADE_DARK_PURPLE,  CRGB_BLADE_CLASH_ORANGE}
};

// stock lightsaber properties template
typedef struct {
  CRGB *color;
  blade_timing_t ignition_time;
  blade_timing_t extinguish_time_delay;
  blade_timing_t extinguish_time;
} stock_lightsaber_t;

// savi's workshop lightsaber properties
// timing values were taken from logic analyzer captures of a stock V2 blade controller
stock_lightsaber_t savi_lightsaber[LIGHTSABER_TABLE_LEN] = {
//  COLOR,                                IGNITION_TIME,    EXTINGUISH_TIME_DELAY,  EXTINGUISH_TIME
  { color_table[INDEX_BLADE_WHITE],       TIME_ENCODE(280), TIME_ENCODE(370),       TIME_ENCODE(470)}, //  white kyber crystal
  { color_table[INDEX_BLADE_RED],         TIME_ENCODE(280), TIME_ENCODE(940),       TIME_ENCODE(470)}, //    red kyber crystal
  { color_table[INDEX_BLADE_ORANGE],      TIME_ENCODE(280), TIME_ENCODE(000),       TIME_ENCODE(275)}, // orange kyber crystal
  { color_table[INDEX_BLADE_YELLOW],      TIME_ENCODE(280), TIME_ENCODE(000),       TIME_ENCODE(275)}, // yellow kyber crystal
  { color_table[INDEX_BLADE_GREEN],       TIME_ENCODE(280), TIME_ENCODE(180),       TIME_ENCODE(470)}, //  green kyber crystal
  { color_table[INDEX_BLADE_CYAN],        TIME_ENCODE(280), TIME_ENCODE(275),       TIME_ENCODE(565)}, //   cyan kyber crystal
  { color_table[INDEX_BLADE_BLUE],        TIME_ENCODE(280), TIME_ENCODE(275),       TIME_ENCODE(565)}, //   blue kyber crystal
  { color_table[INDEX_BLADE_PURPLE],      TIME_ENCODE(280), TIME_ENCODE(370),       TIME_ENCODE(660)}, // purple kyber crystal
  { color_table[INDEX_BLADE_DARK_PURPLE], TIME_ENCODE(280), TIME_ENCODE(945),       TIME_ENCODE(470)}, //
  { color_table[INDEX_BLADE_RED],         TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { color_table[INDEX_BLADE_RED],         TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { color_table[INDEX_BLADE_YELLOW],      TIME_ENCODE(240), TIME_ENCODE(000),       TIME_ENCODE(240)}, //
  { color_table[INDEX_BLADE_GREEN],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, //
  { color_table[INDEX_BLADE_RED],         TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { color_table[INDEX_BLADE_BLUE],        TIME_ENCODE(240), TIME_ENCODE(245),       TIME_ENCODE(500)}, //
  { color_table[INDEX_BLADE_PURPLE],      TIME_ENCODE(240), TIME_ENCODE(330),       TIME_ENCODE(545)}  //
};

// legacy lightsaber properties
// timing values were taken from logic analyzer captures of a stock V2 blade controller
stock_lightsaber_t legacy_lightsaber[LIGHTSABER_TABLE_LEN] = {
//  COLOR,                                IGNITION_TIME,    EXTINGUISH_TIME_DELAY,  EXTINGUISH_TIME
  { color_table[INDEX_BLADE_YELLOW],      TIME_ENCODE(240), TIME_ENCODE(000),       TIME_ENCODE(325)}, // Temple Guard
  { color_table[INDEX_BLADE_RED],         TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(335)}, // Kylo Ren
  { color_table[INDEX_BLADE_BLUE],        TIME_ENCODE(240), TIME_ENCODE(500),       TIME_ENCODE(415)}, // Rey (Anakin), Rey Reforged, Ahsoka (CW)
  { color_table[INDEX_BLADE_PURPLE],      TIME_ENCODE(240), TIME_ENCODE(490),       TIME_ENCODE(585)}, // Mace Windu
  { color_table[INDEX_BLADE_RED],         TIME_ENCODE(240), TIME_ENCODE(660),       TIME_ENCODE(335)}, // Asajj Ventress
  { color_table[INDEX_BLADE_WHITE],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, // Ahsoka (Rebels)
  { color_table[INDEX_BLADE_GREEN],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, // Luke
  { color_table[INDEX_BLADE_RED],         TIME_ENCODE(240), TIME_ENCODE(500),       TIME_ENCODE(500)}, // Darth Vader
  { color_table[INDEX_BLADE_RED],         TIME_ENCODE(240), TIME_ENCODE(750),       TIME_ENCODE(330)}, // Darth Maul
  { color_table[INDEX_BLADE_BLUE],        TIME_ENCODE(240), TIME_ENCODE(495),       TIME_ENCODE(415)}, // Obi-Wan, Ben Solo
  { color_table[INDEX_BLADE_WHITE],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { color_table[INDEX_BLADE_WHITE],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { color_table[INDEX_BLADE_WHITE],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { color_table[INDEX_BLADE_WHITE],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { color_table[INDEX_BLADE_WHITE],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { color_table[INDEX_BLADE_WHITE],       TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}
};

// blade states
typedef enum {
  BLADE_UNINITIALIZED,
  BLADE_OFF,
  BLADE_IGNITING,
  BLADE_ON,
  BLADE_IDLE,
  BLADE_CLASH,
  BLADE_EXTINGUISHING,
  BLADE_REFRESH,
  BLADE_FLICKER_LOW,
  BLADE_FLICKER_HIGH
} state_t;

// blade properties template
typedef struct {
  state_t state;
  uint8_t cmd;
  stock_lightsaber_t *lightsaber;
} blade_t;

// global blade properties object
blade_t blade;

// led arrays used by FastLED
#ifdef ADAFRUIT_TRINKET_M0
  CRGB dotstar;                 // Trinket M0 dotstar LED
#endif
CRGB leds[NUM_LEDS];            // blade LEDs

// global variable where decoded hilt command is stored
// it has the volatile keyword because it will be updated from within an interrupt service routine
volatile uint8_t hilt_cmd = 0;

// blade_manager() takes care of changing the colors of the blade
void blade_manager() {
  static uint32_t next_step = 0;
  static uint32_t animate_step = 0;
  static state_t last_state = BLADE_UNINITIALIZED;

  // if there's a change of blade state there may be a need to initiate some animation effect
  if (last_state != blade.state) {
    last_state = blade.state;
    next_step = 0;
    animate_step = 0;

    switch (blade.state) {

      // the blade is off. disable any running animations and shut the LEDs off
      case BLADE_OFF:
        next_step = millis() + SLEEP_AFTER;   // sleep timer
        FastLED.clear();

        // disconnect power to the LEDs
        #ifdef LED_PWR_SWITCH_PIN
          digitalWrite(LED_PWR_SWITCH_PIN, LOW);
        #endif
        break;

      // the blade is powering on
      case BLADE_IGNITING:

        // connect LED battery power
        #ifdef LED_PWR_SWITCH_PIN
          digitalWrite(LED_PWR_SWITCH_PIN, HIGH);
        #endif

        // clear the LEDs immediately after they are powered on via LED_PWR_SWITCH_PIN
        FastLED.clear();

        // set the brightness for the blade
        FastLED.setBrightness(MAX_BRIGHTNESS);

        // delay ignition based on whatever value is stored in the lightsaber's properties
        next_step = millis();
        break;

      // BLADE_ON is when the blade has just finished igniting; perhaps there's something we'll want to do only 
      // under that situation, which is why BLADE_ON and BLADE_IDLE are separate things
      case BLADE_ON:
        blade.state = BLADE_IDLE;
        break;

      // blade is at idle, do nothing      
      case BLADE_IDLE:
        break;

      // a clash command has been sent; this happens when the blade hits something or the hilt stops suddenly
      case BLADE_CLASH:

        // immediately set the blade to the clash color
        fill_solid(leds, NUM_LEDS, blade.lightsaber->color[INDEX_COLOR_TABLE_CLASH]);

        // wait 40 milliseconds and then change the blade color back to normal
        next_step = millis() + 40;
        break;

      // the blade is turning off
      case BLADE_EXTINGUISHING:

        // different lightsabers have different delays before the extinguish begins, so this statement sets that delay
        next_step = millis() + TIME_DECODE(blade.lightsaber->extinguish_time_delay);
        break;

      // every second or so the hilt sends this command to the blade. this is done to keep blade the correct color in the event
      // it should wiggle in its socket and momentarily lose connection and reset or a corrupted data command sets the blade
      // to a color other than what it should be
      case BLADE_REFRESH:

        // connect LED battery power
        #ifdef LED_PWR_SWITCH_PIN
          digitalWrite(LED_PWR_SWITCH_PIN, HIGH);
        #endif

        // set the blade color
        fill_solid(leds, NUM_LEDS, blade.lightsaber->color[INDEX_COLOR_TABLE_COLOR]);

        // set brightness to max
        FastLED.setBrightness(MAX_BRIGHTNESS);

        // change state to on
        blade.state = BLADE_ON;
        break;

      // low brightness flicker command; set the blade to some brightness level between 0 and 50% based on the value supplied
      case BLADE_FLICKER_LOW:
        FastLED.setBrightness((uint8_t)(((float)(blade.cmd & 0x0F)/0x0F) * (MAX_BRIGHTNESS >> 1)));
        next_step = millis() + 40;
        break;

      // high brightness flicker command; set the blade to some brightness level between 50 and 100% based on the value supplied
      case BLADE_FLICKER_HIGH:
        FastLED.setBrightness((uint8_t)((1 + (float)(blade.cmd & 0x0F)/0x0F) * (MAX_BRIGHTNESS >> 1)));
        next_step = millis() + 40;
        break;

      default:
        break;
    }

    // update the LED strip with any changes
    FastLED.show();
  }

  // any ongoing animation is handled here; if next_step has a value greater than 0 then some animation is happening
  // if next_step is less than or equal to the current time in milliseconds then it's time to execute the next step of the animation
  if (next_step > 0 && next_step <= millis()) {
    switch (blade.state) {

      // animate the blade igniting by turning on 1 LED at a time
      case BLADE_IGNITING:
        leds[animate_step] = blade.lightsaber->color[INDEX_COLOR_TABLE_COLOR];
        #ifdef MIRROR_MODE
          leds[(NUM_LEDS - 1) - animate_step] = blade.lightsaber->color[INDEX_COLOR_TABLE_COLOR];
        #endif

        animate_step++;

        #ifdef MIRROR_MODE
          if (animate_step >= ((NUM_LEDS + 1) / 2)) {
            animate_step = NUM_LEDS;
          }
        #endif

        if (animate_step < NUM_LEDS) {

          // time to next LED turning on is calculated based off the ignition time for this lightsaber and the number of LEDs in the blade
          next_step = millis() + (uint16_t)(TIME_DECODE(blade.lightsaber->ignition_time) / NUM_LEDS);

        // ignition squence has finished, blade is fully on
        } else {

          next_step = 0;
          blade.state = BLADE_ON;
        }
        break;

      // second part of the clash animation; set the blade back to its normal color
      case BLADE_CLASH:
        next_step = 0;
        fill_solid(leds, NUM_LEDS, blade.lightsaber->color[INDEX_COLOR_TABLE_COLOR]);
        blade.state = BLADE_IDLE;
        break;

      // animate the blade extinguishing by turning off 1 LED at a time
      case BLADE_EXTINGUISHING:

        #ifdef MIRROR_MODE
          if (NUM_LEDS % 2) {
            leds[(NUM_LEDS / 2) - animate_step] = CRGB_BLADE_OFF;
          } else {
            leds[(NUM_LEDS / 2) - (animate_step + 1)] = CRGB_BLADE_OFF;
          }
          leds[(NUM_LEDS / 2) + animate_step] = CRGB_BLADE_OFF;
        #else
          leds[(NUM_LEDS - 1) - animate_step] = CRGB_BLADE_OFF;
        #endif

        animate_step++;

        #ifdef MIRROR_MODE
          if (animate_step >= ((NUM_LEDS + 1) / 2)) {
            animate_step = NUM_LEDS;
          }
        #endif

        if (animate_step < NUM_LEDS) {

          // time to next LED turning off is calculated based off the extinguish time for this lightsaber and the number of LEDs in the blade
          next_step = millis() + (uint16_t)(TIME_DECODE(blade.lightsaber->extinguish_time) / NUM_LEDS);

        // extinguish squence has finished, blade is fully off
        } else {
          next_step = 0;
          blade.state = BLADE_OFF;
        }
        break;

      // second part of the flicker, reduce brightness by 20%
      case BLADE_FLICKER_LOW:
      case BLADE_FLICKER_HIGH:
        FastLED.setBrightness((uint8_t)((float)FastLED.getBrightness() * .8));
        next_step = 0;
        blade.state = BLADE_IDLE;
        break;

      case BLADE_OFF:
        // blade has been off for SLEEP_AFTER milliseconds. go to sleep to conserve power
        #ifdef ARDUINO_ARCH_AVR
          #ifdef ARDUINO_AVR_ATTINYX5

            // ATTiny85 will wake ONLY when external interrupt pin goes LOW
            //attachInterrupt(digitalPinToInterrupt(HILT_DATA_PIN), read_cmd, LOW);
            MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (LOW << ISC00);
          #endif

          sleep_enable();
          sleep_bod_disable();
          power_all_disable();
          sleep_cpu();                  // put the MCU to sleep
          power_all_enable();
          sleep_disable();

          #ifdef ARDUINO_AVR_ATTINYX5
            // return interrupt trigger to CHANGE instead of LOW
            //attachInterrupt(digitalPinToInterrupt(HILT_DATA_PIN), read_cmd, CHANGE);
            MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (CHANGE << ISC00);
           #endif
        #elif defined ARDUINO_ARCH_SAMD
          LowPower.sleep();
        #endif

        // after waking up, reset sleep timer in case blade never leaves the off state
        next_step = millis() + SLEEP_AFTER;
        break;

      default:
        break;
    }

    // update the LED strip with any changes
    // this is not called at the base of this function in order to reduce overhead caused by needlessly sending data out to the strip
    FastLED.show();
  }
}

// process_command() will interpret the received command and set blade properties appropriately
void process_command() {

  // DEBUG: display decoded command to serial monitor
  #ifdef SERIAL_DEBUG_ENABLE
    Serial.print("CMD: 0x");
    Serial.println(blade.cmd, HEX);
  #endif

  // identify command, set blade state and colors (if needed)
  switch (blade.cmd & 0xF0) {

    case 0x20:  // savi's ignite
      blade.lightsaber = &savi_lightsaber[(blade.cmd & 0x0F) % LIGHTSABER_TABLE_LEN];
      blade.state = BLADE_IGNITING;
      break;

    case 0x30:  // legacy ignite
      blade.lightsaber = &legacy_lightsaber[(blade.cmd & 0x0F) % LIGHTSABER_TABLE_LEN];
      blade.state = BLADE_IGNITING;
      break;

    case 0xA0:  // savi's set color

      // only perform a blade refresh if the blade is in an idle state or if it's off (to force it on after a missed ignite)
      if (blade.state == BLADE_IDLE || blade.state == BLADE_OFF) {
        blade.lightsaber = &savi_lightsaber[(blade.cmd & 0x0F) % LIGHTSABER_TABLE_LEN];
        blade.state = BLADE_REFRESH;
      } else {
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.print("Refresh blocked! Not in IDLE state; state is 0x");
          Serial.println(blade.state, HEX);
        #endif
      }
      break;

    case 0xB0:  // legacy set color

      // only perform a blade refresh if the blade is in an idle state
      if (blade.state == BLADE_IDLE || blade.state == BLADE_OFF) {
        blade.lightsaber = &legacy_lightsaber[(blade.cmd & 0x0F) % LIGHTSABER_TABLE_LEN];
        blade.state = BLADE_REFRESH;
      } else {
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.print("Refresh blocked! Not in IDLE state; state is 0x");
          Serial.println(blade.state, HEX);
        #endif
      }
      break;

    case 0x40:  // savi's extinguish
    case 0x50:  // legacy extinguish
      blade.state = BLADE_EXTINGUISHING;
      break;

    case 0x80:  // off
    case 0x90:  // off
    case 0xE0:  // off
    case 0xF0:  // off
      blade.state = BLADE_OFF;
      break;

    case 0xC0:  // savi's clash
    case 0xD0:  // legacy clash
      blade.state = BLADE_CLASH;
      break;

    case 0x60:  // blade flicker low (0-50% brightness)
      blade.state = BLADE_FLICKER_LOW;
      break;

    case 0x70:  // blade flicker high (53-100% brightness)
      blade.state = BLADE_FLICKER_HIGH;
      break;

    default:
      break;
  }
}

// read data from the hilt
// data is sent in a series of pulses. each pulse represents a 1-bit value (0 or 1)
// how long the pulse lasts will determine if it's a 1 or a 0
// collect 8 pulses in a row in a single, 8-bit value and store that to the global hilt_cmd variable
void read_cmd() {
  static uint8_t cmd = 0;
  static uint8_t bPos = 0;
  static uint32_t last_change = 0;
  uint32_t period = 0;

  // when transitioning from HIGH to LOW record the time the data line was HIGH
  // record the bit value that corresponds to the the length of time the data line was HIGH
  if (digitalRead(HILT_DATA_PIN) == LOW) {

    // determine the length of the the data line was high
    period = micros() - last_change;

    // if it was HIGH for less than 4ms treat it as a good bit value
    if (period < VALID_BIT_CUTOFF) {

      // shift the bits in the cmd variable left by 1 to make room for the new bit
      cmd <<= 1;

      // count that another bit has been received
      bPos++;

      // a HIGH period greater than 2ms can be treated as a 1, otherwise it is a 0
      // analysis of GE hilts shows that typically 1200-1600uS = 0, 2400-3000uS = 1
      // this value may need tweaking; see DEFINE near top of code
      if (period > VALID_BIT_ONE) {      
        cmd++;
      }

      // if more than 7 bits have been recorded (8 bits), we have a good 8-bit command.
      if (bPos > 7) {

        // store the 8-bit command to a global variable which will be picked up by loop() function
        hilt_cmd = cmd;

        // reset bit collection
        cmd = 0;
        bPos = 0;
      }

    // anything longer than 4ms reset bit collection
    } else {
      cmd = 0;
      bPos = 0;
    }

  // just transitioned into LOW (ACTIVE) period, record the time when it started
  } else {
    last_change = micros();
  }
}

// setup() performs one-time initialization steps
void setup() {

  #ifdef ARDUINO_ARCH_AVR
    // default all pins to INPUT_PULLUP mode in order to conserve power
    for (uint8_t i = 0; i < NUM_DIGITAL_PINS; i++ ) {
      pinMode(i, INPUT) ;
    }

    // set sleep mode
    #ifdef ARDUINO_ARCH_AVR
      #ifdef DO_NOT_SLEEP_PWR_DOWN
        set_sleep_mode(SLEEP_MODE_ADC);
      #else
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      #endif
    #endif

    // Disable ADC - reduce power consumption
    #if defined(ADCSRA) && defined(ADEN)
      ADCSRA &= ~(1 << ADEN);
    #endif

    // Disable SPI - reduce power consumption
    #if defined(SPCR) && defined(SPE)
      SPCR &= ~(1 << SPE);
    #endif
  #endif

  // setup LED power switch
  #ifdef LED_PWR_SWITCH_PIN
    pinMode(LED_PWR_SWITCH_PIN, OUTPUT);
    digitalWrite(LED_PWR_SWITCH_PIN, LOW);
  #endif

  // setup DATA pin for hilt
  pinMode(HILT_DATA_PIN, INPUT_PULLUP);

  // define (so we can shut off) the Trinket M0's on-board dotstar LED
  #ifdef ADAFRUIT_TRINKET_M0
    FastLED.addLeds<DOTSTAR, DOTSTAR_DATPIN, DOTSTAR_CLKPIN, BGR>(&dotstar, 1);
  #endif

  // define LED strip
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);

  // SAMD boards like the Trinket M0 need the ArduinoLowPower library to attach the interrupt to ensure the board wakes from sleep
  #ifdef ARDUINO_ARCH_SAMD
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(HILT_DATA_PIN), read_cmd, CHANGE);

  // for all other Arduino boards
  #else
    attachInterrupt(digitalPinToInterrupt(HILT_DATA_PIN), read_cmd, CHANGE);
  #endif

  // start the blade in an OFF state
  blade.state = BLADE_OFF;

  // serial output for debug purposes
  #ifdef SERIAL_DEBUG_ENABLE
    Serial.begin(115200);
    Serial.println("Ready!");
  #endif
}

// loop() is the main proram loop
void loop() {

  // check for a command from the hilt
  if (hilt_cmd != 0) {

    // copy command to a local variable as hilt_cmd is a global, volatile
    // variable that could be overwritten at any second
    blade.cmd = hilt_cmd;

    // immediately set hilt_cmd to zero so we know we've processed it
    hilt_cmd = 0;

    // call function that processes the command received from the hilt
    process_command();
  }

  // handle blade animations, sleep/wake, react to state changes, etc.
  blade_manager();
}
