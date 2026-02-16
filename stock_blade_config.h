/* stock_blade_config.h
 */
#pragma once

#include "hardware.h"

// to save program space store lightsaber blade timings 
// as 8-bit values instead of 16-bit values
#ifdef SPACE_SAVER
  #define TIME_ENCODE(t)  ( uint8_t)((t & 0x3FF) >> 2)
  #define TIME_DECODE(t)  (uint16_t)(t << 2)
  typedef uint8_t blade_timing_t;
#else
  #define TIME_ENCODE(t)  (uint16_t)t
  #define TIME_DECODE(t)  (uint16_t)t
  typedef uint16_t blade_timing_t;
#endif

// how many lightsabers per lightsaber table
#define LIGHTSABER_TABLE_LEN    16

// stock lightsaber properties template
typedef struct {
  uint8_t color_index;
  blade_timing_t ignition_time;
  blade_timing_t extinguish_time_delay;
  blade_timing_t extinguish_time;
} stock_lightsaber_t;

extern const stock_lightsaber_t savi_lightsaber[LIGHTSABER_TABLE_LEN];
extern const stock_lightsaber_t legacy_lightsaber[LIGHTSABER_TABLE_LEN];