#include "layout.h"

// ============================================================================
// Dynamic Layout Engine Implementation
// ============================================================================

// Canonical design grid (144x168 = Aplite/Basalt/Diorite rectangular)
#define CANONICAL_WIDTH  144
#define CANONICAL_HEIGHT 168

// Fixed sizes for 7-segment digits (not scaled to preserve crispness)
#define DIGIT_LARGE_W 26
#define DIGIT_LARGE_H 41
#define DIGIT_SMALL_W 10
#define DIGIT_SMALL_H 15

// ----------------------------------------------------------------------------
// Static state
// ----------------------------------------------------------------------------
static struct {
    GRect bounds;           // Full screen bounds
    GRect safe_bounds;      // Usable area (inset for round)
    int16_t scale_1000;     // Uniform scale factor * 1000 (fixed-point)
    int16_t origin_x;       // X offset to center scaled content
    int16_t origin_y;       // Y offset to center scaled content
    bool initialized;
} s_layout;

// ----------------------------------------------------------------------------
// Canonical rectangles (144x168 grid) - from original layout.h
// ----------------------------------------------------------------------------
static const GRect s_canonical_rects[LAYOUT_COUNT] = {
    [LAYOUT_WEATHER_ICON_DAY]    = { .origin = {7, 15},   .size = {33, 33} },
    [LAYOUT_WEATHER_ICON_MOON]   = { .origin = {7, 21},   .size = {33, 33} },
    [LAYOUT_WEATHER_TEMP]        = { .origin = {50, 10},  .size = {94, 30} },
    [LAYOUT_WEATHER_LOCATION]    = { .origin = {0, -1},   .size = {110, 17} },
    [LAYOUT_WEATHER_LAST_UPDATE] = { .origin = {111, -1}, .size = {33, 17} },
    [LAYOUT_WEATHER_STRING1]     = { .origin = {85, 36},  .size = {58, 14} },  // Right row 1: Hi/Lo
    [LAYOUT_WEATHER_STRING2]     = { .origin = {85, 50},  .size = {58, 14} },  // Right row 2: Wind
    [LAYOUT_WEATHER_STRING3]     = { .origin = {2, 50},   .size = {82, 14} },  // Left row 1 + row 2 (combined), padded from edge
    [LAYOUT_DATE]                = { .origin = {0, 63},   .size = {144, 30} },
    [LAYOUT_CW]                  = { .origin = {72, 135}, .size = {64, 20} },
    [LAYOUT_BATTERY_WIDGET]      = { .origin = {48, 17},  .size = {47, 33} },  // Current layout placement, old battery visuals
    [LAYOUT_BATTERY_TEXT]        = { .origin = {50, 17},  .size = {45, 35} },
    [LAYOUT_BATTERY_TIME]        = { .origin = {50, 32},  .size = {45, 20} },  // Reserved for compatibility
    [LAYOUT_BATTERY_BOX]         = { .origin = {49, 19},  .size = {43, 15} },
    [LAYOUT_CONNECTION]          = { .origin = {47, 152}, .size = {50, 34} },
    [LAYOUT_SUNRISE]             = { .origin = {7, 152},  .size = {50, 30} },
    [LAYOUT_SUNSET]              = { .origin = {110, 152},.size = {50, 30} },
    [LAYOUT_TIMEZONE]            = { .origin = {5, 132},  .size = {100, 20} },
    [LAYOUT_HEALTH_ICON]         = { .origin = {0, 137},  .size = {15, 14} },
    [LAYOUT_HEALTH_TEXT]         = { .origin = {24, 132}, .size = {100, 20} },
    [LAYOUT_HEALTH_TREND]        = { .origin = {14, 140}, .size = {10, 10} },
    [LAYOUT_COLON_TOP]           = { .origin = {69, 102}, .size = {7, 7} },
    [LAYOUT_COLON_BOTTOM]        = { .origin = {69, 124}, .size = {7, 7} },
};

// Canonical time digit positions
static const GRect s_canonical_digits[DIGIT_COUNT] = {
    [DIGIT_H1] = { .origin = {4, 94},   .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_H2] = { .origin = {37, 94},  .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_M1] = { .origin = {80, 94},  .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_M2] = { .origin = {111, 94}, .size = {DIGIT_LARGE_W, DIGIT_LARGE_H} },
    [DIGIT_S1] = { .origin = {113, 137},.size = {DIGIT_SMALL_W, DIGIT_SMALL_H} },
    [DIGIT_S2] = { .origin = {126, 137},.size = {DIGIT_SMALL_W, DIGIT_SMALL_H} },
};

// Canonical line coordinates
static const int16_t s_canonical_lines[LINE_COUNT] = {
    [LINE_HEADER_Y]       = 16,
    [LINE_HEADER_SPLIT_X] = 110,
    [LINE_LEFT_X]         = 47,
    [LINE_RIGHT_X]        = 85,
    [LINE_WEATHER_Y]      = 50,
    [LINE_DATE_Y]         = 69,
    [LINE_TIME_TOP_Y]     = 90,
    [LINE_FOOTER_Y]       = 154,
};

// Canonical battery fill base
static const GRect s_canonical_batt_fill = { .origin = {51, 21}, .size = {0, 11} };

// Canonical sun arrow positions
static const int16_t s_canonical_sunrise_arrow_x = 3;
static const int16_t s_canonical_sunset_arrow_x = 106;
static const int16_t s_canonical_sun_arrow_top_y = 157;
static const int16_t s_canonical_sun_arrow_bottom_y = 165;

// ----------------------------------------------------------------------------
// Helper: Scale a value using fixed-point math
// ----------------------------------------------------------------------------
static int16_t scale_value(int16_t value) {
    return (int16_t)((value * s_layout.scale_1000 + 500) / 1000);
}

// ----------------------------------------------------------------------------
// Helper: Scale and offset a coordinate
// ----------------------------------------------------------------------------
static int16_t scale_and_offset_x(int16_t x) {
    return s_layout.origin_x + scale_value(x);
}

static int16_t scale_and_offset_y(int16_t y) {
    return s_layout.origin_y + scale_value(y);
}

// ----------------------------------------------------------------------------
// Public API
// ----------------------------------------------------------------------------

void layout_init(GRect bounds) {
    s_layout.bounds = bounds;
    
    // Calculate safe bounds (inset for round screens)
    #ifdef PBL_ROUND
        // Adaptive inset: ~7-8% of width for round screen safe area
        int16_t inset = bounds.size.w * 7 / 100;
        s_layout.safe_bounds = grect_inset(bounds, GEdgeInsets(inset));
    #else
        s_layout.safe_bounds = bounds;
    #endif
    
    // Calculate uniform scale factor (use smaller to fit without distortion)
    int16_t scale_w = (s_layout.safe_bounds.size.w * 1000) / CANONICAL_WIDTH;
    int16_t scale_h = (s_layout.safe_bounds.size.h * 1000) / CANONICAL_HEIGHT;
    s_layout.scale_1000 = (scale_w < scale_h) ? scale_w : scale_h;
    
    // Calculate scaled content size
    int16_t scaled_w = scale_value(CANONICAL_WIDTH);
    int16_t scaled_h = scale_value(CANONICAL_HEIGHT);
    
    // Center scaled content within safe bounds
    s_layout.origin_x = s_layout.safe_bounds.origin.x + 
                        (s_layout.safe_bounds.size.w - scaled_w) / 2;
    s_layout.origin_y = s_layout.safe_bounds.origin.y + 
                        (s_layout.safe_bounds.size.h - scaled_h) / 2;
    
    s_layout.initialized = true;
}

GRect layout_get_rect(LayoutId id) {
    if (!s_layout.initialized || id >= LAYOUT_COUNT) {
        return GRectZero;
    }
    
    const GRect* base = &s_canonical_rects[id];
    return GRect(
        scale_and_offset_x(base->origin.x),
        scale_and_offset_y(base->origin.y),
        scale_value(base->size.w),
        scale_value(base->size.h)
    );
}

GRect layout_get_time_digit_rect(TimeDigitId id) {
    if (!s_layout.initialized || id >= DIGIT_COUNT) {
        return GRectZero;
    }
    
    const GRect* base = &s_canonical_digits[id];
    // Scale position only, keep size fixed for crisp digits
    return GRect(
        scale_and_offset_x(base->origin.x),
        scale_and_offset_y(base->origin.y),
        base->size.w,  // Fixed width
        base->size.h   // Fixed height
    );
}

int16_t layout_get_line_coord(LineId id) {
    if (!s_layout.initialized || id >= LINE_COUNT) {
        return 0;
    }
    
    int16_t base = s_canonical_lines[id];
    
    // X coordinates vs Y coordinates
    if (id == LINE_HEADER_SPLIT_X || id == LINE_LEFT_X || id == LINE_RIGHT_X) {
        return scale_and_offset_x(base);
    } else {
        return scale_and_offset_y(base);
    }
}

GRect layout_get_battery_fill_rect(int16_t fill_width_canonical) {
    if (!s_layout.initialized) {
        return GRectZero;
    }
    
    return GRect(
        scale_and_offset_x(s_canonical_batt_fill.origin.x),
        scale_and_offset_y(s_canonical_batt_fill.origin.y),
        scale_value(fill_width_canonical),
        scale_value(s_canonical_batt_fill.size.h)
    );
}

GPoint layout_get_sunrise_arrow_top(void) {
    return GPoint(
        scale_and_offset_x(s_canonical_sunrise_arrow_x),
        scale_and_offset_y(s_canonical_sun_arrow_top_y)
    );
}

GPoint layout_get_sunrise_arrow_bottom(void) {
    return GPoint(
        scale_and_offset_x(s_canonical_sunrise_arrow_x),
        scale_and_offset_y(s_canonical_sun_arrow_bottom_y)
    );
}

GPoint layout_get_sunset_arrow_top(void) {
    return GPoint(
        scale_and_offset_x(s_canonical_sunset_arrow_x),
        scale_and_offset_y(s_canonical_sun_arrow_top_y)
    );
}

GPoint layout_get_sunset_arrow_bottom(void) {
    return GPoint(
        scale_and_offset_x(s_canonical_sunset_arrow_x),
        scale_and_offset_y(s_canonical_sun_arrow_bottom_y)
    );
}
