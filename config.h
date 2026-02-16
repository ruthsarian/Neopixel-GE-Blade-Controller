/* config.h
 *
 *
 * // defines I use with my top secret custom blade project
 * #define ADAFRUIT_LED_TYPE       NEO_RGB
 * #define NUM_LEDS                79
 * #define MAX_BRIGHTNESS          160
 * #define HILT_DATA_PIN           PIN_PC3
 * #define LED_DATA_PIN            PIN_PA2
 * #define LED_PWR_SWITCH_PIN      PIN_PA5
 * #define LATCH_DELAY_US          280
 *
 * // default defines that this code ships with
 * #define ADAFRUIT_LED_TYPE       NEO_GRB+NEO_KHZ800
 * #define NUM_LEDS                144
 * #define MAX_BRIGHTNESS          64
 * #define HILT_DATA_PIN           2
 * #define LED_DATA_PIN            4                      // trinket M0 supports DMA on pin 4, see https://learn.adafruit.com/dma-driven-neopixels
 * #define LED_PWR_SWITCH_PIN      0
 * #define LATCH_DELAY_US          50
 */
#pragma once

//
// What pixels do you have: RGB or GRB?
// 
// Some addressable RGB LEDs use GRB pixel ordering, others RGB. 
// Depending on YOUR addressable RGB LEDs, you may need to modify ADAFRUIT_LED_TYPE and/or FASTLED_RGB_ORDER from "RGB" to "GRB".
// Experiment. Ignite with a red crystal or a green crystal. Does it ignite the other color? Then you need to change your color order!
//

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
//#define SERIAL_DEBUG_ENABLE           // enable debug messages over serial
#define VALID_BIT_CUTOFF_IN_US  4000    // any HIGH period on the data line longer than this value, in microseconds, is considered an invalid bit of data and causes a reset of the data capture
#define VALID_BIT_ONE_IN_US     1600    // any HIGH period longer than this value, in microseconds, but less than VALID_BIT_CUTOFF is treated as a valid 1 bit
                                        // any HIGH period shorter than this value, in microseconds, is treated as a valid 0 bit
                                        // if blade is not registering commands correctly, this value likely needs to be tweaked
                                        // typically a 1 bit is about 2000uS (Legacy) or 2400uS (Savi) long and a 0 is 1200uS long. 
                                        // this value is set between 2000 and 1200 to accomadate delays in timing/processing
#define COLOR_MODE_CHANGE_TIME  1500    // if a blade is turned off then on again within this amount of time, then change to the next color mode
#define COLOR_WHEEL_PAUSE_TIME  2000    // how long to hold a color before moving to the next color
#define COLOR_WHEEL_CYCLE_STEP  16      // how many steps to jump when calculating the next color in the color cycle; a power of 2 is recommended
//#define USE_ADAFRUIT_NEOPIXEL         // uncomment to use the Adafruit NeoPixel library instead of FastLED
//#define ENABLE_DEMO                   // define this to enable a demo program which will run instead of reading commands from the hilt.
                                        // i use this to test the blade without having to connect it to a hilt, just need to provide power and ground to the blade
#define LATCH_DELAY_US          50      // define the length of delay, in microseconds, your RGB LEDs need in order to latch; default is 50 but mine need 280
                                        // used only with tinyNeoPixel (for now)
