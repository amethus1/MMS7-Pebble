#include "time_layer.h"
#include "../painters/digits.h"
#include "../layout.h"

// Digit slot for each digit (H1, H2, M1, M2, S1, S2)
typedef struct {
    int value;
    int size;       // 15 or 41
    int w, h;       // box dimensions for scalable digit art
    bool box_mode;
    GColor color;
} DigitSlot;

struct TimeLayer {
    Layer* root_layer;
    Layer* digit_layers[6];
    bool show_seconds;
    bool leading_zero;
};

static void digit_update_proc(Layer* layer, GContext* ctx) {
    DigitSlot* slot = (DigitSlot*)layer_get_data(layer);
    graphics_context_set_stroke_color(ctx, slot->color);
    if (slot->box_mode) {
        digits_paint_box(ctx, slot->value, slot->w, slot->h, GPoint(0,0));
    } else {
        digits_paint(ctx, slot->value, slot->size, GPoint(0,0));
    }
}

static void init_digit(TimeLayer* tl, int index, int size, GRect frame) {
    Layer* layer = layer_create_with_data(frame, sizeof(DigitSlot));
    tl->digit_layers[index] = layer;

    DigitSlot* slot = (DigitSlot*)layer_get_data(layer);
    slot->size = size;
    slot->w = frame.size.w;
    slot->h = frame.size.h;
#if defined(LAYOUT_LARGE_DISPLAY)
    slot->box_mode = true;
#else
    slot->box_mode = false;
#endif
    slot->value = 0;
    slot->color = GColorWhite;

    layer_set_update_proc(layer, digit_update_proc);
    layer_add_child(tl->root_layer, layer);
}

TimeLayer* time_layer_create(GRect frame) {
    TimeLayer* tl = malloc(sizeof(TimeLayer));
    tl->root_layer = layer_create(frame);
    tl->show_seconds = false;
    tl->leading_zero = true;

    init_digit(tl, 0, 41, layout_get_time_digit_rect(DIGIT_H1));
    init_digit(tl, 1, 41, layout_get_time_digit_rect(DIGIT_H2));
    init_digit(tl, 2, 41, layout_get_time_digit_rect(DIGIT_M1));
    init_digit(tl, 3, 41, layout_get_time_digit_rect(DIGIT_M2));
    
    // Seconds (bottom-right of time area)
    init_digit(tl, 4, 15, layout_get_time_digit_rect(DIGIT_S1));
    init_digit(tl, 5, 15, layout_get_time_digit_rect(DIGIT_S2));

    time_layer_set_show_seconds(tl, false);

    return tl;
}

void time_layer_destroy(TimeLayer* tl) {
    for(int i=0; i<6; i++) {
        layer_destroy(tl->digit_layers[i]);
    }
    layer_destroy(tl->root_layer);
    free(tl);
}

Layer* time_layer_get_layer(TimeLayer* tl) {
    return tl->root_layer;
}

void time_layer_set_show_seconds(TimeLayer* tl, bool show) {
    tl->show_seconds = show;
    layer_set_hidden(tl->digit_layers[4], !show);
    layer_set_hidden(tl->digit_layers[5], !show);
}

void time_layer_update(TimeLayer* tl, int hour, int minute, int second) {
    int h1 = hour / 10;
    int h2 = hour % 10;
    int m1 = minute / 10;
    int m2 = minute % 10;
    int s1 = second / 10;
    int s2 = second % 10;
    
    int values[6] = {h1, h2, m1, m2, s1, s2};
    
    for(int i=0; i<6; i++) {
        DigitSlot* slot = (DigitSlot*)layer_get_data(tl->digit_layers[i]);
        if (slot->value != values[i]) {
            slot->value = values[i];
            layer_mark_dirty(tl->digit_layers[i]);
        }
    }
    layer_set_hidden(tl->digit_layers[0], !tl->leading_zero && h1 == 0);
}

void time_layer_set_leading_zero(TimeLayer* tl, bool leading_zero) {
    tl->leading_zero = leading_zero;
    DigitSlot* slot = (DigitSlot*)layer_get_data(tl->digit_layers[0]);
    layer_set_hidden(tl->digit_layers[0], !leading_zero && slot->value == 0);
}

void time_layer_update_colors(TimeLayer* tl, GColor color) {
    for(int i=0; i<6; i++) {
        DigitSlot* slot = (DigitSlot*)layer_get_data(tl->digit_layers[i]);
        slot->color = color;
        layer_mark_dirty(tl->digit_layers[i]);
    }
}
