/* hilt_cmd.cpp
 *
 * data is sent from the hilt to the blade via a data line.
 * the data line will change states between a logic LOW (0) and a logic HIGH (1).
 * the length of a pulse (logic LOW) is used to determine the bit value of the pulse.
 * commands are transmitted as 1 byte (8 bit) values. 8 pulses = 1 command
 *
 * an interrupt service routine (ISR) is used to detect when the logic level on the
 * data line changes and then to record the length of the pulse.
 *
 * read_cmd() then picks up the pulse length values and interprets them as commands.
 */
#include "hilt_cmd.h"
#include "config.h"
#include "hardware.h"

// global variable where decoded hilt command is stored
uint8_t hilt_cmd = 0;

// global variable where pulse period is stored by the data ISR and then
// picked up by read_cmd()
volatile uint32_t cmd_pulse_period = 0;

// ISR responsible for determining the length of a pulse on the data line. 
// TinyAVRs will use their event system while all other MCUs will use a more
// generic approach.
#ifdef USE_AVR_EV_CAPT 
  ISR(TCB0_INT_vect) {
    cmd_pulse_period = TCB0.CCMP; // reading CCMP should also clear the interrupt flag
  }
#else
  void data_pin_interrupt() {
    static uint32_t last_change = 0;

    // on transition from a logic LOW to HIGH record the pulse length
    if (digitalRead(HILT_DATA_PIN) == HIGH) {
      cmd_pulse_period = micros() - last_change;
    }
    last_change = micros();
  }
#endif

//
// DONT_SHOW protects us from missing bits once we start recording a command, but we're
// still susceptible to losing bits at the start of a command.
//
// if we have 2 (or more) pulses arrive before read_cmd() is executed we lose everything
// but the last recorded cmd_pulse_period.
//
// if it is going to happen it'll be while SHOW_LEDS() is running as during which that
// time interrupts are disabled. how long does SHOW_LEDS() take? about 30uS per LED. a 
// string of 50 would be about 1.5ms; a string of 250 would be about 7.5ms. 
//
// a single bit cycle (HIGH then LOW) will be 2.4ms. this is less than that 7.5ms case
// which means we could miss up to 3 bits at the start of a command. 
//
// my solution is to enable dont_show when a preamble is detected. every command is
// preceeded by a preamble of a LOW, HIGH, LOW sequence of 16.4ms each. if we enable
// don't show at that point, we should catch the first preamble even if SHOW_LEDS()
// starts just as it arrives.
//
// the only downside? maybe we miss a SHOW_LEDS() or two, but the blade will pickup
// the changes after the command is read and i don't think it'll be noticeable to us.
//


// read_cmd() is called continuously. it checks if a new bit period value has been
// recorded and, if so, processes it until we have a full command. that command
// value is stored in the global hilt_cmd which is then picked up by blade_manager()
// via a call to blade_process_command()
void read_cmd() {
  static uint8_t cmd = 0;
  static uint8_t bPos = 0;
  static uint32_t last_pulse_time = 0;
  uint32_t period = 0;

  // grab the current pulse period value. this is done while interrupts are disabled in order
  // to prevent any data loss from an interrupt triggering between these two operations.
  noInterrupts();
    period = cmd_pulse_period;
    cmd_pulse_period = 0;
  interrupts();

  // if there's a new pulse detected
  if (period > 0) {

    // record the time
    last_pulse_time = micros();

    // if period was less than 4ms treat it as a good bit value
    if (period < VALID_BIT_CUTOFF) {

      // shift the bits in the cmd variable left by 1 to make room for the new bit
      cmd <<= 1;

      // count that another bit has been received
      bPos++;

      // a zero bit is 2.4ms long; a one bit is 1.2ms long.
      // VALID_BIT_SPLIT is set to 1600 by default, meaning anything over is a 0, anything under is a 1

      // a LOW period greater than 2ms can be treated as a 0, otherwise it is a 0
      // analysis of GE hilts shows that typically 1200-1600uS = 0, 2400-3000uS = 1
      // this value may need tweaking; see DEFINE near top of code
      if (period < VALID_BIT_SPLIT) {      
        cmd++;
      }

      // if more than 7 bits have been recorded (8 bits), we have a good 8-bit command.
      if (bPos > 7) {

        // store the 8-bit command to a global variable which will be picked up by loop() function
        hilt_cmd = cmd;

        // disable dont_show
        #ifdef USE_DONT_SHOW
          dont_show = false;
        #endif

        // reset bit collection
        cmd = 0;
        bPos = 0;
      } else {

        // enable dont_show while reading a command
        #ifdef USE_DONT_SHOW
          dont_show = true;
        #endif
      }

    // assume anything longer than 4ms indicates the start of a new command; reset bit collection
    // this also covers us if there is a data collection problem and we missed an edge when measuring a pulse
    } else {
      cmd = 0;
      bPos = 0;

      // we've got an interesting situation here. we've received A pulse of some kind. either it's noise
      // or it could be part of a preamble. if it is a preamble then the real command is about to start.
      // enabling dont_show NOW should protect us from missing this command later under certain circumstances
      //
      // so check the value of period. if it's less than (VALID_BIT_SPLIT * 10) then let's assume it's a 
      // preamble pulse and enable dont_show. if we're wrong, it's fine, we skip an SHOW_LEDS() or two
      // and dont_show is unset later via the timeout check below
      //
      // why VALID_BIT_SPLIT * 10? typical preamble is around 16.4ms or 16400us. VALID_BIT_SPLIT * 10
      // will be 18750 (assuming you haven't changed that in config.h) which is 2ms longer than the expected
      // preamble length. this seemed like a good enough value to use and derriving it from an existing 
      // value rather than adding yet another seems like the way to go.
      #ifdef USE_DONT_SHOW
        if (period < VALID_BIT_SPLIT * 10) {
          dont_show = true;
        }
      #endif
    }
  }

  // unset dont_show if it's set and it's been more than 10 times the value of VALID_BIT_SPLIT since
  // we last saw a pulse; this is the same amount of time we consider the upper limit on a preamble
  #ifdef USE_DONT_SHOW
    if (dont_show && micros() - last_pulse_time > VALID_BIT_SPLIT * 10) {
      dont_show = false;
      bPos = 0;
      cmd = 0;
    }
  #endif

  // don't we need to reset regardless of if dont_show is set?
  // no. why? because each command is preceeded by a preamble which will cause our command capture
  // variables to reset themselves anyways. we do however need to worry about dont_show if 
  // its enabled as the blade will just stop updating and that's no good.

}

void cmd_capture_setup() {
  // setup DATA pin for hilt
  pinMode(HILT_DATA_PIN, INPUT_PULLUP);

  // setup event system and timer for hilt data capture
  #ifdef USE_AVR_EV_CAPT
    EVSYS.SYNCCH0 = 0x0A;           // set PC3 as generator for syncrhonous channel
    EVSYS.ASYNCUSER0 = 0x01;        // set TCB0 as a user of synchronous channel

    TCB0.EVCTRL = TCB_FILTER_bm     // enable noise filter
                | TCB_CAPTEI_bm     // enable event input capture
                | TCB_EDGE_bm;      // counter starts on falling edge, capture and interrupt on rising edge (idle HIGH)

    TCB0.CTRLB = TCB_CNTMODE2_bm;   // set input capture mode to pulse-width measurement

    TCB0.CTRLA = TCB_CLKSEL0_bm     // enable prescaler (CLK_PER/2), gives us more time to capture events
              | TCB_ENABLE_bm;      // enable TCB0

  // SAMD boards like the Trinket M0 need the ArduinoLowPower library to attach the interrupt to ensure the board wakes from sleep
  #elif defined(ARDUINO_ARCH_SAMD)
    LowPower.attachInterruptWakeup(digitalPinToInterrupt(HILT_DATA_PIN), data_pin_interrupt, CHANGE);

  // for all other Arduino boards
  #else
    attachInterrupt(digitalPinToInterrupt(HILT_DATA_PIN), data_pin_interrupt, CHANGE);
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