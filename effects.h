/* effects.h
 */
#pragma once

#include <stdint.h>
#include "hardware.h"

#define REGISTRY_MAX          10

typedef struct {
  void (*init)();           // Called when effect becomes active
  uint16_t (*update)();     // Called after init() the first time, then after N milliseconds where N is a value returned by update()
  void (*onClash)();        // Optional: some effects may react
  void (*nextParam)();      // Move to next parameter
  void (*incParam)();       // Increment current parameter
  uint8_t (*paramCount)();  // How many parameters this effect has
} effect_interface_t;

typedef enum {
  EFFECT_CLASS_COLOR,
  EFFECT_CLASS_BRIGHTNESS
} effect_class_t;

extern effect_interface_t *color_effects[];
extern effect_interface_t *brightness_effects[];
extern uint8_t color_effects_len;
extern uint8_t brightness_effects_len;

bool register_effect(effect_interface_t*, effect_class_t);
void register_effects();
LED_RGB_TYPE color_by_wheel(uint8_t wheel);