/* hardware.h
 * Setup and management of the LED and MCU hardware.
 *
 * LED LIBRARIES (FastLED, tinyNeoPixel, Adafruit NeoPixel, Adafruit DotStar)
 *   my preferred LED library is FastLED.
 *
 *   if using Adafruit NeoPixel, be sure to also install the Adafruit DotStar library.
 *
 *   megaTinyCore does not support FastLED, however it does come with its own version of
 *   the Adafruit NeoPixel library
 *   see: https://github.com/SpenceKonde/megaTinyCore/tree/master/megaavr/libraries/tinyNeoPixel
 */
#pragma once

// define library-agnostic macros so the rest of the code can manage LEDs without having to know which
// specific hardware library is being used.
//
// this first block is for tinyNeoPixel and Adafruit NeoPixel libraries which share the same API
#if defined(MEGATINYCORE) || defined(USE_ADAFRUIT_NEOPIXEL)
  #define LED_OBJ             leds
  #define LED_RGB             LED_OBJ.Color
  #define LED_RGB_TYPE        uint32_t
  #define LED_SET_PIXEL(n, c) LED_OBJ.setPixelColor(n, c)     // n = pixel number, c = color
  #define LED_FILL(c)         LED_OBJ.fill(c)                 // c = color
  #define LED_FILL_N(c, s, n) LED_OBJ.fill(c, s, n)           // c = color, s = starting LED, n = number of LEDs to fill

  // we must use tinyNeoPixel for megaTinyCore
  //
  // using tinyNeoPixel_Static to save memory space which can be used to support larger strands
  // compiling will also clue us in as to whether or not we're near our pixel limit when we start seeing 'low memory available' warnings
  //
  // about 60 bytes for local variables is the target
  #ifdef MEGATINYCORE
    #include <tinyNeoPixel_Static.h>
    extern byte LED_OBJ_array[];
    extern tinyNeoPixel LED_OBJ;

  // otherwise use the Adafruit NeoPixel library
  #else
    #include <Adafruit_NeoPixel.h>
    extern Adafruit_NeoPixel LED_OBJ;

    // Adafruit_NeoPixel halts the millis() counter while executing; this slows ignition and extinguish considerably.
    // Use this value to compensate for the lost millis() time. This assuming 30 microseconds per LED.
    #define ADAFRUIT_ADJUST   ((TARGET_MAX * 30) / 1000)

    // Trinket M0 users also need the Adafruit DotStar library in order to turn off the on-board
    // DotStart LED (thus saving a few mA of power consumption)
    #ifdef ADAFRUIT_TRINKET_M0
      #include <Adafruit_DotStar.h>
      extern Adafruit_DotStar dotstar;
    #endif
  #endif

// if not using Adafruit NeoPixel library, assume FastLED
#else
  #include <FastLED.h>
  #define LEDLIB_FASTLED
  #define LED_OBJ             FastLED
  #define LED_RGB             CRGB
  #define LED_RGB_TYPE        CRGB
  #define LED_SET_PIXEL(n, c) leds[n] = c
  #define LED_FILL(c)         fill_solid(leds, NUM_LEDS, c)

  // Trinket M0 users also need a DotStar object defined to turn off the on-board DotStar LED.
  #ifdef ADAFRUIT_TRINKET_M0
    extern LED_RGB_TYPE dotstar;  // Trinket M0 dotstar LED
  #endif
  extern LED_RGB_TYPE leds[];     // blade LEDs
#endif

// for tinyAVR MCUs (using megaTinyCore) we're going to not use interrupts to detect data from the hilt.
// instead we'll use the event system and timer B to capture how long each data pulse was HIGH.
//
// this is done because the show() command disables interrupts which causes state changes on the hilt
// data pin to be missed and also disrupts the detected timings because the micros() counter also stops
// while interrupts are disabled.
// 
// other MCUs likely have a similar issue, but i'm not currenlty invested in getting them perfect like
// i am with my ATTiny806/1606-based blade project. maybe i'll look at them more closely another day.
#if defined(MEGATINYCORE) && defined(TCB0) && defined(EVSYS)
  #warning "Using AVR tricks to read commands from hilt. NO WARRANTY!"

  // identify that we're using the AVR event system timer to capture data rather than interrupts.
  #define USE_AVR_EV_CAPT

  // translate bit timings from microseconds to clock ticks
  #define TCB0_TICKS_FROM_US(us, clk_div)   ((uint32_t)((us) * (F_CPU / (1000000UL * (clk_div)))))
  #define VALID_BIT_CUTOFF_IN_TICKS         TCB0_TICKS_FROM_US(VALID_BIT_CUTOFF_IN_US, 2)
  #define VALID_BIT_SPLIT_IN_TICKS          TCB0_TICKS_FROM_US(VALID_BIT_SPLIT_IN_US,  2)
#endif

// DONT_SHOW
//
// when used, this feature will tell the hardware to skip updating the LEDs via call to show()  
// because there is an incoming command and a call to show() will likely corrupt the incoming command
//
// currently this is only being used by AVR with MegaTinyCore, but I should probably use this with all
// hardware. i need to do more testing before I do. 
//
// TODO: see read_cmd() in hilt_cmd.cpp which is currently where this is set and currently only being
//       applied to AVRs via the USE_AVR_EV_CAPT define; we need to extract DONT_SHOW from those confines
#ifdef USE_DONT_SHOW
  extern bool dont_show;
  #define SHOW_LEDS if(!dont_show)LED_OBJ.show
#else
  #define SHOW_LEDS LED_OBJ.show
#endif

// something to help calculate values for ignition and extinguish loops
#ifdef MIRROR_MODE
  #define TARGET_MAX ((NUM_LEDS + 1) / 2)
#else
  #define TARGET_MAX NUM_LEDS
#endif

// power consumption considerations
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
  #include <avr/sleep.h>
  #include <avr/power.h>
#elif defined ARDUINO_ARCH_SAMD
  #include "ArduinoLowPower.h"    // see https://www.arduino.cc/en/Reference/ArduinoLowPower
  #define VERY_LOW_POWER          // for the Arduino SAMD core; helps reduce power consumption
#endif

// program space considerations for the ATtiny806 which has a very small amount of program space
// i'll leave this separate from the next ifdef block in case we find other MCUs that have similar
// space constraint issues (ATtiny85 perhaps?)
#if defined(ARDUINO_AVR_ATtiny806)

  // defining SPACE_SAVER will enable some space-saving steps within the code to help it fit into smaller program space areas
  #define SPACE_SAVER
#endif

// help save program space
#ifdef SPACE_SAVER

  // remove all uses of Serial() as it requires a good chunk of program space.
  #undef SERIAL_DEBUG_ENABLE
#endif

// function prototypes
void led_power_off();
void led_power_on();
void harware_setup();
void hardware_sleep();
