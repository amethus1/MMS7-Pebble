#pragma once
#include <pebble.h>

// Heart rate, drawn as a small heart and the bpm in the week label's slot
// under the clock. The caller decides when it is shown instead of the week.
typedef struct HeartLayer HeartLayer;

HeartLayer* heart_layer_create(void);
void heart_layer_destroy(HeartLayer* hl);
Layer* heart_layer_get_layer(HeartLayer* hl);

// bpm <= 0 shows "--" (no reading in the last 15 minutes)
void heart_layer_set_bpm(HeartLayer* hl, int bpm);

void heart_layer_set_colors(HeartLayer* hl, GColor text, GColor heart);

// Emery: follow the week label into the left slot (true) or the middle slot (false)
void heart_layer_set_left(HeartLayer* hl, bool left);
