#pragma once
#include <stdint.h>

// The "Custom" colour scheme: the user picks a colour for each part of the
// face on the settings page. Kept free of Pebble types so the settings code
// and its tests can share it.

// Index of the custom scheme in KEY_SET_INVERT_COLOR, after the built-in ones
#define COLOR_PROFILE_CUSTOM 16

// The parts that take a colour. This is also the order they are stored in;
// add new ones at the end.
typedef enum {
    CUSTOM_COLOR_BACKGROUND = 0,
    CUSTOM_COLOR_CLOCK,         // Digits and colon
    CUSTOM_COLOR_DATE,          // Date line, week label, heart rate
    CUSTOM_COLOR_WEATHER,       // Temperature (unless coloured by temperature) and the detail lines
    CUSTOM_COLOR_ICON,          // Moon, and the weather icon when it is not coloured by conditions
    CUSTOM_COLOR_HEADER_TEXT,   // Location and weather age
    CUSTOM_COLOR_HEADER_BG,
    CUSTOM_COLOR_SUN,           // Sunrise / sunset times and arrows
    CUSTOM_COLOR_EXTRA,         // Steps, sleep, timezone
    CUSTOM_COLOR_BLUETOOTH,
    CUSTOM_COLOR_BATTERY,       // Outline and percentage
    CUSTOM_COLOR_LINES,         // Grid lines
    CUSTOM_COLOR_COUNT
} CustomColor;

// 0xRRGGBB defaults: the "White on black, with colours" scheme, so choosing
// Custom starts from a readable face. Must match the settings page (config.js).
#define CUSTOM_COLOR_DEFAULTS { \
    0x000000, /* background  */ \
    0xFFFFFF, /* clock       */ \
    0xFFFFFF, /* date        */ \
    0xFFFFFF, /* weather     */ \
    0xFFFFFF, /* icon        */ \
    0xFFFFFF, /* header text */ \
    0x000000, /* header bg   */ \
    0xFFFF00, /* sun         */ \
    0xFFFFFF, /* extra       */ \
    0x00AAFF, /* bluetooth   */ \
    0xFFFFFF, /* battery     */ \
    0xAAAAAA  /* lines       */ \
}
