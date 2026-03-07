#include "effect_layer.h"
#include <string.h>

static uint8_t find_parent_offset(void) {
    Layer* parent = layer_create(GRect(0, 0, 32, 32));
    Layer* child = layer_create(GRect(0, 0, 16, 16));
    layer_add_child(parent, child);

    uint8_t offset = 0;
    while (offset < 16 && *(((Layer**)(void*)child) + offset) != parent) {
        offset++;
    }

    if (*(((Layer**)(void*)child) + offset) != parent) {
        offset = 0xff;
        APP_LOG(APP_LOG_LEVEL_ERROR, "Unable to find parent layer offset");
    }

    layer_destroy(child);
    layer_destroy(parent);
    return offset;
}

static void effect_layer_update_proc(Layer* layer, GContext* ctx) {
    static uint8_t parent_layer_offset = 0xff;
    if (parent_layer_offset == 0xff) {
        parent_layer_offset = find_parent_offset();
    }

    EffectLayer* effect_layer = (EffectLayer*)layer_get_data(layer);
    GRect layer_frame = layer_get_frame(layer);
    Layer* current = layer;
    while ((current = ((Layer**)(void*)current)[parent_layer_offset])) {
        GRect parent_frame = layer_get_frame(current);
        layer_frame.origin.x += parent_frame.origin.x;
        layer_frame.origin.y += parent_frame.origin.y;
    }

    for (uint8_t i = 0; i < MAX_EFFECTS && effect_layer->effects[i]; i++) {
        effect_layer->effects[i](ctx, layer_frame, effect_layer->params[i]);
    }
}

EffectLayer* effect_layer_create(GRect frame) {
    Layer* layer = layer_create_with_data(frame, sizeof(EffectLayer));
    layer_set_update_proc(layer, effect_layer_update_proc);

    EffectLayer* effect_layer = (EffectLayer*)layer_get_data(layer);
    memset(effect_layer, 0, sizeof(EffectLayer));
    effect_layer->layer = layer;
    return effect_layer;
}

void effect_layer_destroy(EffectLayer* effect_layer) {
    layer_destroy(effect_layer->layer);
}

void effect_layer_add_effect(EffectLayer* effect_layer, effect_cb* effect, void* param) {
    if (effect_layer->next_effect >= MAX_EFFECTS) {
        return;
    }

    effect_layer->effects[effect_layer->next_effect] = effect;
    effect_layer->params[effect_layer->next_effect] = param;
    effect_layer->next_effect++;
}

Layer* effect_layer_get_layer(EffectLayer* effect_layer) {
    return effect_layer->layer;
}
