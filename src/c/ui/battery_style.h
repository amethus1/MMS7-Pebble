#pragma once
#include <pebble.h>
#include "../state/settings.h"

typedef struct {
    GColor text_color;
    GColor background_color;
    GColor fill_color;
    uint8_t inverter_argb;
    uint8_t background_argb;
} BatteryPalette;

void battery_style_get_palette(const GlobalSettings* settings, int percent, BatteryPalette* palette);
