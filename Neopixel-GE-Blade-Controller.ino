/* Galaxy's Edge Lightsaber Compatible Neopixel Blade Controller : v1.8.4
 * code by ruthsarian@gmail.com
 *
 * ABOUT
 *  This code is designed to behave as a blade controller for blades containing addressable RGB LEDs (NeoPixels).
 *  It is compatible with Galaxy's Edge lightsaber hilts. 
 *
 *  It was inspired by the Dead Bothans Society's custom blade project which utilizes a Trinket M0
 *    see: https://www.deadbothans.com/2019/08/31/switching-to-m0-trinket-and-custom-connectors/
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
 *   - ATTiny(8|16)06
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
 * NOTES
 *
 *  Power Consumption
 *    - Trinket M0
 *      ; Trinket M0 on its own, in standby, consumes 1.2mA; removing green power LED (or its resistor) only saves about 200uA of current; probably not worth removing
 *      ; Neopixel strip, when off, consumes about 1mA per pixel. this is why a power switch of some sort to the cut power off from the LED strip is necessary
 *    - NeoPixels (WS2812B)
 *      ; consume about 1mA per pixel while off; 91mA w/144 pixels
 *      ; at max brightness (255), 144 pixels consume about 1.3A
 *      ; at my brightness (64), 144 pixels consume about 500mA
 *
 *  ATTinyX06 / tinyNeoPixel Limitations
 *    Limited memory of the ATTinyX06s cause longer strands of LEDs to not ignite.
 *    
 *    Compile, but don't upload, your sketch. If the output window reports less than 60 bytes of
 *    memory available for local variables then you'll need to decrease NUM_LEDs.
 *    
 *    If you notice LEDs are not fully turning off and have a dim, white color to them then the
 *    issue is with timing. Increase the clock speed to 16MHz or 20MHz.
 *    
 *  ATTinyX06 : tinyNeoPixel : tinyNeoPixelStatic
 *        806   58             92
 *       1606   227            264
 *    
 *  ATTinyX06 VQFN20 Pinout Notes
 *    0 = PA4 = PIN 5
 *    2 = PA6 = PIN 7
 *    4 = PB5 = PIN 9
 *
 * // defines I use with my top secret custom blade project
 * #define HILT_DATA_PIN           PIN_PC3
 * #define LED_DATA_PIN            PIN_PA2
 * #define LED_PWR_SWITCH_PIN      PIN_PA5
 * #define LED_PWR_ON              2 
 *
 */

#define ADAFRUIT_LED_TYPE       NEO_GRB+NEO_KHZ800  // define the NeoPixel type to use with the Adafruit NeoPixel library.
                                                    // if you are NOT using the NeoPixel library then you can ignore this.
                                                    // see: https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html
                                                    //
                                                    // NEO_RGB, NEO_GRB+NEO_KHZ800

#define FASTLED_LED_TYPE        WS2812B // define the type of LED used with the FastLED library
                                        // if you are NOT using the FastLED library then you can ignore this
                                        // see: https://github.com/FastLED/FastLED/blob/master/src/FastLED.h
#define FASTLED_RGB_ORDER       GRB     // the color order for the LEDs
                                        // if you are NOT using the FastLED library then you can ignore this

#define NUM_LEDS                144     // number of LEDs in the strip
#define MAX_BRIGHTNESS          64      // default brightness; lower value = lower current draw
#define HILT_DATA_PIN           2       // digital pin the hilt's data line is connected to
#define LED_DATA_PIN            4       // digital pin the LED strip is attached to
#define LED_PWR_SWITCH_PIN      0       // this pin is held low until the blade turns on, at which point it will be pushed high
                                        // this can be used to control a switch to connect and disconnect a battery switch; see: https://www.pololu.com/product/2811
                                        // if not using a switch, comment out this define
#define LED_PWR_ON              1       // the state of LED_PWR_SWITCH_PIN that will enable power to the LEDs
                                        // 0 = LOW, 1 = HIGH, 2 = LOW w/tri-state off, 3 = HIGH w/tri-state off
//#define MIRROR_MODE                   // uncomment to enable mirror mode
                                        // mirror mode treats the strip of LEDs as a single strip, folded in half, to create the blade
                                        // as such, when igniting both the first and last LEDs in the strip will turn on and the next in turn
                                        // until the last LED to turn on is the one in the middle of the strip
#define SLEEP_AFTER             60000   // how many ms to wait, after turning off, before going to sleep to conserve power
                                        // sleep will also stop the COM port of your microcontroller from appearing on your computer
                                        // set this as a large value while doing development, then lower it to 60000 or less for a
                                        // 'production' environment.
#define SERIAL_DEBUG_ENABLE             // enable debug messages over serial
#define VALID_BIT_CUTOFF        4000    // any HIGH period on the data line longer than this value, in microseconds, is considered an invalid bit of data and causes a reset of the data capture
#define VALID_BIT_ONE           1600    // any HIGH period longer than this value, in microseconds, but less than VALID_BIT_CUTOFF is treated as a valid 1 bit
                                        // any HIGH period shorter than this value, in microseconds, is treated as a valid 0 bit
                                        // if blade is not registering commands correctly, this value likely needs to be tweaked
                                        // typically a 1 bit is about 2000uS (Legacy) or 2400uS (Savi) long and a 0 is 1200uS long. 
                                        // this value is set between 2000 and 1200 to accomadate delays in timing/processing
#define COLOR_MODE_CHANGE_TIME  1500    // if a blade is turned off then on again within this amount of time, then change to the next color mode
#define COLOR_WHEEL_PAUSE_TIME  2000    // how long to hold a color before moving to the next color
#define COLOR_WHEEL_CYCLE_STEP  16      // how many steps to jump when calculating the next color in the color cycle; a power of 2 is recommended
//#define USE_ADAFRUIT_NEOPIXEL         // uncomment to use the Adafruit NeoPixel library instead of FastLED; THERE IS NO REASON TO DO THIS ... unless there is

// megaTinyCore does not support FastLED, however it does come with its own version of
// the Adafruit NeoPixel library. 
// see: https://github.com/SpenceKonde/megaTinyCore/tree/master/megaavr/libraries/tinyNeoPixel
//
// since i'm adding support for tinyNeoPixel, I might as well add Adafruit NeoPixel
// support since they share the same API.
//
// to make the code LED library agnostic (ish), i'll use the macros defined below to control LEDs
//
#if defined(MEGATINYCORE) || defined(USE_ADAFRUIT_NEOPIXEL)
  #define LEDLIB_ADAFRUIT_NEOPIXEL
  #define LED_OBJ             leds
  #define LED_RGB             LED_OBJ.Color
  #define LED_RGB_TYPE        uint32_t
  #define LED_SET_PIXEL(n, c) LED_OBJ.setPixelColor(n, c)     // n = pixel number, c = color
  #define LED_FILL(c)         LED_OBJ.fill(c)                 // c = color
  #define LED_FILL_N(c, s, n) LED_OBJ.fill(c, s, n)           // c = color, s = starting LED, n = number of LEDs to fill

  // must use tinyNeoPixel for megaTinyCore
  //
  // using tinyNeoPixel_Static to save memory space which can be used to support larger strands
  // compiling will also clue us in as to whether or not we're near our pixel limit when we start seeing 'low memory available' warnings
  //
  // about 60 bytes for local variables is the target
  #ifdef MEGATINYCORE
    #include <tinyNeoPixel_Static.h>
    byte LED_OBJ_array[NUM_LEDS * 3];
    tinyNeoPixel LED_OBJ = tinyNeoPixel(NUM_LEDS, LED_DATA_PIN, ADAFRUIT_LED_TYPE, LED_OBJ_array);

  // otherwise use the Adafruit NeoPixel library
  #else
    #include <Adafruit_NeoPixel.h>
    Adafruit_NeoPixel LED_OBJ = Adafruit_NeoPixel(NUM_LEDS, LED_DATA_PIN, ADAFRUIT_LED_TYPE);

    // Trinket M0 users also need the Adafruit DotStar library in order to turn off the on-board
    // DotStart LED (thus saving a few mA of power consumption)
    #ifdef ADAFRUIT_TRINKET_M0
      #include <Adafruit_DotStar.h>
      Adafruit_DotStar dotstar = Adafruit_DotStar(1, INTERNAL_DS_DATA, INTERNAL_DS_CLK, DOTSTAR_BGR);
    #endif
  #endif

// If not using Adafruit NeoPixel or tinyNeoPixel then use FastLED.
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
    LED_RGB_TYPE dotstar;       // Trinket M0 dotstar LED
  #endif
  LED_RGB_TYPE leds[NUM_LEDS];  // blade LEDs
#endif

// something to help calculate values for ignition and extinguish loops
#ifdef MIRROR_MODE
  #define TARGET_MAX ((NUM_LEDS + 1) / 2)
#else
  #define TARGET_MAX NUM_LEDS
#endif

// Adafruit_NeoPixel halts the millis() counter while executing; this slows ignition and extinguish considerably.
// Use this value to compensate for the lost millis() time
#ifdef MEGATINYCORE
  #define ADAFRUIT_ADJUST   (TARGET_MAX >> 5)
#else
  #define ADAFRUIT_ADJUST   (TARGET_MAX >> 4)
#endif

// power consumption considerations
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
  #include <avr/sleep.h>
  #include <avr/power.h>
#elif defined ARDUINO_ARCH_SAMD
  #include "ArduinoLowPower.h"    // see https://www.arduino.cc/en/Reference/ArduinoLowPower
  #define VERY_LOW_POWER          // for the Arduino SAMD core; helps reduce power consumption
#endif

// program space considerations
#if defined(ARDUINO_AVR_ATtiny806) || defined(ARDUINO_AVR_ATtiny1606)

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
  #define TIME_ENCODE(t)  (uint16_t)t
  #define TIME_DECODE(t)  (uint16_t)t
  typedef uint16_t blade_timing_t;
#endif

// stock blade colors; these values are based on PWM duty cycles measured from a stock blade controller
#define RGB_BLADE_WHITE         LED_RGB(102, 102, 102)
#define RGB_BLADE_RED           LED_RGB(255,   0,   0)
#define RGB_BLADE_ORANGE        LED_RGB(231,  77,   0)
#define RGB_BLADE_YELLOW        LED_RGB(154, 154,   0)
#define RGB_BLADE_GREEN         LED_RGB(  0, 255,   0)
#define RGB_BLADE_CYAN          LED_RGB(  0, 154, 154)
#define RGB_BLADE_BLUE          LED_RGB(  0,   0, 255)
#define RGB_BLADE_PURPLE        LED_RGB(154,   0, 154)
#define RGB_BLADE_DARK_PURPLE   LED_RGB( 26,   0,  13)
#define RGB_BLADE_CLASH_YELLOW  LED_RGB(255, 255,   0)
#define RGB_BLADE_CLASH_ORANGE  LED_RGB(255,  64,   0)
#define RGB_BLADE_CLASH_WHITE   LED_RGB(128, 128, 128)
#define RGB_BLADE_OFF           LED_RGB(  0,   0,   0)

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
const LED_RGB_TYPE color_table[][2] = {
  {RGB_BLADE_WHITE,        RGB_BLADE_CLASH_YELLOW},
  {RGB_BLADE_RED,          RGB_BLADE_CLASH_ORANGE},
  {RGB_BLADE_ORANGE,       RGB_BLADE_CLASH_WHITE},
  {RGB_BLADE_YELLOW,       RGB_BLADE_CLASH_WHITE},
  {RGB_BLADE_GREEN,        RGB_BLADE_CLASH_YELLOW},
  {RGB_BLADE_CYAN,         RGB_BLADE_CLASH_YELLOW},
  {RGB_BLADE_BLUE,         RGB_BLADE_CLASH_YELLOW},
  {RGB_BLADE_PURPLE,       RGB_BLADE_CLASH_YELLOW},
  {RGB_BLADE_DARK_PURPLE,  RGB_BLADE_CLASH_ORANGE}
};

// stock lightsaber properties template
typedef struct {
  uint8_t color_index;
  blade_timing_t ignition_time;
  blade_timing_t extinguish_time_delay;
  blade_timing_t extinguish_time;
} stock_lightsaber_t;

// savi's workshop lightsaber properties
// timing values were taken from logic analyzer captures of a stock V2 blade controller
const stock_lightsaber_t savi_lightsaber[LIGHTSABER_TABLE_LEN] = {
//  COLOR,                    IGNITION_TIME,    EXTINGUISH_TIME_DELAY,  EXTINGUISH_TIME
  { INDEX_BLADE_WHITE,        TIME_ENCODE(280), TIME_ENCODE(370),       TIME_ENCODE(470)}, //  white kyber crystal
  { INDEX_BLADE_RED,          TIME_ENCODE(280), TIME_ENCODE(940),       TIME_ENCODE(470)}, //    red kyber crystal
  { INDEX_BLADE_ORANGE,       TIME_ENCODE(280), TIME_ENCODE(000),       TIME_ENCODE(275)}, // orange kyber crystal
  { INDEX_BLADE_YELLOW,       TIME_ENCODE(280), TIME_ENCODE(000),       TIME_ENCODE(275)}, // yellow kyber crystal
  { INDEX_BLADE_GREEN,        TIME_ENCODE(280), TIME_ENCODE(180),       TIME_ENCODE(470)}, //  green kyber crystal
  { INDEX_BLADE_CYAN,         TIME_ENCODE(280), TIME_ENCODE(275),       TIME_ENCODE(565)}, //   cyan kyber crystal
  { INDEX_BLADE_BLUE,         TIME_ENCODE(280), TIME_ENCODE(275),       TIME_ENCODE(565)}, //   blue kyber crystal
  { INDEX_BLADE_PURPLE,       TIME_ENCODE(280), TIME_ENCODE(370),       TIME_ENCODE(660)}, // purple kyber crystal
  { INDEX_BLADE_DARK_PURPLE,  TIME_ENCODE(280), TIME_ENCODE(945),       TIME_ENCODE(470)}, //
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { INDEX_BLADE_YELLOW,       TIME_ENCODE(240), TIME_ENCODE(000),       TIME_ENCODE(240)}, //
  { INDEX_BLADE_GREEN,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, //
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { INDEX_BLADE_BLUE,         TIME_ENCODE(240), TIME_ENCODE(245),       TIME_ENCODE(500)}, //
  { INDEX_BLADE_PURPLE,       TIME_ENCODE(240), TIME_ENCODE(330),       TIME_ENCODE(545)}  //
};

// legacy lightsaber properties
// timing values were taken from logic analyzer captures of a stock V2 blade controller
const stock_lightsaber_t legacy_lightsaber[LIGHTSABER_TABLE_LEN] = {
//  COLOR,                    IGNITION_TIME,    EXTINGUISH_TIME_DELAY,  EXTINGUISH_TIME
  { INDEX_BLADE_YELLOW,       TIME_ENCODE(240), TIME_ENCODE(000),       TIME_ENCODE(325)}, // Temple Guard
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(335)}, // Kylo Ren
  { INDEX_BLADE_BLUE,         TIME_ENCODE(240), TIME_ENCODE(500),       TIME_ENCODE(415)}, // Rey (Anakin), Rey Reforged, Ahsoka (CW)
  { INDEX_BLADE_PURPLE,       TIME_ENCODE(240), TIME_ENCODE(490),       TIME_ENCODE(585)}, // Mace Windu
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(660),       TIME_ENCODE(335)}, // Asajj Ventress
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, // Ahsoka (Rebels)
  { INDEX_BLADE_GREEN,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, // Luke
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(500),       TIME_ENCODE(500)}, // Darth Vader
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(750),       TIME_ENCODE(330)}, // Darth Maul
  { INDEX_BLADE_BLUE,         TIME_ENCODE(240), TIME_ENCODE(495),       TIME_ENCODE(415)}, // Obi-Wan, Ben Solo
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}
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

// color mode
typedef enum {
  COLOR_MODE_STOCK,
  COLOR_MODE_WHEEL_CYCLE,
  COLOR_MODE_WHEEL_HOLD
} color_mode_t;

// blade properties template
typedef struct {
  state_t state;
  uint8_t cmd;
  const stock_lightsaber_t *lightsaber;
  color_mode_t color_mode;
  LED_RGB_TYPE color;
  LED_RGB_TYPE color_clash;
} blade_t;

// global blade properties object
blade_t blade;

// global variable where decoded hilt command is stored
// it has the volatile keyword because it will be updated from within an interrupt service routine
volatile uint8_t hilt_cmd = 0;

// generate an RGB color based on an 8-bit input value
LED_RGB_TYPE color_by_wheel(uint8_t wheel) {
  wheel = 255 - wheel;
  if (wheel < 85) {
    return(LED_RGB(255 - wheel * 3, 0, wheel * 3));
  } else if (wheel < 170) {
    wheel -= 85;
    return(LED_RGB(0, wheel * 3, 255 - wheel * 3));
  } else {
    wheel -= 170;
    return(LED_RGB(wheel * 3, 255 - wheel * 3, 0));
  }
}

// switch off power to the LEDs
void led_power_off() {
  switch(LED_PWR_ON) {
    case 0:
      digitalWrite(LED_PWR_SWITCH_PIN, HIGH);
      break;

    // if the power switch is a PNP transistor and power to the LEDs is higher than 
    // the power of the microcontroller, then even if output is set HIGH, voltage at
    // the base will be lower than the emitter, thus power will flow to the LEDs.
    //
    // in 'tri-state' mode the pin goes high impedance, preventing any current flowing into
    // the pin, thus turning the PNP transistor off even if it's emitter has a higher voltage.
    //
    // see: https://forum.arduino.cc/t/tri-state-logic-on-io-pins/12277
    case 2:
    case 3:
      digitalWrite(LED_PWR_SWITCH_PIN, LOW);
      pinMode(LED_PWR_SWITCH_PIN, OUTPUT);
      pinMode(LED_PWR_SWITCH_PIN, INPUT);
      break;
    default:
      digitalWrite(LED_PWR_SWITCH_PIN, LOW);
      break;
  }
}

// switch on power to the LEDs
void led_power_on() {
  switch(LED_PWR_ON) {
    case 2:
      pinMode(LED_PWR_SWITCH_PIN, OUTPUT);
    case 0:
      digitalWrite(LED_PWR_SWITCH_PIN, LOW);
      break;
    case 3:
      pinMode(LED_PWR_SWITCH_PIN, OUTPUT);
    default:
      digitalWrite(LED_PWR_SWITCH_PIN, HIGH);
      break;
  }
}

// blade_manager() takes care of changing the colors of the blade
//
// if you're looking to add custom blade behaviors then this function is likely
// where you want to make your changes.
void blade_manager() {
  static uint32_t next_step = 0;
  static uint32_t animate_step = 0;
  static uint32_t last_extinguish = 0;
  static state_t last_state = BLADE_UNINITIALIZED;
  static uint8_t wheel_index = 0;
  uint16_t target;

  //
  // ** BLADE MANAGER STAGE ONE : NEW STATE INITIALIZATION **
  //
  // The blade has changed states. In this section of code any initialization needed for the new
  // blade state is handled.
  //
  if (last_state != blade.state) {
    last_state = blade.state;

    // do not allow a refresh, on, or idle state change to disrupt any potential ongoing effects
    switch (blade.state) {
      case BLADE_REFRESH:
      case BLADE_ON:
      case BLADE_IDLE:
        break;
      default:
        next_step = 0;
        animate_step = 0;
        break;
    }

    switch (blade.state) {

      // the blade is off. disable any running animations and shut the LEDs off
      case BLADE_OFF:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_OFF"));
        #endif

        // set the point when the blade controller should go to sleep
        next_step = millis() + SLEEP_AFTER;

        // turn the LEDs off
        LED_OBJ.clear();
        LED_OBJ.show();     // unnecessary since the next step is to cut power to the LEDs
                            // i'm leaving this line in because my test environment will sometimes
                            // involve keeping the LEDs always powered
                            //
                            // and someone may have their pololu switch set to ON thus negating
                            // the whole purpose of the power switch to begin with.

        // disconnect power to the LEDs
        #ifdef LED_PWR_SWITCH_PIN
          led_power_off();
        #endif
        break;

      // the blade is powering on
      case BLADE_IGNITING:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_IGNITING"));
        #endif

        // switch color modes if blade was off for less than COLOR_MODE_CHANGE_TIME
        if (last_extinguish > 0 && (millis() - last_extinguish) < COLOR_MODE_CHANGE_TIME) {

          // the color mode we move to next is based on the current color mode
          switch (blade.color_mode) {

            case COLOR_MODE_STOCK:
              #ifdef SERIAL_DEBUG_ENABLE
                Serial.println(F("New Color Mode: COLOR_MODE_WHEEL_CYCLE"));
              #endif

              blade.color_mode = COLOR_MODE_WHEEL_CYCLE;
              break;

            case COLOR_MODE_WHEEL_CYCLE:
              #ifdef SERIAL_DEBUG_ENABLE
                Serial.println(F("New Color Mode: COLOR_MODE_WHEEL_HOLD"));
              #endif

              blade.color_mode = COLOR_MODE_WHEEL_HOLD;
              break;

            default:
              #ifdef SERIAL_DEBUG_ENABLE
                Serial.println(F("New Color Mode: COLOR_MODE_STOCK"));
              #endif

              blade.color_mode = COLOR_MODE_STOCK;
              break;              
          }
        }

        // set the color and clash color of the blade based on the current color mode
        switch (blade.color_mode) {

          // wheel color is based on wheel_index
          case COLOR_MODE_WHEEL_CYCLE:
          case COLOR_MODE_WHEEL_HOLD:
            blade.color = color_by_wheel(wheel_index);
            blade.color_clash = RGB_BLADE_CLASH_WHITE;   // TODO: intelligently pick a clash color; CRGB(255, 255, 255)
            break;

          // in all other instances, use the stock blade color
          default:
            blade.color = color_table[blade.lightsaber->color_index][INDEX_COLOR_TABLE_COLOR];
            blade.color_clash = color_table[blade.lightsaber->color_index][INDEX_COLOR_TABLE_CLASH];
            break;
        }

        // connect LED battery power
        #ifdef LED_PWR_SWITCH_PIN
          led_power_on();
        #endif

        // clear the LEDs immediately after they are powered on via LED_PWR_SWITCH_PIN
        LED_OBJ.clear();

        // set the brightness for the blade
        LED_OBJ.setBrightness(MAX_BRIGHTNESS);

        // delay ignition based on whatever value is stored in the lightsaber's properties
        next_step = millis();
        break;

      // BLADE_ON is when the blade has just finished igniting; perhaps there's something we'll want to do only 
      // under that situation, which is why BLADE_ON and BLADE_IDLE are separate things
      case BLADE_ON:
        blade.state = BLADE_IDLE;
        break;

      // blade is at idle
      case BLADE_IDLE:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_IDLE"));
        #endif

        // some color modes may want to do something while the blade is idling
        switch (blade.color_mode) {

          // in color wheel cycle mode, cycle through colors in the wheel every COLOR_WHEEL_PAUSE_TIME milliseconds
          //
          // at this point, it's possible we're entering an IDLE state after a blade refresh, in which case we don't
          // want to touch next_step; only set next_step if it has a value less than the current time in ms
          case COLOR_MODE_WHEEL_CYCLE:
            if (next_step < millis()) {
              next_step = millis() + COLOR_WHEEL_PAUSE_TIME;
            }
            break;
        }
        break;

      // a clash command has been sent; this happens when the blade hits something or the hilt stops suddenly
      case BLADE_CLASH:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_CLASH"));
        #endif

        // immediately set the blade to the clash color
        LED_FILL(blade.color_clash);

        // wait 40 milliseconds and then change the blade color back to normal
        next_step = millis() + 40;
        break;

      // the blade is turning off
      case BLADE_EXTINGUISHING:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_EXTINGUISHING"));
        #endif

        last_extinguish = millis();

        // different lightsabers have different delays before the extinguish begins, so this statement sets that delay
        next_step = millis() + TIME_DECODE(blade.lightsaber->extinguish_time_delay);
        break;

      // every second or so the hilt sends this command to the blade. this is done to keep blade the correct color in the event
      // it should wiggle in its socket and momentarily lose connection and reset or a corrupted data command sets the blade
      // to a color other than what it should be
      case BLADE_REFRESH:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_REFRESH"));
        #endif

        // connect LED battery power
        #ifdef LED_PWR_SWITCH_PIN
          led_power_on();
        #endif

        // set the blade color
        LED_FILL(blade.color);

        // set brightness to max
        LED_OBJ.setBrightness(MAX_BRIGHTNESS);

        // change state to on
        blade.state = BLADE_ON;
        break;

      // low brightness flicker command; set the blade to some brightness level between 0 and 50% based on the value supplied
      case BLADE_FLICKER_LOW:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_FLICKER_LOW"));
        #endif

        LED_OBJ.setBrightness((uint8_t)(((float)(blade.cmd & 0x0F)/0x0F) * (MAX_BRIGHTNESS >> 1)));
        next_step = millis() + 40;
        break;

      // high brightness flicker command; set the blade to some brightness level between 50 and 100% based on the value supplied
      case BLADE_FLICKER_HIGH:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_FLICKER_HIGH"));
        #endif

        LED_OBJ.setBrightness((uint8_t)((1 + (float)(blade.cmd & 0x0F)/0x0F) * (MAX_BRIGHTNESS >> 1)));
        next_step = millis() + 40;
        break;

      default:
        break;
    }

    // update the LED strip with any changes
    LED_OBJ.show();
  }

  //
  // ** BLADE MANAGER STAGE TWO : ONGOING ACTION **
  //
  // If next_step has a value greater than 0 then some animation is happening.
  // 
  // If next_step is less than or equal to the current time in milliseconds then it's time to execute the next step
  // of action (animation, color change, etc.)
  //
  if (next_step > 0 && next_step <= millis()) {
    switch (blade.state) {

      // animate the blade igniting by turning on 1 LED at a time
      case BLADE_IGNITING:

        // if using the Adafruit NeoPixel library, adjust next_step to take into account the time lost
        // due to the library blocking the increment of millis() during its operations
        #ifdef LEDLIB_ADAFRUIT_NEOPIXEL
          next_step -= ADAFRUIT_ADJUST;
        #endif

        // next_step contains the timestamp when the blade began ignition
        //
        // calculate how much time has elapsed since ignition began and calculate how many LEDs should
        // be ignited at this point.
        if ((next_step + TIME_DECODE(blade.lightsaber->ignition_time)) <= millis()) {
          target = TARGET_MAX;
        } else {
          target = ceil(((millis() - next_step)/(float)(TIME_DECODE(blade.lightsaber->ignition_time))) * NUM_LEDS);

          // do not allow target to go above TARGET_MAX
          if (target > TARGET_MAX) {
              target = TARGET_MAX;
          }
        }

        // ignite the blade

        // if the LED_FILL_N macro exists (Adafruit NeoPixel) use that to help speed things up
        // by filling multiple pixels in one step rather than incrementing through each pixel
        // individually using LED_SET_PIXEL
        #ifdef LED_FILL_N

          // the fill command, if given a count value of 0, will fill the entire blade
          // so make sure we have at least 1 LED to fill before using that fill command
          if ((target - animate_step) > 0) {
            //LED_FILL_N(blade.color, animate_step, target - animate_step);
            LED_SET_PIXEL(animate_step++, blade.color);

            // if using MIRROR_MODE, start ignting from the end of the stip towards the center
            #ifdef MIRROR_MODE
              LED_FILL_N(blade.color, NUM_LEDS - target, target - animate_step);
            #endif

            // update animate_step
            animate_step = target;

            // it seems that very short strands need this little delay 
            // when using tinyNeoPixel. i do not know why, but without it
            // the ignition and extinguish animations do not work.
            #ifdef MEGATINYCORE
              delayMicroseconds(100);
            #endif
          }
        #else

          // increment through each pixel in this block and set its color
          for (;animate_step < target;animate_step++) {
            LED_SET_PIXEL(animate_step, blade.color);

            #ifdef MIRROR_MODE
              LED_SET_PIXEL((NUM_LEDS - 1) - animate_step, blade.color);
            #endif
          }
        #endif

        // have we reached the end of the strip of LEDs?
        if (target >= TARGET_MAX) {
          next_step = 0;
          blade.state = BLADE_ON;
        }
        break;

      // second part of the clash animation; set the blade back to its normal color
      case BLADE_CLASH:
        next_step = 0;
        LED_FILL(blade.color);
        blade.state = BLADE_IDLE;
        break;

      // animate the blade extinguishing by turning off 1 LED at a time
      case BLADE_EXTINGUISHING:

        #ifdef LEDLIB_ADAFRUIT_NEOPIXEL
          next_step -= ADAFRUIT_ADJUST;
        #endif

        // how many LEDs should be extinguished at this point in time?
        if ((next_step + TIME_DECODE(blade.lightsaber->extinguish_time)) <= millis()) {
          target = TARGET_MAX;
        } else {
          target = ceil(((millis() - next_step)/(float)(TIME_DECODE(blade.lightsaber->extinguish_time))) * NUM_LEDS);
          if (target > TARGET_MAX) {
              target = TARGET_MAX;
          }
        }

        #ifdef LED_FILL_N
          if ((target - animate_step) > 0) {
            #ifdef MIRROR_MODE
              LED_FILL_N(RGB_BLADE_OFF, TARGET_MAX - target, target * 2);
            #else
              LED_FILL_N(RGB_BLADE_OFF, NUM_LEDS - target, target - animate_step);
            #endif

            animate_step = target;

            #ifdef MEGATINYCORE
              delayMicroseconds(100);
            #endif
          }
        #else

          // extinguish those LEDs
          for (;animate_step < target;animate_step++) {
            #ifdef MIRROR_MODE
              LED_SET_PIXEL(TARGET_MAX + animate_step, RGB_BLADE_OFF);
              LED_SET_PIXEL(TARGET_MAX - animate_step, RGB_BLADE_OFF);
            #else
              LED_SET_PIXEL((NUM_LEDS - 1) - animate_step, RGB_BLADE_OFF);
            #endif
          }
        #endif

        if (animate_step >= TARGET_MAX) {
          next_step = 0;
          blade.state = BLADE_OFF;
        }
        break;

      // second part of the flicker, reduce brightness by 20%
      case BLADE_FLICKER_LOW:
      case BLADE_FLICKER_HIGH:
        LED_OBJ.setBrightness((uint8_t)((float)LED_OBJ.getBrightness() * .8));
        next_step = 0;
        blade.state = BLADE_IDLE;
        break;

      case BLADE_OFF:
        // blade has been off for SLEEP_AFTER milliseconds. go to sleep to conserve power
        #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)

          // allow the MCU to go to sleep
          sleep_enable();

          // megaTinyCore has BOD configured in the board settings, not here.
          #ifndef ARDUINO_ARCH_MEGAAVR
          sleep_bod_disable();
          power_all_disable();
          #endif

          sleep_cpu();                  // put the MCU to sleep

          #ifndef ARDUINO_ARCH_MEGAAVR
          power_all_enable();
          #endif

          sleep_disable();

        #elif defined ARDUINO_ARCH_SAMD
          LowPower.sleep();
        #endif

        // after waking up, reset sleep timer in case blade never leaves the off state
        next_step = millis() + SLEEP_AFTER;
        break;

      // blade is at idle
      case BLADE_IDLE:

        switch (blade.color_mode) {

          // if in the color wheel cycle mode
          case COLOR_MODE_WHEEL_CYCLE:

            // increment the wheel
            wheel_index += COLOR_WHEEL_CYCLE_STEP;

            #ifdef SERIAL_DEBUG_ENABLE
              Serial.print(F("\nNext Color: "));
              Serial.println(wheel_index);
            #endif

            // set the new color by wheel
            blade.color = color_by_wheel(wheel_index);
            LED_FILL(blade.color);
            next_step = millis() + COLOR_WHEEL_PAUSE_TIME;
            break;
        }
        break;

      default:
        break;
    }

    // update the LED strip with any changes
    // this is not called at the base of this function in order to reduce overhead caused by needlessly sending data out to the strip
    LED_OBJ.show();
  }
}

// process_command() will interpret the received command and set blade properties appropriately
void process_command() {

  // DEBUG: display decoded command to serial monitor
  #ifdef SERIAL_DEBUG_ENABLE
    Serial.print(F("CMD: 0x"));
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
          Serial.print(F("Refresh blocked! Not in IDLE state; state is 0x"));
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
          Serial.print(F("Refresh blocked! Not in IDLE state; state is 0x"));
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

  #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
    // default all pins to INPUT_PULLUP mode in order to conserve power
    for (uint8_t i = 0; i < NUM_DIGITAL_PINS; i++ ) {
      pinMode(i, INPUT_PULLUP) ;
    }

    // set sleep mode
    #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
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
    led_power_off();
  #endif

  // setup DATA pin for hilt
  pinMode(HILT_DATA_PIN, INPUT_PULLUP);

  // SAMD boards like the Trinket M0 need the ArduinoLowPower library to attach the interrupt to ensure the board wakes from sleep
  #ifdef ARDUINO_ARCH_SAMD
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(HILT_DATA_PIN), read_cmd, CHANGE);

  // for all other Arduino boards
  #else
    attachInterrupt(digitalPinToInterrupt(HILT_DATA_PIN), read_cmd, CHANGE);
  #endif

  // initialize LEDs
  #ifdef LEDLIB_ADAFRUIT_NEOPIXEL
    #ifdef MEGATINYCORE
      pinMode(LED_DATA_PIN, OUTPUT);
    #else
      LED_OBJ.begin();
      #ifdef ADAFRUIT_TRINKET_M0
        dotstar.begin();
        dotstar.fill(RGB_BLADE_OFF);
        dotstar.show();
      #endif
    #endif
  #else 
    #ifdef ADAFRUIT_TRINKET_M0
      FastLED.addLeds<DOTSTAR, INTERNAL_DS_DATA, INTERNAL_DS_CLK, BGR>(&dotstar, 1);
    #endif
    FastLED.addLeds<FASTLED_LED_TYPE, LED_DATA_PIN, FASTLED_RGB_ORDER>(leds, NUM_LEDS);
  #endif
  LED_OBJ.clear();
  LED_OBJ.show();
  delay(100);

  // start the blade in an OFF state
  blade.state = BLADE_OFF;

  // serial output for debug purposes
  #ifdef SERIAL_DEBUG_ENABLE
    Serial.begin(115200);
    Serial.println(F("Ready!"));
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
