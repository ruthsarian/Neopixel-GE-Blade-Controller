/* hilt_cmd.cpp
 */
#include "hilt_cmd.h"
#include "config.h"
#include "hardware.h"

// global variable where decoded hilt command is stored
#ifdef USE_AVR_EV_CAPT
  uint8_t hilt_cmd = 0;
#else
  // volatile because it will be updated from within an interrupt service routine
  volatile uint8_t hilt_cmd = 0;
#endif

// read data from the hilt
// data is sent in a series of pulses. each pulse represents a 1-bit value (0 or 1)
// how long the pulse lasts will determine if it's a 1 or a 0
// collect 8 pulses in a row in a single, 8-bit value and store that to the global hilt_cmd variable
void read_cmd() {
  static uint8_t cmd = 0;
  static uint8_t bPos = 0;
  static uint32_t last_change = 0;
  uint32_t period = 0;

#ifdef USE_AVR_EV_CAPT

  if (TCB0.INTFLAGS & TCB_CAPT_bm) {

    // record the time we're reading a new bit
    last_change = micros();

    // record the length of period
    period = TCB0.CCMP;

#else

  // when transitioning from HIGH to LOW record the time the data line was HIGH
  // record the bit value that corresponds to the the length of time the data line was HIGH
  if (digitalRead(HILT_DATA_PIN) == LOW) {

    // determine the length of the the data line was high
    period = micros() - last_change;

#endif

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

#ifdef USE_DONT_SHOW
    if (bPos != 0 && !dont_show) {
      dont_show = true;
    } else if (bPos == 0 && dont_show) {
      dont_show = false;
    }
#endif

  } else {


// TODO: separate dont_show out from USE_AVR_EV_CAPT
//       this must be done if we want to use DONT_SHOT with other MCUs
#ifdef USE_AVR_EV_CAPT
      // reset if we have a partial command and more than 4 times a valid bit cutoff period have passed
      if (dont_show && micros() - last_change > VALID_BIT_CUTOFF * 4) {
        dont_show = false;
        bPos = 0;
        cmd = 0;
      }
#else
      // just transitioned into LOW (ACTIVE) period, record the time when it started
      last_change = micros();
#endif

  }
}

void cmd_capture_setup() {
  // setup DATA pin for hilt
  pinMode(HILT_DATA_PIN, INPUT_PULLUP);

  // setup event system and timer for hilt data capture
  #ifdef USE_AVR_EV_CAPT
    EVSYS.SYNCCH0 = 0x0A;           // set PC3 as generator for syncrhonous channel
    EVSYS.ASYNCUSER0 = 0x01;        // set TCB0 as a user of synchronous channel

    TCB0.EVCTRL = TCB_FILTER_bm     // enable noise filter
                | TCB_CAPTEI_bm;    // enable event input capture

    TCB0.CTRLB = TCB_CNTMODE2_bm;   // set input capture mode to pulse-width measurement

    TCB0.CTRLA = TCB_CLKSEL0_bm     // enable prescaler (CLK_PER/2), gives us more time to capture events
              | TCB_ENABLE_bm;      // enable TCB0

  // SAMD boards like the Trinket M0 need the ArduinoLowPower library to attach the interrupt to ensure the board wakes from sleep
  #elif defined(ARDUINO_ARCH_SAMD)
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(HILT_DATA_PIN), read_cmd, CHANGE);

  // for all other Arduino boards
  #else
    attachInterrupt(digitalPinToInterrupt(HILT_DATA_PIN), read_cmd, CHANGE);
  #endif
}

#ifdef ENABLE_DEMO
void cmd_demo() {
  static uint32_t next_action = 0;
  static uint8_t cmds_idx = 0;
  //static uint8_t cmds[] = {0x80, 0x20, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0x40};    // reset, reset, power on white, red, orange, yellow, green, cyan, blue, purple, power off
  static uint8_t cmds[] = {0x80, 0x20, 0x40, 0x21, 0x41, 0x22, 0x42, 0x23, 0x43, 0x24, 0x44, 0x25, 0x45, 0x26, 0x46, 0x27, 0x47 };  
  static uint8_t cmds_len = sizeof(cmds) / sizeof(uint8_t);

  // cmds[] is a list of command values to send to the hilt. commands are sent in order, once every 5 seconds. the sequence will repeat forever.
  // galaxy's edge hilt protocol is explained on the hilt & blade communications tab of the galaxy's edge research spreadsheet located here:
  // https://docs.google.com/spreadsheets/d/13P_GE6tNYpGvoVUTEQvA3SQzMqpZ-SoiWaTNoJoTV9Q/edit?gid=1478016256

  // is it time to perform the next action?
  if (next_action < millis()) {

    #ifdef SERIAL_DEBUG_ENABLE
      if (cmds_idx == 0) {
        Serial.println(F("\n\n** (RE)STARTING DEMO **\n\n"));
      }
    #endif

    hilt_cmd = cmds[cmds_idx++];
    next_action = millis() + 5000;

    // reset back to the start of the command list
    if (cmds_idx >= cmds_len) {
      cmds_idx = 0;
    }
  }
}
#endif

// this is a template function for reference only and is never actually called by the code
// you can make a copy of this function and modify it to meet your needs
void cmd_process_template(uint8_t cmd) {

  // DEBUG: display decoded command to serial monitor
  #ifdef SERIAL_DEBUG_ENABLE
    Serial.print(F("CMD: 0x"));
    Serial.println(cmd, HEX);
  #endif

  // identify command
  switch (cmd & 0xF0) {

    case 0x20:  // savi's ignite
      break;

    case 0x30:  // legacy ignite
      break;

    case 0xA0:  // savi's set color
      break;

    case 0xB0:  // legacy set color
      break;

    case 0x40:  // savi's extinguish
    case 0x50:  // legacy extinguish
      break;

    case 0x80:  // off
    case 0x90:  // off
    case 0xE0:  // off
    case 0xF0:  // off
      break;

    case 0xC0:  // savi's clash
    case 0xD0:  // legacy clash
      break;

    case 0x60:  // blade flicker low (0-50% brightness)
      break;

    case 0x70:  // blade flicker high (53-100% brightness)
      break;

    default:
      break;
  }
}