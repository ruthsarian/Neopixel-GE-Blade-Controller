/* blade.cpp
 */
#include "blade.h"
#include "hilt_cmd.h"
#include "stock_blade_config.h"
#include "blade_color_table.h"
#include "config.h"
#include "effects.h"

// global blade properties object
blade_t blade;

// blade_manager() takes care of changing the colors of the blade
//
// if you're looking to add custom blade behaviors then this function is likely
// where you want to make your changes.
void blade_manager() {
  static uint32_t next_step = 0;
  static uint32_t animate_step = 0;
  static uint32_t last_extinguish = 0;
  static blade_state_t last_state = BLADE_UNINITIALIZED;
  static uint8_t wheel_index = 0;
  uint16_t target;
  bool update_blade = false;

  //
  // TODO: DONT_SHOW mod, make update_blade static and unset it if SHOW_LEDS() is called successfully.
  //

  //
  // ** BLADE MANAGER STAGE ZERO : CHECK FOR AND PROCESS INCOMING COMMAND **
  //
  // hilt_cmd is a global variable that is populated with a non-zero value when a new command
  // is received. after we detect a new hilt_cmd value is present, record it locally then 
  // reset this variable back to 0 so we know we've processed it.
  //
  if (hilt_cmd != 0) {

    // copy command to a local variable as hilt_cmd is a global, volatile
    // variable that could be overwritten at any second
    blade.cmd = hilt_cmd;

    // immediately set hilt_cmd to zero so we know we've processed it
    hilt_cmd = 0;

    // call function that processes the command received from the hilt
    blade_process_command();
  }

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
        SHOW_LEDS();    // unnecessary since the next step is to cut power to the LEDs
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

            case COLOR_MODE_WHEEL_CYCLE_WHITE:
              #ifdef SERIAL_DEBUG_ENABLE
                Serial.println(F("New Color Mode: COLOR_MODE_WHEEL_HOLD"));
              #endif

              blade.color_mode = COLOR_MODE_WHEEL_HOLD_WHITE;
              break;

            case COLOR_MODE_WHEEL_HOLD:
            case COLOR_MODE_WHEEL_HOLD_WHITE:
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

          // white blade
          case COLOR_MODE_WHEEL_CYCLE_WHITE:
          case COLOR_MODE_WHEEL_HOLD_WHITE:
            blade.color = RGB_BLADE_WHITE;
            blade.color_clash = RGB_BLADE_CLASH_YELLOW;
            break;

          // in all other instances, use the stock blade color
          default:
            blade.color = blade_color_table[blade.lightsaber->color_index][INDEX_COLOR_TABLE_COLOR];
            blade.color_clash = blade_color_table[blade.lightsaber->color_index][INDEX_COLOR_TABLE_CLASH];
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
        update_blade = true;

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
          case COLOR_MODE_WHEEL_CYCLE:
          case COLOR_MODE_WHEEL_CYCLE_WHITE:

            // at this point, it's possible we're entering an IDLE state after a blade refresh, 
            // in which case we don't want to touch next_step; only set next_step if it has a 
            // value less than the current time in ms
            if (next_step < millis()) {
              next_step = millis() + COLOR_WHEEL_PAUSE_TIME;

              // if we're a white blade (triggered by a clash) hold the color a little longer;
              if (blade.color_mode == COLOR_MODE_WHEEL_CYCLE_WHITE) {
                next_step += COLOR_WHEEL_PAUSE_TIME;
              }
            }
            break;

          default:
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
        update_blade = true;

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

        // set the blade color; this is needed for color-changing legacy hilts like Cal and Ahsoka
        if (blade.color_mode == COLOR_MODE_STOCK) {
            blade.color = blade_color_table[blade.lightsaber->color_index][INDEX_COLOR_TABLE_COLOR];
            blade.color_clash = blade_color_table[blade.lightsaber->color_index][INDEX_COLOR_TABLE_CLASH];
        }

        // connect LED battery power
        #ifdef LED_PWR_SWITCH_PIN
          led_power_on();
        #endif

        // set the blade color
        LED_FILL(blade.color);

        // set brightness to max
        LED_OBJ.setBrightness(MAX_BRIGHTNESS);
        update_blade = true;

        // change state to on
        blade.state = BLADE_ON;
        break;

      // low brightness flicker command; set the blade to some brightness level between 0 and 50% based on the value supplied
      case BLADE_FLICKER_LOW:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_FLICKER_LOW"));
        #endif

        LED_OBJ.setBrightness((uint8_t)(((float)(blade.cmd & 0x0F)/0x0F) * (MAX_BRIGHTNESS >> 1)));
        update_blade = true;
        next_step = millis() + 40;
        break;

      // high brightness flicker command; set the blade to some brightness level between 50 and 100% based on the value supplied
      case BLADE_FLICKER_HIGH:
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("Blade State Change: BLADE_FLICKER_HIGH"));
        #endif

        LED_OBJ.setBrightness((uint8_t)((1 + (float)(blade.cmd & 0x0F)/0x0F) * (MAX_BRIGHTNESS >> 1)));
        update_blade = true;
        next_step = millis() + 40;
        break;

      default:
        break;
    }
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


    /* THOUGHTS

       could we break this into two sections. maybe one sets color and another sets which LEDs are on? 
         this would mean we'd be processing every LED in the strand even if only 1 LED is going to be shown.
         isn't that a waste of processing power?
         what if we did the which LEDs are on first? create a variable? then subsequent variables know which LEDs
         to work on? ... but .. intermittent/pattern off/on wouldn't work. blech.

        what if we created a blade_effect object? and a blade_effects array? where each entry in the array corresponds
        to an enum?

        and each blade effect is broken into parts
          onStart
          onGoing

        do we separate color and brightness effects? we probably do.

        do we chain effects? 

        would effects need to know if they're a 'base' or 'modifier' effect? e.g. the base effect sets the colors then modifiers
        can read the existing color? hmm...

     */


    switch (blade.state) {

      // animate the blade igniting by turning on 1 LED at a time
      case BLADE_IGNITING:

        // calculate how much time has elapsed since ignition began and calculate how many LEDs should
        // be ignited at this point.
        if ((next_step + TIME_DECODE(blade.lightsaber->ignition_time)) <= millis()) {
          target = TARGET_MAX;
        } else {

          // calculate how many LEDs should be ON at this point in the ignition sequence
          target = ((millis() - next_step) * TARGET_MAX) / TIME_DECODE(blade.lightsaber->ignition_time);

          // do not allow target to go above TARGET_MAX
          if (target > TARGET_MAX) {
              target = TARGET_MAX;
          }
        }

        if (animate_step < target) {
          update_blade = true;

          // if the LED_FILL_N macro exists (Adafruit NeoPixel) use that to help speed things up
          // by filling multiple pixels in one step rather than incrementing through each pixel
          // individually using LED_SET_PIXEL
          #ifdef LED_FILL_N

            // the fill command, if given a count value of 0, will fill the entire blade
            // so make sure we have at least 1 LED to fill before using that fill command
            if ((target - animate_step) > 0) {
              LED_FILL_N(blade.color, animate_step, target - animate_step);

              // if using MIRROR_MODE, start ignting from the end of the stip towards the center
              #ifdef MIRROR_MODE
                LED_FILL_N(blade.color, NUM_LEDS - target, target - animate_step);
              #endif

              // update animate_step
              animate_step = target;
            }
          #else

            // increment through each pixel in this block and set its color
            while (animate_step < target) {
              LED_SET_PIXEL(animate_step, blade.color);

              #ifdef MIRROR_MODE
                LED_SET_PIXEL((NUM_LEDS - 1) - animate_step, blade.color);
              #endif
              animate_step++;
            }
          #endif
        }

        // have we reached the end of the strip of LEDs?
        if (target >= TARGET_MAX) {
          next_step = 0;
          blade.state = BLADE_ON;
        }
        break;

      // second part of the clash animation; set the blade back to its normal color
      case BLADE_CLASH:

        // clash during color cycle momentarily select white
        if (blade.color_mode == COLOR_MODE_WHEEL_CYCLE) {

          blade.color_mode = COLOR_MODE_WHEEL_CYCLE_WHITE;
          blade.color = RGB_BLADE_WHITE;
          blade.color_clash = RGB_BLADE_CLASH_YELLOW;

        // if we're already in white for the wheel cycle, exit
        } else if (blade.color_mode == COLOR_MODE_WHEEL_CYCLE_WHITE) {

          blade.color = color_by_wheel(wheel_index);
          blade.color_clash = RGB_BLADE_CLASH_WHITE;
          blade.color_mode = COLOR_MODE_WHEEL_CYCLE;          
        }
        next_step = 0;
        LED_FILL(blade.color);
        blade.state = BLADE_IDLE;
        break;

      // animate the blade extinguishing by turning off 1 LED at a time
      case BLADE_EXTINGUISHING:

        // how many LEDs should be extinguished at this point in time?
        if ((next_step + TIME_DECODE(blade.lightsaber->extinguish_time)) <= millis()) {
          target = TARGET_MAX;
        } else {
          target = ((millis() - next_step) * TARGET_MAX) / TIME_DECODE(blade.lightsaber->extinguish_time);
          if (target > TARGET_MAX) {
              target = TARGET_MAX;
          }
        }

        // are there LEDs to turn off at this time?
        if (animate_step < target) {

          // target = how many LEDs in total should be off
          // aniamte_step = how many LEDs have already been turned off
          //
          // target - animate_step = how many LEDs we need to turn off this time

          // since target - animate > 0 we can assume the blade will need updating as we're turning off LEDs
          update_blade = true;

          // LED_FILL_N is only in Adafruit library; shame, it's a useful function.
          #ifdef LED_FILL_N
            if ((target - animate_step) > 0) {

              // MIRROR_MODE means the strip of pixels is folded back on itself to make up the blade.
              #ifdef MIRROR_MODE
                // in mirror mode, find the middle, subtract how many LEDs to turn off, then double how many we turn off to clear the middle of the LED strip
                LED_FILL_N(RGB_BLADE_OFF, TARGET_MAX - target, target * 2);
              #else
                // otherwise, count back from the tip how many LEDs we need to turn off
                // we can say only turn off target - animate_step here because everything closer to the tip should already be off
                // maybe this saves a few cycles, but we don't have such savings in mirror mode
                // would it be more readable to not use this little trick?
                LED_FILL_N(RGB_BLADE_OFF, NUM_LEDS - target, target - animate_step);
              #endif
              animate_step = target;
            }

          // without LED_FILL_N we'll just turn them off one pixel at a time
          #else
            while (animate_step < target) {
              LED_SET_PIXEL(TARGET_MAX - (animate_step + 1), RGB_BLADE_OFF);    // it took me far far too long to realize i needed +1 here
              #ifdef MIRROR_MODE
                LED_SET_PIXEL(TARGET_MAX + animate_step, RGB_BLADE_OFF);
              #endif
              animate_step++;
            }
          #endif
        }

        if (animate_step >= TARGET_MAX) {
          next_step = 0;
          blade.state = BLADE_OFF;
        }
        break;

      // second part of the flicker, reduce brightness by 20%
      case BLADE_FLICKER_LOW:
      case BLADE_FLICKER_HIGH:
        LED_OBJ.setBrightness((uint8_t)((float)LED_OBJ.getBrightness() * .8));
        update_blade = true;
        next_step = 0;
        blade.state = BLADE_IDLE;
        break;

      // we should only get here if the blade has been off long enough that it's time to sleep
      case BLADE_OFF:

        // put the blade to sleep to convserve power
        // don't do that if we're debugging as some devices will drop the serial connection
        #ifndef SERIAL_DEBUG_ENABLE
          hardware_sleep();
        #endif

        // after waking up, reset sleep timer in case blade never leaves the off state
        next_step = millis() + SLEEP_AFTER;
        break;

      // blade is at idle
      case BLADE_IDLE:

        switch (blade.color_mode) {

          // we've held white long enough...
          case COLOR_MODE_WHEEL_CYCLE_WHITE:
            blade.color_mode = COLOR_MODE_WHEEL_CYCLE;
            blade.color_clash = RGB_BLADE_CLASH_WHITE;

          // if in the color wheel cycle mode
          case COLOR_MODE_WHEEL_CYCLE:

            // increment the wheel
            wheel_index += COLOR_WHEEL_CYCLE_STEP;

            #ifdef SERIAL_DEBUG_ENABLE
              Serial.print(F("Next Color: "));
              Serial.println(wheel_index);
            #endif

            // set the new color by wheel
            blade.color = color_by_wheel(wheel_index);
            LED_FILL(blade.color);
            update_blade = true;
            next_step = millis() + COLOR_WHEEL_PAUSE_TIME;
            break;

          default:
            break;
        }
        break;

      default:
        break;
    }

  }

  // update the LED strip with any changes
  if (update_blade) {

    // next_step contains the timestamp, in milliseconds, when the blade began ignition
    //
    // if using the Adafruit NeoPixel library, adjust next_step to take into account the time lost
    // due to the library blocking the increment of millis() during its operations
    #ifdef ADAFRUIT_ADJUST
      switch (blade.state) {
          case BLADE_IGNITING:
          case BLADE_EXTINGUISHING: 
            next_step -= ADAFRUIT_ADJUST;
            break;
          default:
            break;
      }
    #endif

    SHOW_LEDS();
  }
}


// blade_process_command() will interpret the received command and set blade properties appropriately
void blade_process_command() {

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

void blade_setup() {
  // start the blade in an OFF state
  blade.state = BLADE_OFF;

  // set initial blade effects to stock
  blade.current_color_effect = 0;
  blade.current_brightness_effect = 0;
}