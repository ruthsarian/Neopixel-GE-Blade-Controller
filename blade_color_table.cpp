/* blade_color_table.cpp
 */
#include "blade_color_table.h"

// all possible stock blade colors and their corresponding clash color
const LED_RGB_TYPE blade_color_table[][2] = {
  {RGB_BLADE_WHITE,        RGB_BLADE_CLASH_YELLOW},     // WHITE
  {RGB_BLADE_RED,          RGB_BLADE_CLASH_ORANGE},     // RED
  {RGB_BLADE_ORANGE,       RGB_BLADE_CLASH_WHITE},      // ORANGE
  {RGB_BLADE_YELLOW,       RGB_BLADE_CLASH_WHITE},      // YELLOW
  {RGB_BLADE_GREEN,        RGB_BLADE_CLASH_YELLOW},     // GREEN
  {RGB_BLADE_CYAN,         RGB_BLADE_CLASH_YELLOW},     // CYAN
  {RGB_BLADE_BLUE,         RGB_BLADE_CLASH_YELLOW},     // BLUE
  {RGB_BLADE_PURPLE,       RGB_BLADE_CLASH_YELLOW},     // PURPLE
  {RGB_BLADE_DARK_PURPLE,  RGB_BLADE_CLASH_ORANGE},     // DARK PURPLE  ; a color stock blades provide, but is never used
  {RGB_BLADE_ORANGE_RED,   RGB_BLADE_CLASH_YELLOW}      // ORANGE RED   ; used by the Baylan Skill & Shin Hati limited edition legacy hilts
};
