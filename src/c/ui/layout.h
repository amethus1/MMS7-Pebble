#pragma once
#include <pebble.h>

// ============================================================================
// Platform Layout Engine
// ============================================================================
// Every platform has its own hand-tuned native table (144x168 rectangle,
// 180x180 round, 200x228 Emery). Fonts and digit art do not scale, so a scaled
// grid was never quite right on the other screens.

// Emery and Gabbro have roughly twice the pixels of the classic screens; the
// layers pick larger fonts and resources on them.
#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
#define LAYOUT_LARGE_DISPLAY 1
#endif

// Emery's refined layout: a drawn Bluetooth glyph instead of the word, a
// two-tone grid without the rule above the clock, the calendar week in a
// fixed middle slot, and the health trend arrow placed after the measured
// step count (Emery) or the whole health group centred as one measured
// unit (Gabbro).
#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
#define LAYOUT_REFINED_STATUS 1
#endif

// ----------------------------------------------------------------------------
// Layout IDs for UI elements
// ----------------------------------------------------------------------------
typedef enum {
    LAYOUT_WEATHER_ICON_DAY,
    LAYOUT_WEATHER_ICON_MOON,
    LAYOUT_WEATHER_TEMP,
    LAYOUT_WEATHER_TEMP_DEGREE,
    LAYOUT_WEATHER_LOCATION,
    LAYOUT_WEATHER_LAST_UPDATE,
    LAYOUT_WEATHER_STRING1,
    LAYOUT_WEATHER_STRING2,
    LAYOUT_WEATHER_STRING3,
    LAYOUT_DATE,
    LAYOUT_CW,
    LAYOUT_BATTERY_TEXT,
    LAYOUT_BATTERY_TIME,
    LAYOUT_BATTERY_BOX,
    LAYOUT_CONNECTION,
    LAYOUT_SUNRISE,
    LAYOUT_SUNSET,
    LAYOUT_TIMEZONE,
    LAYOUT_HEALTH_ICON,
    LAYOUT_HEALTH_TEXT,
    LAYOUT_HEALTH_TREND,
    LAYOUT_COLON_TOP,
    LAYOUT_COLON_BOTTOM,
    LAYOUT_COUNT  // Must be last
} LayoutId;

// Time digit positions
typedef enum {
    DIGIT_H1,  // Hours tens
    DIGIT_H2,  // Hours units
    DIGIT_M1,  // Minutes tens
    DIGIT_M2,  // Minutes units
    DIGIT_S1,  // Seconds tens
    DIGIT_S2,  // Seconds units
    DIGIT_COUNT
} TimeDigitId;

// Line/separator Y positions
typedef enum {
    LINE_HEADER_Y,
    LINE_HEADER_SPLIT_X,
    LINE_HEADER_SPLIT_TOP_Y,  // Where the header split line starts (0 unless something sits above the header)
    LINE_LEFT_X,
    LINE_RIGHT_X,
    LINE_WEATHER_Y,
    LINE_DATE_Y,
    LINE_TIME_TOP_Y,
    LINE_FOOTER_Y,
    LINE_COUNT
} LineId;

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

// Initialize layout engine with screen bounds. Call once in main_window_load.
void layout_init(GRect bounds);

// Get the platform-specific rectangle for a UI element
GRect layout_get_rect(LayoutId id);

// Get the platform-specific rectangle for a time digit
GRect layout_get_time_digit_rect(TimeDigitId id);

// Get a platform-specific separator coordinate
int16_t layout_get_line_coord(LineId id);

// Maximum native fill width that keeps the fill inside LAYOUT_BATTERY_BOX.
#if defined(LAYOUT_LARGE_DISPLAY)
#define LAYOUT_BATTERY_FILL_MAX_W 38
#else
#define LAYOUT_BATTERY_FILL_MAX_W 37
#endif

// Get the battery fill rectangle. Width is 0..LAYOUT_BATTERY_FILL_MAX_W.
GRect layout_get_battery_fill_rect(int16_t fill_width);

// Sun arrow row: the sunrise arrow's x, and the top / bottom y shared by both
// arrows. The sunset arrow's x is measured from its right-aligned text at runtime.
GPoint layout_get_sunrise_arrow_top(void);
GPoint layout_get_sunrise_arrow_bottom(void);
