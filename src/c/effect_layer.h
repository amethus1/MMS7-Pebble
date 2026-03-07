#pragma once
#include <pebble.h>
#include "effects.h"

#define MAX_EFFECTS 4

typedef struct {
    Layer* layer;
    effect_cb* effects[MAX_EFFECTS];
    void* params[MAX_EFFECTS];
    uint8_t next_effect;
} EffectLayer;

EffectLayer* effect_layer_create(GRect frame);
void effect_layer_destroy(EffectLayer* effect_layer);
void effect_layer_add_effect(EffectLayer* effect_layer, effect_cb* effect, void* param);
Layer* effect_layer_get_layer(EffectLayer* effect_layer);
