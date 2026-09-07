#pragma once
#include <pebble.h>

typedef struct DateLayer DateLayer;

DateLayer* date_layer_create(GRect frame);
void date_layer_destroy(DateLayer* layer);
Layer* date_layer_get_layer(DateLayer* layer);

// Update date string (e.g. from strftime)
void date_layer_update_text(DateLayer* layer, const char* date_text);

// Update calendar week text
void date_layer_update_cw(DateLayer* layer, const char* cw_text);

// Update colors
void date_layer_update_color(DateLayer* layer, GColor color);

// Show/hide calendar week
void date_layer_set_cw_visible(DateLayer* layer, bool visible);

// Emery: put the week label in the left slot (true) or the middle slot (false)
void date_layer_set_cw_left(DateLayer* dl, bool left);
