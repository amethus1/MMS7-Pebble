#include "layout.h"

// ============================================================================
// Platform Layout Engine Implementation
// ============================================================================
// Every platform is laid out directly in its native pixel grid. Fonts and the
// seven-segment art do not scale, so a scaled 144x168 grid can only ever be
// approximately right on other screens; native tables let each screen be
// tuned by hand. The root-layer origin is still applied so the layout behaves
// correctly inside a non-zero-origin layer.
//
//   Basalt / Diorite : 144x168 rectangle (the classic MSS grid)
//   Chalk            : 180x180 round
//   Emery            : 200x228 rectangle
//   Gabbro           : 260x260 round

// Fixed sizes for the hand-drawn 7-segment digit art
#define DIGIT_LARGE_W 26
#define DIGIT_LARGE_H 41
#define DIGIT_SMALL_W 10
#define DIGIT_SMALL_H 15

static struct {
    int16_t origin_x;
    int16_t origin_y;
    bool initialized;
} s_layout;

#if defined(PBL_PLATFORM_EMERY)
// ----------------------------------------------------------------------------
// Emery: native Pebble Time 2 rectangles (200x228)
// ----------------------------------------------------------------------------
static const GRect s_rects[LAYOUT_COUNT] = {
    [LAYOUT_WEATHER_ICON_DAY]    = { .origin = {10, 20},  .size = {46, 46} },
    [LAYOUT_WEATHER_ICON_MOON]   = { .origin = {10, 29},  .size = {46, 46} },
    [LAYOUT_WEATHER_TEMP]        = { .origin = {111, 20}, .size = {72, 46} },  // Number ends at 183, degree sign to ~192: 8px right margin
    [LAYOUT_WEATHER_TEMP_DEGREE] = { .origin = {184, 24}, .size = {9, 20} },
    [LAYOUT_WEATHER_LOCATION]    = { .origin = {0, 0},    .size = {153, 22} },
    [LAYOUT_WEATHER_LAST_UPDATE] = { .origin = {154, 0},  .size = {46, 22} },
    [LAYOUT_WEATHER_STRING1]     = { .origin = {114, 53}, .size = {78, 22} },  // Right-aligned to the same 8px margin
    [LAYOUT_WEATHER_STRING2]     = { .origin = {114, 72}, .size = {78, 22} },
    [LAYOUT_WEATHER_STRING3]     = { .origin = {3, 71},   .size = {114, 22} },
    [LAYOUT_DATE]                = { .origin = {0, 92},   .size = {200, 38} },  // 6px under the rule, 5px above the digits
    [LAYOUT_CW]                  = { .origin = {83, 181}, .size = {50, 22} },   // Fixed middle slot (centred text); seconds live to its right
    [LAYOUT_BATTERY_TEXT]        = { .origin = {72, 23},  .size = {41, 22} },
    [LAYOUT_BATTERY_TIME]        = { .origin = {67, 45},  .size = {50, 20} },
    [LAYOUT_BATTERY_BOX]         = { .origin = {71, 26},  .size = {44, 18} },
    [LAYOUT_CONNECTION]          = { .origin = {95, 211}, .size = {10, 16} },  // Drawn Bluetooth glyph, centred on x=100
    [LAYOUT_SUNRISE]             = { .origin = {12, 203}, .size = {60, 26} },  // Gothic 24; arrow at 8, text from 12: same margin as the clock
    [LAYOUT_SUNSET]              = { .origin = {132, 203},.size = {60, 26} },  // Right-aligned, ends at 192
    [LAYOUT_TIMEZONE]            = { .origin = {8, 181},  .size = {72, 22} },
    [LAYOUT_HEALTH_ICON]         = { .origin = {8, 184},  .size = {15, 16} },
    [LAYOUT_HEALTH_TEXT]         = { .origin = {26, 181}, .size = {56, 22} },  // Up to 5 digits; the trend arrow follows the measured text
    [LAYOUT_HEALTH_TREND]        = { .origin = {72, 186}, .size = {10, 12} },
    [LAYOUT_COLON_TOP]           = { .origin = {95, 135}, .size = {10, 10} },  // Centred on x=100, symmetric about the digit centre (y=153)
    [LAYOUT_COLON_BOTTOM]        = { .origin = {95, 161}, .size = {10, 10} },
};

static const GRect s_digits[DIGIT_COUNT] = {
    // 34x54 keeps the classic 26:41 digit proportion with 8px side margins
    [DIGIT_H1] = { .origin = {8, 126},   .size = {34, 54} },
    [DIGIT_H2] = { .origin = {52, 126},  .size = {34, 54} },
    [DIGIT_M1] = { .origin = {114, 126}, .size = {34, 54} },
    [DIGIT_M2] = { .origin = {158, 126}, .size = {34, 54} },
    [DIGIT_S1] = { .origin = {159, 184}, .size = {14, 20} },  // Right of the CW slot, ending at the clock's right edge
    [DIGIT_S2] = { .origin = {178, 184}, .size = {14, 20} },
};

static const int16_t s_lines[LINE_COUNT] = {
    [LINE_HEADER_Y]           = 22,
    [LINE_HEADER_SPLIT_X]     = 153,
    [LINE_HEADER_SPLIT_TOP_Y] = 0,
    [LINE_LEFT_X]             = 66,
    [LINE_RIGHT_X]            = 118,
    [LINE_WEATHER_Y]          = 68,
    [LINE_DATE_Y]             = 94,
    [LINE_TIME_TOP_Y]         = 122,
    [LINE_FOOTER_Y]           = 209,
};

static const GRect s_batt_fill = { .origin = {73, 28}, .size = {0, 14} };

static const int16_t s_sunrise_arrow_x = 8;
static const int16_t s_sun_arrow_top_y = 212;
static const int16_t s_sun_arrow_bottom_y = 224;

#elif defined(PBL_PLATFORM_GABBRO)
// ----------------------------------------------------------------------------
// Gabbro: native Pebble Round 2 layout (260x260)
// ----------------------------------------------------------------------------
// Same arrangement as Chalk, with Emery's larger fonts, icon glyphs and
// scalable digits. The bottom cap is ~100px wide at the timezone baseline and
// the top cap ~85px where the Bluetooth label sits.
//
//   y   0- 22  Bluetooth, centred in the top cap
//   y  24- 46  location | last update
//   y  48-113  icon + humidity/pressure | battery | temp, hi/lo, wind
//   y 114-146  date
//   y 150-206  time, centred
//   y 209-231  sunrise | calendar week (or seconds, while shown) | sunset
//   y 233-255  timezone / health, centred in the bottom cap
static const GRect s_rects[LAYOUT_COUNT] = {
    [LAYOUT_WEATHER_ICON_DAY]    = { .origin = {21, 48},  .size = {50, 50} },
    [LAYOUT_WEATHER_ICON_MOON]   = { .origin = {21, 53},  .size = {50, 44} },
    [LAYOUT_WEATHER_TEMP]        = { .origin = {150, 49}, .size = {68, 46} },
    [LAYOUT_WEATHER_TEMP_DEGREE] = { .origin = {219, 53}, .size = {9, 20} },
    [LAYOUT_WEATHER_LOCATION]    = { .origin = {50, 24},  .size = {100, 22} },
    [LAYOUT_WEATHER_LAST_UPDATE] = { .origin = {154, 24}, .size = {50, 22} },
    [LAYOUT_WEATHER_STRING1]     = { .origin = {150, 80}, .size = {68, 22} },  // Hi/Lo
    [LAYOUT_WEATHER_STRING2]     = { .origin = {150, 95}, .size = {68, 22} },  // Wind
    [LAYOUT_WEATHER_STRING3]     = { .origin = {4, 93},   .size = {143, 22} }, // Humidity / pressure
    [LAYOUT_DATE]                = { .origin = {0, 113},  .size = {260, 32} },
    [LAYOUT_CW]                  = { .origin = {100, 209},.size = {60, 22} },
    [LAYOUT_BATTERY_TEXT]        = { .origin = {91, 51},  .size = {41, 22} },
    [LAYOUT_BATTERY_TIME]        = { .origin = {87, 73},  .size = {50, 20} },
    [LAYOUT_BATTERY_BOX]         = { .origin = {90, 54},  .size = {44, 18} },
    [LAYOUT_CONNECTION]          = { .origin = {60, 3},   .size = {140, 22} },
    [LAYOUT_SUNRISE]             = { .origin = {52, 209}, .size = {50, 22} },
    [LAYOUT_SUNSET]              = { .origin = {162, 209},.size = {50, 22} },  // Right-aligned, mirrors the sunrise margin
    [LAYOUT_TIMEZONE]            = { .origin = {55, 233}, .size = {150, 22} },
    [LAYOUT_HEALTH_ICON]         = { .origin = {92, 237}, .size = {15, 14} },
    [LAYOUT_HEALTH_TEXT]         = { .origin = {111, 233},.size = {60, 22} },
    [LAYOUT_HEALTH_TREND]        = { .origin = {158, 236},.size = {10, 12} },
    [LAYOUT_COLON_TOP]           = { .origin = {125, 160},.size = {10, 10} },
    [LAYOUT_COLON_BOTTOM]        = { .origin = {125, 190},.size = {10, 10} },
};

static const GRect s_digits[DIGIT_COUNT] = {
    [DIGIT_H1] = { .origin = {35, 150},  .size = {36, 56} },
    [DIGIT_H2] = { .origin = {81, 150},  .size = {36, 56} },
    [DIGIT_M1] = { .origin = {144, 150}, .size = {36, 56} },
    [DIGIT_M2] = { .origin = {190, 150}, .size = {36, 56} },
    [DIGIT_S1] = { .origin = {114, 210}, .size = {14, 20} },  // Shares the CW slot
    [DIGIT_S2] = { .origin = {133, 210}, .size = {14, 20} },
};

static const int16_t s_lines[LINE_COUNT] = {
    [LINE_HEADER_Y]           = 47,
    [LINE_HEADER_SPLIT_X]     = 152,
    [LINE_HEADER_SPLIT_TOP_Y] = 24,   // Start below the Bluetooth cap
    [LINE_LEFT_X]             = 82,
    [LINE_RIGHT_X]            = 148,
    [LINE_WEATHER_Y]          = 93,   // Under icon + battery, above humidity/pressure
    [LINE_DATE_Y]             = 114,
    [LINE_TIME_TOP_Y]         = 147,
    [LINE_FOOTER_Y]           = 232,
};

static const GRect s_batt_fill = { .origin = {92, 56}, .size = {0, 14} };

static const int16_t s_sunrise_arrow_x = 48;
static const int16_t s_sun_arrow_top_y = 216;
static const int16_t s_sun_arrow_bottom_y = 227;

#elif defined(PBL_ROUND)
// ----------------------------------------------------------------------------
// Chalk: native Pebble Time Round layout (180x180)
// ----------------------------------------------------------------------------
// The circle is only ~60px wide in the top and bottom 14px and ~120px wide at
// the sunrise/sunset row, so the rectangular bands cannot simply be scaled.
// The classic structure is kept, with the small status items redistributed to
// slots that fit inside the chord at their height:
//
//   y   0- 13  Bluetooth, centred in the narrow cap of the circle
//   y  13- 30  location | last update
//   y  31- 75  icon + humidity/pressure | battery | temp, hi/lo, wind
//   y  76-101  date
//   y 103-144  time, centred; the widest usable band
//   y 145-160  sunrise | calendar week (or seconds, while shown) | sunset
//   y 161-177  timezone / health, centred in the bottom cap
static const GRect s_rects[LAYOUT_COUNT] = {
    [LAYOUT_WEATHER_ICON_DAY]    = { .origin = {20, 31},  .size = {36, 36} },
    [LAYOUT_WEATHER_ICON_MOON]   = { .origin = {20, 35},  .size = {36, 33} },
    [LAYOUT_WEATHER_TEMP]        = { .origin = {106, 26}, .size = {54, 30} },
    [LAYOUT_WEATHER_TEMP_DEGREE] = { .origin = {0, 0},    .size = {0, 0} },
    [LAYOUT_WEATHER_LOCATION]    = { .origin = {38, 13},  .size = {74, 16} },
    [LAYOUT_WEATHER_LAST_UPDATE] = { .origin = {113, 13}, .size = {26, 16} },
    [LAYOUT_WEATHER_STRING1]     = { .origin = {106, 51}, .size = {54, 16} },  // Hi/Lo
    [LAYOUT_WEATHER_STRING2]     = { .origin = {106, 62}, .size = {54, 16} },  // Wind
    [LAYOUT_WEATHER_STRING3]     = { .origin = {6, 62},   .size = {97, 16} },  // Humidity / pressure
    [LAYOUT_DATE]                = { .origin = {0, 75},   .size = {180, 26} },
    [LAYOUT_CW]                  = { .origin = {70, 145}, .size = {40, 16} },
    [LAYOUT_BATTERY_TEXT]        = { .origin = {58, 30},  .size = {42, 14} },  // Percentage inside the outline (Gothic 14 glyphs land 2px below the rect top)
    [LAYOUT_BATTERY_TIME]        = { .origin = {56, 47},  .size = {47, 16} },  // Time since last charge
    [LAYOUT_BATTERY_BOX]         = { .origin = {58, 32},  .size = {42, 15} },  // Same size as the original MSS7 art
    [LAYOUT_CONNECTION]          = { .origin = {40, 0},   .size = {100, 16} },
    [LAYOUT_SUNRISE]             = { .origin = {38, 145}, .size = {34, 16} },
    [LAYOUT_SUNSET]              = { .origin = {113, 145},.size = {33, 16} },  // Right-aligned, mirrors the sunrise margin
    [LAYOUT_TIMEZONE]            = { .origin = {45, 161}, .size = {90, 16} },
    [LAYOUT_HEALTH_ICON]         = { .origin = {62, 162}, .size = {15, 14} },
    [LAYOUT_HEALTH_TEXT]         = { .origin = {79, 161}, .size = {40, 16} },
    [LAYOUT_HEALTH_TREND]        = { .origin = {114, 163},.size = {8, 10} },
    [LAYOUT_COLON_TOP]           = { .origin = {88, 111}, .size = {7, 7} },
    [LAYOUT_COLON_BOTTOM]        = { .origin = {88, 133}, .size = {7, 7} },
};

static const GRect s_digits[DIGIT_COUNT] = {
    [DIGIT_H1] = { .origin = {24, 103},  .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_H2] = { .origin = {57, 103},  .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_M1] = { .origin = {100, 103}, .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_M2] = { .origin = {131, 103}, .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_S1] = { .origin = {79, 146},  .size = {DIGIT_SMALL_W, DIGIT_SMALL_H} },  // Shares the CW slot
    [DIGIT_S2] = { .origin = {92, 146},  .size = {DIGIT_SMALL_W, DIGIT_SMALL_H} },
};

static const int16_t s_lines[LINE_COUNT] = {
    [LINE_HEADER_Y]           = 30,
    [LINE_HEADER_SPLIT_X]     = 112,
    [LINE_HEADER_SPLIT_TOP_Y] = 14,   // Start below the Bluetooth cap
    [LINE_LEFT_X]             = 56,
    [LINE_RIGHT_X]            = 103,
    [LINE_WEATHER_Y]          = 63,   // Under icon + battery, above humidity/pressure
    [LINE_DATE_Y]             = 76,
    [LINE_TIME_TOP_Y]         = 102,
    [LINE_FOOTER_Y]           = 161,
};

static const GRect s_batt_fill = { .origin = {60, 34}, .size = {0, 11} };

static const int16_t s_sunrise_arrow_x = 34;
static const int16_t s_sun_arrow_top_y = 150;
static const int16_t s_sun_arrow_bottom_y = 158;

#else
// ----------------------------------------------------------------------------
// Basalt / Diorite: the classic 144x168 MSS grid
// ----------------------------------------------------------------------------
static const GRect s_rects[LAYOUT_COUNT] = {
    [LAYOUT_WEATHER_ICON_DAY]    = { .origin = {4, 15},   .size = {33, 33} },
    [LAYOUT_WEATHER_ICON_MOON]   = { .origin = {4, 21},   .size = {33, 33} },
    [LAYOUT_WEATHER_TEMP]        = { .origin = {50, 10},  .size = {94, 30} },
    [LAYOUT_WEATHER_TEMP_DEGREE] = { .origin = {0, 0},    .size = {0, 0} },
    [LAYOUT_WEATHER_LOCATION]    = { .origin = {0, 0},    .size = {110, 16} },
    [LAYOUT_WEATHER_LAST_UPDATE] = { .origin = {111, 0},  .size = {33, 16} },
    [LAYOUT_WEATHER_STRING1]     = { .origin = {88, 36},  .size = {55, 14} },  // Right row 1: Hi/Lo
    [LAYOUT_WEATHER_STRING2]     = { .origin = {88, 50},  .size = {55, 14} },  // Right row 2: Wind
    [LAYOUT_WEATHER_STRING3]     = { .origin = {1, 50},   .size = {87, 14} },  // Left row 1 + row 2 (combined)
    [LAYOUT_DATE]                = { .origin = {0, 63},   .size = {144, 30} },
    [LAYOUT_CW]                  = { .origin = {72, 135}, .size = {64, 20} },
    [LAYOUT_BATTERY_TEXT]        = { .origin = {43, 17},  .size = {42, 14} },  // Percentage inside the outline (Gothic 14 glyphs land 2px below the rect top)
    [LAYOUT_BATTERY_TIME]        = { .origin = {40, 34},  .size = {48, 16} },  // Time since last charge
    [LAYOUT_BATTERY_BOX]         = { .origin = {43, 19},  .size = {42, 15} },  // Same size as the original MSS7 art
    [LAYOUT_CONNECTION]          = { .origin = {44, 152}, .size = {56, 20} },
    [LAYOUT_SUNRISE]             = { .origin = {7, 152},  .size = {50, 16} },
    [LAYOUT_SUNSET]              = { .origin = {104, 152},.size = {37, 16} },  // Right-aligned, mirrors the sunrise margin
    [LAYOUT_TIMEZONE]            = { .origin = {5, 132},  .size = {100, 20} },
    [LAYOUT_HEALTH_ICON]         = { .origin = {0, 137},  .size = {15, 14} },
    [LAYOUT_HEALTH_TEXT]         = { .origin = {24, 132}, .size = {100, 20} },
    [LAYOUT_HEALTH_TREND]        = { .origin = {14, 140}, .size = {10, 10} },
    [LAYOUT_COLON_TOP]           = { .origin = {69, 102}, .size = {7, 7} },
    [LAYOUT_COLON_BOTTOM]        = { .origin = {69, 124}, .size = {7, 7} },
};

static const GRect s_digits[DIGIT_COUNT] = {
    [DIGIT_H1] = { .origin = {4, 94},   .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_H2] = { .origin = {37, 94},  .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_M1] = { .origin = {80, 94},  .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_M2] = { .origin = {111, 94}, .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_S1] = { .origin = {113, 137},.size = {DIGIT_SMALL_W, DIGIT_SMALL_H} },
    [DIGIT_S2] = { .origin = {126, 137},.size = {DIGIT_SMALL_W, DIGIT_SMALL_H} },
};

static const int16_t s_lines[LINE_COUNT] = {
    [LINE_HEADER_Y]           = 16,
    [LINE_HEADER_SPLIT_X]     = 110,
    [LINE_HEADER_SPLIT_TOP_Y] = 0,
    [LINE_LEFT_X]             = 40,
    [LINE_RIGHT_X]            = 88,
    [LINE_WEATHER_Y]          = 50,
    [LINE_DATE_Y]             = 69,
    [LINE_TIME_TOP_Y]         = 90,
    [LINE_FOOTER_Y]           = 154,
};

static const GRect s_batt_fill = { .origin = {45, 21}, .size = {0, 11} };

static const int16_t s_sunrise_arrow_x = 3;
static const int16_t s_sun_arrow_top_y = 157;
static const int16_t s_sun_arrow_bottom_y = 165;
#endif

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

void layout_init(GRect bounds) {
    s_layout.origin_x = bounds.origin.x;
    s_layout.origin_y = bounds.origin.y;
    s_layout.initialized = true;
}

static GRect offset_rect(const GRect* base) {
    return GRect(
        s_layout.origin_x + base->origin.x,
        s_layout.origin_y + base->origin.y,
        base->size.w,
        base->size.h
    );
}

GRect layout_get_rect(LayoutId id) {
    if (!s_layout.initialized || id >= LAYOUT_COUNT) {
        return GRectZero;
    }
    return offset_rect(&s_rects[id]);
}

GRect layout_get_time_digit_rect(TimeDigitId id) {
    if (!s_layout.initialized || id >= DIGIT_COUNT) {
        return GRectZero;
    }
    return offset_rect(&s_digits[id]);
}

int16_t layout_get_line_coord(LineId id) {
    if (!s_layout.initialized || id >= LINE_COUNT) {
        return 0;
    }
    bool is_x = (id == LINE_HEADER_SPLIT_X || id == LINE_LEFT_X || id == LINE_RIGHT_X);
    return (is_x ? s_layout.origin_x : s_layout.origin_y) + s_lines[id];
}

GRect layout_get_battery_fill_rect(int16_t fill_width) {
    if (!s_layout.initialized) {
        return GRectZero;
    }
    return GRect(
        s_layout.origin_x + s_batt_fill.origin.x,
        s_layout.origin_y + s_batt_fill.origin.y,
        fill_width,
        s_batt_fill.size.h
    );
}

GPoint layout_get_sunrise_arrow_top(void) {
    return GPoint(s_layout.origin_x + s_sunrise_arrow_x, s_layout.origin_y + s_sun_arrow_top_y);
}

GPoint layout_get_sunrise_arrow_bottom(void) {
    return GPoint(s_layout.origin_x + s_sunrise_arrow_x, s_layout.origin_y + s_sun_arrow_bottom_y);
}

