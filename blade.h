/* blade.h
 */
#pragma once

#include "stock_blade_config.h"

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
} blade_state_t;

// color mode -- DELETE ME LATER
typedef enum {
  COLOR_MODE_STOCK,
  COLOR_MODE_WHEEL_CYCLE,
  COLOR_MODE_WHEEL_CYCLE_WHITE,
  COLOR_MODE_WHEEL_HOLD,
  COLOR_MODE_WHEEL_HOLD_WHITE
} blade_color_mode_t;

// blade properties template
typedef struct {
  blade_state_t state;
  uint8_t cmd;
  const stock_lightsaber_t *lightsaber;
  LED_RGB_TYPE color;
  LED_RGB_TYPE color_clash;
  uint8_t current_color_effect;
  uint8_t current_brightness_effect;
  blade_color_mode_t color_mode;        // DELETE ME LATER
} blade_t;

void blade_setup();
void blade_manager();
void blade_process_command();
