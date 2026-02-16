/* effects.cpp
 */
#include "effects.h"

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