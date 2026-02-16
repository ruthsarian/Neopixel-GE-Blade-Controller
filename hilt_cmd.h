/* hilt_cmd.h
 */
#pragma once

#include <stdint.h>

// global variable where decoded hilt command is stored
#ifdef USE_AVR_EV_CAPT
  extern int8_t hilt_cmd;
#else
  extern volatile uint8_t hilt_cmd;
#endif

// determine whether to use clock ticks or microseconds to determine valid bit 
// values from the hilt
#ifdef USE_AVR_EV_CAPT
  #define VALID_BIT_CUTOFF  VALID_BIT_CUTOFF_IN_TICKS
  #define VALID_BIT_ONE     VALID_BIT_ONE_IN_TICKS
#else
  #define VALID_BIT_CUTOFF  VALID_BIT_CUTOFF_IN_US
  #define VALID_BIT_ONE     VALID_BIT_ONE_IN_US
#endif

void cmd_capture_setup();
void cmd_demo();