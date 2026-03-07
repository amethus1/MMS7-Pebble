#pragma once
#include <pebble.h>
#include "../state/state.h"

void health_module_init();
void health_module_deinit();

// Poll health service and update state
void health_update();
