#include "battery_style.h"
#include "../modules/colors.h"

#if defined(PBL_COLOR)
static int clamp_percent(int percent) {
    if (percent < 0) {
        return 0;
    }
    if (percent > 100) {
        return 100;
    }
    return percent;
}
#endif

void battery_style_get_palette(const GlobalSettings* settings, int percent, BatteryPalette* palette) {
    const ColorScheme* scheme = colors_get_scheme(settings->ColorProfile);

    palette->text_color = scheme->battery_text;

#if defined(PBL_COLOR)
    int clamped = clamp_percent(percent);
    uint8_t variable_color;
#if defined(PBL_ROUND)
    if (clamped > 30) {
        variable_color = GColorKellyGreenARGB8;
    } else if (clamped > 20) {
        variable_color = GColorOrangeARGB8;
    } else {
        variable_color = GColorRedARGB8;
    }
#else
    if (clamped > 20) {
        variable_color = GColorKellyGreenARGB8;
    } else if (clamped > 10) {
        variable_color = GColorOrangeARGB8;
    } else {
        variable_color = GColorRedARGB8;
    }
#endif

    GColor fill = (GColor8){ .argb = variable_color };
    // The gauge is drawn straight onto the watchface background, so a state
    // colour that matches it would be invisible - which is exactly what the
    // "Black on Red" profile does at low battery. Fall back to the profile's
    // own fill colour in that case.
    if (gcolor_equal(fill, scheme->clock_bg)) {
        fill = scheme->battery_fill;
    }
    palette->fill_color = fill;
#else
    // Black & white: a solid bar in the text colour, with the percentage
    // inverted over it, reads far better than text over a dither.
    (void)percent;
    palette->fill_color = scheme->battery_text;
#endif

    palette->fill_text_color = scheme->battery_bg;
}
