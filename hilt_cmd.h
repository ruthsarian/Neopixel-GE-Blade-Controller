/* hilt_cmd.h
 */
#pragma once

#include <stdint.h>

// global variable where decoded hilt command is stored
extern uint8_t hilt_cmd;

// global variable where ISR records last data pulse length
extern volatile uint32_t cmd_pulse_period;

// determine whether to use clock ticks or microseconds to determine valid bit 
// values from the hilt
//
// why am i not just defining VALID_BUT_CUTOFF ... 
//
#ifdef USE_AVR_EV_CAPT
  #define VALID_BIT_CUTOFF  VALID_BIT_CUTOFF_IN_TICKS
  #define VALID_BIT_SPLIT   VALID_BIT_SPLIT_IN_TICKS
#else
  #define VALID_BIT_CUTOFF  VALID_BIT_CUTOFF_IN_US
  #define VALID_BIT_SPLIT   VALID_BIT_SPLIT_IN_US
#endif

#ifndef USE_AVR_EV_CAPT 
  void data_pin_interrupt();
#endif

void cmd_capture_setup();
void read_cmd();
void cmd_demo();