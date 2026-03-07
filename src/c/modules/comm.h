#pragma once
#include <pebble.h>

// Initialize communication
// callback: function to call when processing is required (e.g., process_events)
void comm_init(void (*callback)(void));

// Deinitialize
void comm_deinit();

// Request new weather data from JS
void comm_request_weather();
