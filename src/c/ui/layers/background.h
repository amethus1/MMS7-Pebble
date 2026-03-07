#pragma once
#include <pebble.h>

typedef struct BackgroundLayer BackgroundLayer;

BackgroundLayer* background_layer_create(GRect frame);
void background_layer_destroy(BackgroundLayer* layer);
Layer* background_layer_get_layer(BackgroundLayer* layer);

// Trigger a redraw (e.g. color scheme change)
void background_layer_mark_dirty(BackgroundLayer* layer);
