#pragma once
#include <pebble.h>
#include "../state/settings.h"

typedef struct {
    GColor text_color;  // Percentage / remaining-time text
    GColor fill_color;  // Charge bar inside the battery outline
} BatteryPalette;

void battery_style_get_palette(const GlobalSettings* settings, int percent, BatteryPalette* palette);
