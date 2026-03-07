#pragma once
#include <pebble.h>

typedef struct WeatherLayer WeatherLayer;

WeatherLayer* weather_layer_create(GRect frame);
void weather_layer_destroy(WeatherLayer* layer);
Layer* weather_layer_get_layer(WeatherLayer* layer);

void weather_layer_update_data(WeatherLayer* layer); // Pulls from global state
void weather_layer_update_colors(WeatherLayer* layer);
