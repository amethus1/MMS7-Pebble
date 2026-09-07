#include "date_layer.h"
#include "../../state/settings.h"
#include "../../modules/colors.h"
#include "../layout.h"

struct DateLayer {
    Layer* root_layer;
    TextLayer* text_layer;
    TextLayer* cw_layer;  // Calendar week
};

#if defined(LAYOUT_LARGE_DISPLAY)
#define DATE_MAIN_FONT FONT_KEY_GOTHIC_28_BOLD
#define DATE_CW_FONT FONT_KEY_GOTHIC_18
#else
#define DATE_MAIN_FONT FONT_KEY_GOTHIC_24_BOLD
#define DATE_CW_FONT FONT_KEY_GOTHIC_14
#endif

DateLayer* date_layer_create(GRect frame) {
    DateLayer* dl = malloc(sizeof(DateLayer));
    dl->root_layer = layer_create(frame);
    
    // Date (centered)
    dl->text_layer = text_layer_create(layout_get_rect(LAYOUT_DATE));
    text_layer_set_font(dl->text_layer, fonts_get_system_font(DATE_MAIN_FONT));
    text_layer_set_text_alignment(dl->text_layer, GTextAlignmentCenter);
    text_layer_set_background_color(dl->text_layer, GColorClear);
    text_layer_set_text_color(dl->text_layer, GColorWhite);
    layer_add_child(dl->root_layer, text_layer_get_layer(dl->text_layer));
    
    // Calendar week
    dl->cw_layer = text_layer_create(layout_get_rect(LAYOUT_CW));
    text_layer_set_font(dl->cw_layer, fonts_get_system_font(DATE_CW_FONT));
#if defined(PBL_ROUND) || defined(LAYOUT_REFINED_STATUS)
    text_layer_set_text_alignment(dl->cw_layer, GTextAlignmentCenter);   // Fixed middle slot
#else
    text_layer_set_text_alignment(dl->cw_layer, GTextAlignmentLeft);
#endif
    text_layer_set_background_color(dl->cw_layer, GColorClear);
    text_layer_set_text_color(dl->cw_layer, GColorWhite);
    layer_add_child(dl->root_layer, text_layer_get_layer(dl->cw_layer));
    
    return dl;
}

void date_layer_destroy(DateLayer* dl) {
    text_layer_destroy(dl->text_layer);
    text_layer_destroy(dl->cw_layer);
    layer_destroy(dl->root_layer);
    free(dl);
}

Layer* date_layer_get_layer(DateLayer* dl) {
    return dl->root_layer;
}

void date_layer_update_text(DateLayer* dl, const char* date_text) {
    text_layer_set_text(dl->text_layer, date_text);
}

void date_layer_update_cw(DateLayer* dl, const char* cw_text) {
    text_layer_set_text(dl->cw_layer, cw_text);
}

void date_layer_update_color(DateLayer* dl, GColor color) {
    text_layer_set_text_color(dl->text_layer, color);
    text_layer_set_text_color(dl->cw_layer, color);
}

void date_layer_set_cw_visible(DateLayer* dl, bool visible) {
    layer_set_hidden(text_layer_get_layer(dl->cw_layer), !visible);
}
