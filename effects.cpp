/* effects.cpp
 */
#include "effects.h"

effect_interface_t *color_effects[REGISTRY_MAX];
effect_interface_t *brightness_effects[REGISTRY_MAX];

uint8_t color_effects_len = 0;
uint8_t brightness_effects_len = 0;

void register_effects() {

  // the order in which effects are registered is the order in which they will be presented
  // when configuring the blade

  // should we make a separate register_effects.cpp file and that's the file others can modify to add their own effects?
  // then we could register the stock/root effects here



}

bool register_effect(effect_interface_t *effect_interface, effect_class_t effect_class) {

  // should there be some code here to validate the interface? e.g. that functions are defined?

  switch(effect_class) {
    case EFFECT_CLASS_COLOR:
      if (color_effects_len >= REGISTRY_MAX) {
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("ERROR: register_effect() color effects registry full"));
        #endif
        return false;
        color_effects[color_effects_len] = effect_interface;
        color_effects_len++;
      }
      break;
    case EFFECT_CLASS_BRIGHTNESS:
      if (brightness_effects_len >= REGISTRY_MAX) {
        #ifdef SERIAL_DEBUG_ENABLE
          Serial.println(F("ERROR: register_effect() brightness effects registry full"));
        #endif
        return false;
        brightness_effects[brightness_effects_len] = effect_interface;
        brightness_effects_len++;
      }
      break;
    default:
      #ifdef SERIAL_DEBUG_ENABLE
        Serial.println(F("ERROR: register_effect() invalid effect class"));
      #endif
      return false;
      break;
  }
  return true;
}


// generate an RGB color based on an 8-bit input value
LED_RGB_TYPE color_by_wheel(uint8_t wheel) {
  wheel = 255 - wheel;
  if (wheel < 85) {
    return(LED_RGB(255 - wheel * 3, 0, wheel * 3));
  } else if (wheel < 170) {
    wheel -= 85;
    return(LED_RGB(0, wheel * 3, 255 - wheel * 3));
  } else {
    wheel -= 170;
    return(LED_RGB(wheel * 3, 255 - wheel * 3, 0));
  }
}

