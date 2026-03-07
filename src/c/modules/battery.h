#pragma once
#include <pebble.h>
#include "../state/state.h"

// Initialize battery service subscription
void battery_module_init();

// Unsubscribe
void battery_module_deinit();

// Handle system battery event, update state
void battery_handle_event(BatteryChargeState charge_state);
