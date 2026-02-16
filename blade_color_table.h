/* blade_color_table.h
 */
#pragma once

#include "hardware.h"

// stock blade colors; these values are based on PWM duty cycles measured from a stock blade controller
#define RGB_BLADE_WHITE         LED_RGB(112, 112, 112)
#define RGB_BLADE_RED           LED_RGB(255,   0,   0)
#define RGB_BLADE_ORANGE        LED_RGB(255, 102,   0)
#define RGB_BLADE_YELLOW        LED_RGB(152, 152,   0)
#define RGB_BLADE_GREEN         LED_RGB(  0, 255,   0)
#define RGB_BLADE_CYAN          LED_RGB(  0, 152, 152)
#define RGB_BLADE_BLUE          LED_RGB(  0,   0, 255)
#define RGB_BLADE_PURPLE        LED_RGB(152,   0, 152)
#define RGB_BLADE_DARK_PURPLE   LED_RGB( 16,   0,  32)
#define RGB_BLADE_ORANGE_RED    LED_RGB(192,  32,   0)
#define RGB_BLADE_CLASH_WHITE   LED_RGB(128, 128, 128)
#define RGB_BLADE_CLASH_YELLOW  LED_RGB(255, 255,   0)
#define RGB_BLADE_CLASH_ORANGE  LED_RGB(255,  64,   0)
#define RGB_BLADE_OFF           LED_RGB(  0,   0,   0)

// color table index
#define INDEX_BLADE_WHITE       0
#define INDEX_BLADE_RED         1
#define INDEX_BLADE_ORANGE      2
#define INDEX_BLADE_YELLOW      3
#define INDEX_BLADE_GREEN       4
#define INDEX_BLADE_CYAN        5
#define INDEX_BLADE_BLUE        6
#define INDEX_BLADE_PURPLE      7
#define INDEX_BLADE_DARK_PURPLE 8
#define INDEX_BLADE_ORANGE_RED  9
#define INDEX_COLOR_TABLE_COLOR 0
#define INDEX_COLOR_TABLE_CLASH 1

// blade_color_table will contain all known stock blade colors and their corresponding clash colors
// as clash color seems to be tied to blade color and not hilt.
extern const LED_RGB_TYPE blade_color_table[][2];
