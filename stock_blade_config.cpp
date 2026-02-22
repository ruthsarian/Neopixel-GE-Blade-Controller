/* stock_blade_config.cpp
 *
 * these timing values were determined from data recorded by a logic analyzer. 
 * they may not be exact, but are pretty close.
 *
 * we care about the ignition and extinguish times because we need our ignition
 * and extinguish animation effects to match with what the hilt expects. otherwise
 * the animations may not align with the hilt's sound effects.
 *
 */
#include "stock_blade_config.h"
#include "blade_color_table.h"

// savi's workshop lightsaber properties
// timing values were taken from logic analyzer captures of a stock V2 blade controller
const stock_lightsaber_t savi_lightsaber[LIGHTSABER_TABLE_LEN] = {
//  COLOR,                    IGNITION_TIME,    EXTINGUISH_TIME_DELAY,  EXTINGUISH_TIME
  { INDEX_BLADE_WHITE,        TIME_ENCODE(280), TIME_ENCODE(370),       TIME_ENCODE(470)}, //  white kyber crystal
  { INDEX_BLADE_RED,          TIME_ENCODE(280), TIME_ENCODE(940),       TIME_ENCODE(470)}, //    red kyber crystal
  { INDEX_BLADE_ORANGE,       TIME_ENCODE(280), TIME_ENCODE(000),       TIME_ENCODE(275)}, // orange kyber crystal
  { INDEX_BLADE_YELLOW,       TIME_ENCODE(280), TIME_ENCODE(000),       TIME_ENCODE(275)}, // yellow kyber crystal
  { INDEX_BLADE_GREEN,        TIME_ENCODE(280), TIME_ENCODE(180),       TIME_ENCODE(470)}, //  green kyber crystal
  { INDEX_BLADE_CYAN,         TIME_ENCODE(280), TIME_ENCODE(275),       TIME_ENCODE(565)}, //   cyan kyber crystal
  { INDEX_BLADE_BLUE,         TIME_ENCODE(280), TIME_ENCODE(275),       TIME_ENCODE(565)}, //   blue kyber crystal
  { INDEX_BLADE_PURPLE,       TIME_ENCODE(280), TIME_ENCODE(370),       TIME_ENCODE(660)}, // purple kyber crystal
  { INDEX_BLADE_DARK_PURPLE,  TIME_ENCODE(280), TIME_ENCODE(945),       TIME_ENCODE(470)}, // unused; blades bundled with legacy hilts produce ORANGE instead
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { INDEX_BLADE_YELLOW,       TIME_ENCODE(240), TIME_ENCODE(000),       TIME_ENCODE(240)}, //
  { INDEX_BLADE_GREEN,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, //
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(415)}, //
  { INDEX_BLADE_BLUE,         TIME_ENCODE(240), TIME_ENCODE(245),       TIME_ENCODE(500)}, //
  { INDEX_BLADE_PURPLE,       TIME_ENCODE(240), TIME_ENCODE(330),       TIME_ENCODE(545)}  //
};

// legacy lightsaber properties
// timing values were taken from logic analyzer captures of a stock V2 blade controller
const stock_lightsaber_t legacy_lightsaber[LIGHTSABER_TABLE_LEN] = {
//  COLOR,                    IGNITION_TIME,    EXTINGUISH_TIME_DELAY,  EXTINGUISH_TIME
  { INDEX_BLADE_YELLOW,       TIME_ENCODE(240), TIME_ENCODE(000),       TIME_ENCODE(325)}, // Temple Guard
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(830),       TIME_ENCODE(335)}, // Kylo Ren
  { INDEX_BLADE_BLUE,         TIME_ENCODE(240), TIME_ENCODE(500),       TIME_ENCODE(415)}, // Rey (Anakin), Rey Reforged, Ahsoka (CW)
  { INDEX_BLADE_PURPLE,       TIME_ENCODE(240), TIME_ENCODE(490),       TIME_ENCODE(585)}, // Mace Windu
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(660),       TIME_ENCODE(335)}, // Asajj Ventress
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, // Ahsoka (Rebels)
  { INDEX_BLADE_GREEN,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}, // Luke
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(500),       TIME_ENCODE(500)}, // Darth Vader
  { INDEX_BLADE_RED,          TIME_ENCODE(240), TIME_ENCODE(750),       TIME_ENCODE(330)}, // Darth Maul
  { INDEX_BLADE_BLUE,         TIME_ENCODE(240), TIME_ENCODE(495),       TIME_ENCODE(415)}, // Obi-Wan, Ben Solo
  { INDEX_BLADE_ORANGE_RED,   TIME_ENCODE(240), TIME_ENCODE(175),       TIME_ENCODE(450)}, // Baylan Skoll / Shin Hati
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)},
  { INDEX_BLADE_WHITE,        TIME_ENCODE(240), TIME_ENCODE(160),       TIME_ENCODE(415)}
};
