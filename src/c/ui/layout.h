#pragma once
#include <pebble.h>

// ============================================================================
// Dynamic Layout Engine
// ============================================================================
// Replaces hardcoded X_OFFSET/Y_OFFSET with runtime-calculated positions.
// Uses a canonical 144x168 design grid, scaled to fit any screen with
// proper safe-area handling for round displays.

// ----------------------------------------------------------------------------
// Layout IDs for UI elements
// ----------------------------------------------------------------------------
typedef enum {
    LAYOUT_WEATHER_ICON_DAY,
    LAYOUT_WEATHER_ICON_MOON,
    LAYOUT_WEATHER_TEMP,
    LAYOUT_WEATHER_LOCATION,
    LAYOUT_WEATHER_LAST_UPDATE,
    LAYOUT_WEATHER_STRING1,
    LAYOUT_WEATHER_STRING2,
    LAYOUT_WEATHER_STRING3,
    LAYOUT_DATE,
    LAYOUT_CW,
    LAYOUT_BATTERY_WIDGET,
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

// Time digit positions (fixed size, scaled position)
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

// Get scaled rectangle for a UI element
GRect layout_get_rect(LayoutId id);

// Get scaled rectangle for a time digit (fixed size, scaled position)
GRect layout_get_time_digit_rect(TimeDigitId id);

// Get scaled Y coordinate for separator lines
int16_t layout_get_line_coord(LineId id);

// Get scaled battery fill rect (width is canonical 0-38, will be scaled)
GRect layout_get_battery_fill_rect(int16_t fill_width_canonical);

// Get sun arrow points (sunrise/sunset indicators)
GPoint layout_get_sunrise_arrow_top(void);
GPoint layout_get_sunrise_arrow_bottom(void);
GPoint layout_get_sunset_arrow_top(void);
GPoint layout_get_sunset_arrow_bottom(void);
