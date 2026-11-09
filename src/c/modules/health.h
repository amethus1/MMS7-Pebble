#pragma once
#include <pebble.h>
#include "../state/state.h"

void health_module_init();
void health_module_deinit();

// Poll health service and update state
void health_update();

// Read only the heart rate (cheap; safe to call every minute)
void health_update_heart_rate();
