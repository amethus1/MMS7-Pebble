#pragma once
#include <stdbool.h>

// Small display rules kept free of Pebble dependencies so they can be unit tested.

// Moon-phase modes (KEY_SET_MOON_PHASE)
#define MOON_MODE_NIGHT          0   // Moon at night
#define MOON_MODE_ALWAYS         1
#define MOON_MODE_NEVER          2
#define MOON_MODE_NIGHT_IF_CLEAR 3   // Moon at night, unless it is raining or snowing

// Climacons glyphs for rain, drizzle, showers, sleet, hail, snow and lightning.
bool rules_icon_is_precipitation(int icon_code);

// Whether the moon replaces the weather icon right now. A stale icon is not
// trusted to veto the moon.
bool rules_show_moon(int moon_mode, bool is_night, bool weather_is_current, int icon_code);

// Auto health mode: last night's sleep in the morning until sleep_until_hour,
// steps for the rest of the day.
bool rules_show_sleep(int hour_of_day, int sleep_until_hour);
