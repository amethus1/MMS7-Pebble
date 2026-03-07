#pragma once
#include <pebble.h>

typedef struct TimeLayer TimeLayer;

TimeLayer* time_layer_create(GRect frame);
void time_layer_destroy(TimeLayer* layer);
Layer* time_layer_get_layer(TimeLayer* layer);

// Update time digits
// hour, minute, second: values to display
// animate: whether to animate (optional, maybe later)
void time_layer_update(TimeLayer* layer, int hour, int minute, int second);

// Update colors
void time_layer_update_colors(TimeLayer* layer, GColor color);

// Show/hide seconds digits
void time_layer_set_show_seconds(TimeLayer* layer, bool show);

