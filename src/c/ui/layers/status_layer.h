#pragma once
#include <pebble.h>

typedef struct StatusLayer StatusLayer;

StatusLayer* status_layer_create(GRect frame);
void status_layer_destroy(StatusLayer* layer);
Layer* status_layer_get_layer(StatusLayer* layer);

void status_layer_update(StatusLayer* layer);
void status_layer_update_colors(StatusLayer* layer);
