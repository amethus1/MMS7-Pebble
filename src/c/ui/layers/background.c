#include "background.h"
#include "../../state/state.h"
#include "../../state/settings.h"
#include "../../modules/colors.h"
#include "../battery_style.h"
#include "../layout.h"

struct BackgroundLayer {
    Layer* layer;
};

static void update_proc(Layer* layer, GContext* ctx) {
    AppState* state = state_get_ptr();
    GlobalSettings* settings = settings_get_instance();
    const ColorScheme* scheme = colors_get_scheme(settings->ColorProfile);
    GRect bounds = layer_get_bounds(layer);
    
    // Fill with background color
    graphics_context_set_fill_color(ctx, scheme->clock_bg);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
    
    // Get layout coordinates
    int16_t header_y = layout_get_line_coord(LINE_HEADER_Y);
    int16_t header_split_x = layout_get_line_coord(LINE_HEADER_SPLIT_X);
    int16_t header_split_top_y = layout_get_line_coord(LINE_HEADER_SPLIT_TOP_Y);
    int16_t left_x = layout_get_line_coord(LINE_LEFT_X);
    int16_t right_x = layout_get_line_coord(LINE_RIGHT_X);
    int16_t weather_y = layout_get_line_coord(LINE_WEATHER_Y);
    int16_t date_y = layout_get_line_coord(LINE_DATE_Y);
    int16_t time_top_y = layout_get_line_coord(LINE_TIME_TOP_Y);
    int16_t footer_y = layout_get_line_coord(LINE_FOOTER_Y);
    
    // Draw separator lines (optional)
    if (settings->ShowGridLines) {
#if defined(LAYOUT_REFINED_STATUS)
        // Column dividers are quieter than the horizontal rules, and the
        // date's own spacing separates it from the clock.
        GColor minor = scheme->lines_minor;
        bool draw_time_rule = false;
#else
        GColor minor = scheme->lines_bg;
        bool draw_time_rule = true;
#endif
        graphics_context_set_stroke_color(ctx, minor);
        // Header split line on the right
        graphics_draw_line(ctx, GPoint(header_split_x, header_split_top_y), GPoint(header_split_x, header_y - 1));
        // Column separators
        graphics_draw_line(ctx, GPoint(left_x, header_y + 1), GPoint(left_x, weather_y - 1));
        graphics_draw_line(ctx, GPoint(right_x, header_y + 1), GPoint(right_x, date_y - 1));

        graphics_context_set_stroke_color(ctx, scheme->lines_bg);
        // Header bottom
        graphics_draw_line(ctx, GPoint(0, header_y), GPoint(bounds.size.w, header_y));
        // Weather info line
        graphics_draw_line(ctx, GPoint(0, weather_y), GPoint(right_x - 1, weather_y));
        // Date row
        graphics_draw_line(ctx, GPoint(0, date_y), GPoint(bounds.size.w, date_y));
        // Time row top
        if (draw_time_rule) {
            graphics_draw_line(ctx, GPoint(0, time_top_y), GPoint(bounds.size.w, time_top_y));
        }
        // Footer separator
        graphics_draw_line(ctx, GPoint(0, footer_y), GPoint(bounds.size.w, footer_y));
    }

    // Colon separator between hour and minute digits
    graphics_context_set_fill_color(ctx, scheme->clock);
    graphics_fill_rect(ctx, layout_get_rect(LAYOUT_COLON_TOP), 0, GCornerNone);
    graphics_fill_rect(ctx, layout_get_rect(LAYOUT_COLON_BOTTOM), 0, GCornerNone);

    // Sunrise / sunset arrows are drawn by the status layer, next to their times.

    BatteryPalette battery_palette;
    battery_style_get_palette(settings, state->battery.charge_percent, &battery_palette);

    GRect box = layout_get_rect(LAYOUT_BATTERY_BOX);
    GRect fill = layout_get_battery_fill_rect(
        (LAYOUT_BATTERY_FILL_MAX_W * state->battery.charge_percent) / 100);

    // Keep the header background clean. Battery state color is restricted to
    // the charge fill instead of painting a large rectangular widget.
    if (fill.size.w > 0) {
        graphics_context_set_fill_color(ctx, battery_palette.fill_color);
        graphics_fill_rect(ctx, fill, 0, GCornerNone);
    }

    graphics_context_set_stroke_color(ctx, battery_palette.text_color);
    graphics_draw_line(ctx, GPoint(box.origin.x, box.origin.y), GPoint(box.origin.x + box.size.w - 2, box.origin.y));
    graphics_draw_line(ctx, GPoint(box.origin.x, box.origin.y + box.size.h - 1),
                       GPoint(box.origin.x + box.size.w - 2, box.origin.y + box.size.h - 1));
    graphics_draw_line(ctx, GPoint(box.origin.x, box.origin.y), GPoint(box.origin.x, box.origin.y + box.size.h - 1));

    int16_t box_right = box.origin.x + box.size.w - 2;
    int16_t box_top = box.origin.y;
    int16_t box_bottom = box.origin.y + box.size.h - 1;
    graphics_draw_line(ctx, GPoint(box_right, box_top), GPoint(box_right, box_top + 4));
    graphics_draw_line(ctx, GPoint(box_right, box_bottom - 4), GPoint(box_right, box_bottom));
    graphics_draw_line(ctx, GPoint(box_right + 2, box_top + 4), GPoint(box_right + 2, box_bottom - 4));
    graphics_draw_line(ctx, GPoint(box_right, box_top + 4), GPoint(box_right + 2, box_top + 4));
    graphics_draw_line(ctx, GPoint(box_right, box_bottom - 4), GPoint(box_right + 2, box_bottom - 4));
}

BackgroundLayer* background_layer_create(GRect frame) {
    BackgroundLayer* bg = malloc(sizeof(BackgroundLayer));
    bg->layer = layer_create(frame);
    layer_set_update_proc(bg->layer, update_proc);
    return bg;
}

void background_layer_destroy(BackgroundLayer* bg) {
    layer_destroy(bg->layer);
    free(bg);
}

Layer* background_layer_get_layer(BackgroundLayer* bg) {
    return bg->layer;
}

void background_layer_mark_dirty(BackgroundLayer* bg) {
    layer_mark_dirty(bg->layer);
}
