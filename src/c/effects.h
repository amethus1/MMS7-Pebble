#pragma once
#include <pebble.h>

typedef void effect_cb(GContext* ctx, GRect position, void* param);

extern uint8_t GlobalInverterColor;
extern uint8_t GlobalBkgColor;

effect_cb effect_invert_color;
