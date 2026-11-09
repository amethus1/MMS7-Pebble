#include "heart_layer.h"
#include <string.h>
#include "../layout.h"

// Same font as the week label it stands in for (see date_layer.c)
#if defined(LAYOUT_LARGE_DISPLAY)
#define HEART_FONT FONT_KEY_GOTHIC_18
// Gothic 18 digits are drawn 7..17px below the top of their box, 7px apart
#define HEART_W 9
#define HEART_TOP 8
#define HEART_GAP 2
#else
#define HEART_FONT FONT_KEY_GOTHIC_14
// Gothic 14 digits are drawn 5..13px below the top of their box, 6px apart
#define HEART_W 7
#define HEART_TOP 6
#define HEART_GAP 2
#endif

// The heart as horizontal runs (row, first x, last x), pixel-aligned so it
// stays crisp and needs no font or bitmap resource. Sized to sit centred on
// the digits next to it.
typedef struct { int8_t row, x0, x1; } HeartRun;
#if defined(LAYOUT_LARGE_DISPLAY)
static const HeartRun s_heart[] = {      // 9 x 8
    {0, 1, 2}, {0, 6, 7},
    {1, 0, 3}, {1, 5, 8},
    {2, 0, 8},
    {3, 0, 8},
    {4, 1, 7},
    {5, 2, 6},
    {6, 3, 5},
    {7, 4, 4},
};
#else
static const HeartRun s_heart[] = {      // 7 x 6
    {0, 1, 2}, {0, 4, 5},
    {1, 0, 6},
    {2, 0, 6},
    {3, 1, 5},
    {4, 2, 4},
    {5, 3, 3},
};
#endif

typedef struct {
    char text[6];
    GColor text_color;
    GColor heart_color;
    bool left;
} HeartData;

struct HeartLayer {
    Layer* layer;
};

// The week label is centred in its slot on round screens and on the refined
// Emery / Gabbro layouts, left-aligned on the classic rectangle. The heart and
// number are measured as one group and placed the same way.
static bool heart_is_centred(const HeartData* d) {
#if defined(PBL_ROUND) || defined(LAYOUT_REFINED_STATUS)
    return !d->left;
#else
    (void)d;
    return false;
#endif
}

static void heart_update_proc(Layer* layer, GContext* ctx) {
    HeartData* d = (HeartData*)layer_get_data(layer);
    GRect b = layer_get_bounds(layer);
    GFont font = fonts_get_system_font(HEART_FONT);

    GSize text = graphics_text_layout_get_content_size(d->text, font,
        GRect(0, 0, b.size.w, b.size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
    int16_t group_w = HEART_W + HEART_GAP + text.w;
    int16_t x0 = heart_is_centred(d) ? (b.size.w - group_w) / 2 : 0;
    if (x0 < 0) x0 = 0;

    graphics_context_set_fill_color(ctx, d->heart_color);
    for (size_t i = 0; i < sizeof(s_heart) / sizeof(s_heart[0]); i++) {
        const HeartRun* r = &s_heart[i];
        graphics_fill_rect(ctx, GRect(x0 + r->x0, HEART_TOP + r->row, r->x1 - r->x0 + 1, 1), 0, GCornerNone);
    }

    graphics_context_set_text_color(ctx, d->text_color);
    int16_t text_x = x0 + HEART_W + HEART_GAP;
    graphics_draw_text(ctx, d->text, font, GRect(text_x, 0, b.size.w - text_x, b.size.h),
                       GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
}

HeartLayer* heart_layer_create(void) {
    HeartLayer* hl = malloc(sizeof(HeartLayer));
    hl->layer = layer_create_with_data(layout_get_rect(LAYOUT_CW), sizeof(HeartData));
    HeartData* d = (HeartData*)layer_get_data(hl->layer);
    *d = (HeartData){ .text_color = GColorWhite, .heart_color = GColorWhite, .left = false };
    snprintf(d->text, sizeof(d->text), "--");
    layer_set_update_proc(hl->layer, heart_update_proc);
    return hl;
}

void heart_layer_destroy(HeartLayer* hl) {
    layer_destroy(hl->layer);
    free(hl);
}

Layer* heart_layer_get_layer(HeartLayer* hl) {
    return hl->layer;
}

void heart_layer_set_bpm(HeartLayer* hl, int bpm) {
    HeartData* d = (HeartData*)layer_get_data(hl->layer);
    char text[sizeof(d->text)];
    if (bpm > 0) {
        snprintf(text, sizeof(text), "%d", bpm > 999 ? 999 : bpm);
    } else {
        snprintf(text, sizeof(text), "--");
    }
    if (strcmp(text, d->text) != 0) {
        memcpy(d->text, text, sizeof(d->text));
        layer_mark_dirty(hl->layer);
    }
}

void heart_layer_set_colors(HeartLayer* hl, GColor text, GColor heart) {
    HeartData* d = (HeartData*)layer_get_data(hl->layer);
    d->text_color = text;
    d->heart_color = heart;
    layer_mark_dirty(hl->layer);
}

void heart_layer_set_left(HeartLayer* hl, bool left) {
#if defined(LAYOUT_REFINED_STATUS) && !defined(PBL_ROUND)
    HeartData* d = (HeartData*)layer_get_data(hl->layer);
    d->left = left;
    layer_set_frame(hl->layer, layout_get_rect(left ? LAYOUT_CW_LEFT : LAYOUT_CW));
    layer_mark_dirty(hl->layer);
#else
    (void)hl; (void)left;
#endif
}
