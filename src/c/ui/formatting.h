#pragma once
#include <pebble.h>
#include "../state/state.h"

// Format elapsed time (e.g. "5 m", "2h 30m", "1d 3h")
void format_time_elapsed(char* buffer, size_t buf_size, time_t seconds);

// Format sun time (sunrise/sunset) in the weather location's timezone.
void format_sun_time(char* buffer, size_t buf_size, time_t unix_time, int utc_offset_seconds, bool is_24h);

// Create a text layer with common properties in one call
TextLayer* create_text_layer(GRect frame, const char* font_key, GTextAlignment align, GColor text_color);
