/* hardware.cpp
 */
#include "hardware.h"
#include "config.h"

#if defined(MEGATINYCORE) || defined(USE_ADAFRUIT_NEOPIXEL)
  #ifdef MEGATINYCORE
    byte LED_OBJ_array[NUM_LEDS * 3];
    tinyNeoPixel LED_OBJ = tinyNeoPixel(NUM_LEDS, LED_DATA_PIN, ADAFRUIT_LED_TYPE, LED_OBJ_array);
  #else
    Adafruit_NeoPixel LED_OBJ = Adafruit_NeoPixel(NUM_LEDS, LED_DATA_PIN, ADAFRUIT_LED_TYPE);
    #ifdef ADAFRUIT_TRINKET_M0
      Adafruit_DotStar dotstar = Adafruit_DotStar(1, INTERNAL_DS_DATA, INTERNAL_DS_CLK, DOTSTAR_BGR);
    #endif
  #endif
#else

  // Trinket M0 users also need a DotStar object defined to turn off the on-board DotStar LED.
  #ifdef ADAFRUIT_TRINKET_M0
    LED_RGB_TYPE dotstar;       // Trinket M0 dotstar LED
  #endif
  LED_RGB_TYPE leds[NUM_LEDS];  // blade LEDs
#endif

#ifdef USE_DONT_SHOW
  bool dont_show = false;
#endif

// switch off power to the LEDs
void led_power_off() {
  #ifdef SERIAL_DEBUG_ENABLE
    //Serial.println(F("led_power_off()"));
  #endif
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
  #ifdef SERIAL_DEBUG_ENABLE
    //Serial.println(F("led_power_on()"));
  #endif
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

// setup the hardware for the device
void harware_setup() {

  // setup AVR microcontrollers
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

  // initialize LEDs
  #ifdef MEGATINYCORE
    pinMode(LED_DATA_PIN, OUTPUT);
    LED_OBJ.updateLatch(LATCH_DELAY_US);
  #elif defined(USE_ADAFRUIT_NEOPIXEL)
    LED_OBJ.begin();
    #ifdef ADAFRUIT_TRINKET_M0
      dotstar.begin();
      dotstar.fill(RGB_BLADE_OFF);
      dotstar.show();
    #endif
  #else
    #ifdef ADAFRUIT_TRINKET_M0
      FastLED.addLeds<DOTSTAR, INTERNAL_DS_DATA, INTERNAL_DS_CLK, BGR>(&dotstar, 1);
    #endif
    FastLED.addLeds<FASTLED_LED_TYPE, LED_DATA_PIN, FASTLED_RGB_ORDER>(leds, NUM_LEDS);
  #endif
  LED_OBJ.clear();
  SHOW_LEDS();
  
  // serial output for debug purposes
  #ifdef SERIAL_DEBUG_ENABLE
    Serial.begin(115200);
  #endif
}

// empty ISR to call when waking from sleep
#ifdef USE_AVR_EV_CAPT
  void wakeISR() { }
#endif

// put the hardware to sleep
void hardware_sleep() {

  // for AVR devices we have some work to do  
  #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)

    // allow the MCU to go to sleep
    sleep_enable();

    // megaTinyCore has BOD configured in the board settings, not here.
    #ifndef ARDUINO_ARCH_MEGAAVR
      sleep_bod_disable();
      power_all_disable();
    #endif

    // if not already using an interrupt, i have to attach an interrupt so the mcu wakes up
    #ifdef USE_AVR_EV_CAPT
      attachInterrupt(digitalPinToInterrupt(HILT_DATA_PIN), wakeISR, CHANGE);
    #endif  

    sleep_cpu();                  // put the MCU to sleep

    #ifdef USE_AVR_EV_CAPT
      detachInterrupt(digitalPinToInterrupt(HILT_DATA_PIN));
    #endif

    #ifndef ARDUINO_ARCH_MEGAAVR
      power_all_enable();
    #endif

    sleep_disable();

  // for SAMD devices we can just use the Arduino Low Power library to handle everything
  #elif defined ARDUINO_ARCH_SAMD

    LowPower.sleep();
  #endif
}