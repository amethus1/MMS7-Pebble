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

// Week-number types (KEY_SET_WEEK_TYPE)
#define WEEK_TYPE_CALENDAR      0   // ISO 8601 calendar week
#define WEEK_TYPE_FISCAL_DATE   1   // Counted from a month and day the user picks
#define WEEK_TYPE_FISCAL_RETAIL 2   // Retail calendar, see below

// Week of the retail fiscal year, whose years end on the Saturday closest to
// January 31 - so each one starts on the Sunday nearest February 1 and weeks
// run Sunday to Saturday. Most years have 52 weeks, some 53.
// year is the full year, yday counts from 0 on January 1, wday from 0 on
// Sunday (struct tm's tm_year + 1900, tm_yday and tm_wday).
int rules_retail_fiscal_week(int year, int yday, int wday);
