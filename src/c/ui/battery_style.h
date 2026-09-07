#pragma once
#include <pebble.h>
#include "../state/settings.h"

typedef struct {
    GColor text_color;       // Percentage / remaining-time text on the background
    GColor fill_color;       // Charge bar inside the battery outline
    GColor fill_text_color;  // Percentage where it overlaps the charge bar (inverted, like MSS7)
} BatteryPalette;

void battery_style_get_palette(const GlobalSettings* settings, int percent, BatteryPalette* palette);
