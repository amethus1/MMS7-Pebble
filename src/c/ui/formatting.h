#pragma once
#include <pebble.h>
#include "../state/state.h"

// Format temperature into buffer (e.g. "24°", "-5°")
void format_temperature(char* buffer, size_t buf_size, int temp_c, bool is_metric);

// Get formatted description (e.g. "Cloudy", "Rain")
// Uses raw condition text from the JS weather provider
void format_condition_text(char* buffer, size_t buf_size, const char* raw_desc);

// Format battery status (e.g. "80%", "Charging")
void format_battery_status(char* buffer, size_t buf_size, BatteryState* component);

// Format layer string (generic helper)
// e.g. for complications like "Steps: 5000"
void format_health_string(char* buffer, size_t buf_size, HealthState* component);

// Format elapsed time (e.g. "5 m", "2h 30m", "1d 3h")
void format_time_elapsed(char* buffer, size_t buf_size, time_t seconds);

// Format sun time (sunrise/sunset) in the weather location's timezone.
void format_sun_time(char* buffer, size_t buf_size, time_t unix_time, int utc_offset_seconds, bool is_24h);

// Create a text layer with common properties in one call
TextLayer* create_text_layer(GRect frame, const char* font_key, GTextAlignment align, GColor text_color);
