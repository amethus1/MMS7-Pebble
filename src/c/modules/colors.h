#pragma once
#include <pebble.h>

typedef struct {
  GColor clock;
  GColor seconds;
  GColor timezone;
  GColor steps;
  GColor calendar_week;
  GColor clock_bg;
  
  GColor date;
  GColor date_bg;
  
  GColor weather;
  GColor weather_bg;
  
  GColor moon;
  GColor moon_bg;
  
  GColor sun;
  GColor connection;
  GColor status_bg;
  
  GColor location;
  GColor location_bg;
  
  GColor last_update;
  GColor last_update_bg;
  
  GColor lines_bg;
  
  // Helpers for Battery colors
  GColor battery_text;
  GColor battery_bg;
  GColor battery_fill;  // Fill bar color (should contrast with battery_text)
} ColorScheme;

// Maximum number of static profiles defined
#define MAX_COLOR_PROFILES 15

// Initialize color schemes (needed because C99 doesn't support complex static initializers for GColor)
void colors_init(void);

// Get a pointer to the color scheme for the given index
const ColorScheme* colors_get_scheme(int index);

// Helper to get color for weather icon based on current profile and icon code
void colors_get_weather_icon_colors(int profile_index, int icon_code, bool is_night, GColor *fg, GColor *bg);
