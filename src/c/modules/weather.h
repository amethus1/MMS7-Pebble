#pragma once
#include <pebble.h>
#include "../state/state.h"

// Initialize weather module (load persistence)
void weather_init();

// Deinitialize (save persistence)
void weather_deinit();

// Handle incoming AppMessage dictionary for weather keys
bool weather_handle_app_message(DictionaryIterator *iterator);

// Check if data is stale, return true if state changed
bool weather_check_staleness();

void weather_request_update(void);

// Helper: Calculate night mode from state
bool weather_is_night();

// Helper: Check if valid weather data exists
bool weather_has_data();

// Helper: Get timezone string
const char* weather_get_timezone();
