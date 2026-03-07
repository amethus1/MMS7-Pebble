#pragma once
#include <pebble.h>

// Paint a digit (0-9) at the specified position with the given size (height).
// Supported sizes: 15 (small), 41 (large/normal)
void digits_paint(GContext* ctx, int digit, int size, GPoint pos);

// Clear the area for a digit
void digits_clear(GContext* ctx, int size, GPoint pos);
