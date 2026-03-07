#pragma once
#include <pebble.h>

typedef enum {
  EVENT_NONE = 0,
  EVENT_TIME_TICK      = 1 << 0,  // Minute/Second updated
  EVENT_WEATHER_DATA   = 1 << 1,  // Weather temperature/icon changed
  EVENT_BATTERY_STATE  = 1 << 2,  // Battery level/charging changed
  EVENT_SETTINGS_CHANGED = 1 << 3, // Config updated
  EVENT_HEALTH_UPDATE  = 1 << 4,  // Steps/Sleep changed
  EVENT_CONNECTION     = 1 << 5,  // Bluetooth connection status changed
  EVENT_ALL            = 0xFFFF
} EventFlag;

// Initialize the event system
void events_init();

// Set one or more event flags
void events_set_flag(EventFlag flag);

// Get the accumulated health flags and clear them
// Should be called by the main loop/render function
EventFlag events_get_and_clear_flags();
