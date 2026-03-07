#include "battery_style.h"

static int clamp_percent(int percent) {
    if (percent < 0) {
        return 0;
    }
    if (percent > 100) {
        return 100;
    }
    return percent;
}

void battery_style_get_palette(const GlobalSettings* settings, int percent, BatteryPalette* palette) {
    int clamped = clamp_percent(percent);

#if defined(PBL_COLOR)
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

    if (settings->ColorProfile == 0) {
        palette->inverter_argb = GColorWhiteARGB8;
        palette->background_argb = GColorBlackARGB8;
    } else if (settings->ColorProfile == 1) {
        palette->inverter_argb = GColorBlackARGB8;
        palette->background_argb = GColorWhiteARGB8;
    } else {
        palette->inverter_argb = GColorWhiteARGB8;
        palette->background_argb = variable_color;
    }

    if (clamped <= 20) {
        palette->inverter_argb = GColorWhiteARGB8;
        palette->background_argb = variable_color;
    }

#if defined(PBL_PLATFORM_DIORITE)
    if (clamped <= 20) {
        if (settings->ColorProfile == 0) {
            palette->inverter_argb = GColorBlackARGB8;
            palette->background_argb = GColorWhiteARGB8;
        } else if (settings->ColorProfile == 1) {
            palette->inverter_argb = GColorWhiteARGB8;
            palette->background_argb = GColorBlackARGB8;
        } else {
            palette->inverter_argb = GColorBlackARGB8;
            palette->background_argb = GColorWhiteARGB8;
        }
    }
#endif
#else
    if (clamped <= 20) {
        if (settings->ColorProfile == 0) {
            palette->inverter_argb = GColorBlackARGB8;
            palette->background_argb = GColorWhiteARGB8;
        } else {
            palette->inverter_argb = GColorWhiteARGB8;
            palette->background_argb = GColorBlackARGB8;
        }
    } else {
        if (settings->ColorProfile == 1) {
            palette->inverter_argb = GColorBlackARGB8;
            palette->background_argb = GColorWhiteARGB8;
        } else {
            palette->inverter_argb = GColorWhiteARGB8;
            palette->background_argb = GColorBlackARGB8;
        }
    }
#endif

    palette->text_color = (GColor8){ .argb = palette->inverter_argb };
    palette->background_color = (GColor8){ .argb = palette->background_argb };
    palette->fill_color = palette->background_color;
}
