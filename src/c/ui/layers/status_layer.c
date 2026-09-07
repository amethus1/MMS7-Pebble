#include "status_layer.h"
#include <string.h>
#include "../../state/state.h"
#include "../../state/settings.h"
#include "../formatting.h"
#include "../../modules/colors.h"
#include "../../modules/weather.h"
#include "../battery_style.h"
#include "../../modules/rules.h"
#include "../layout.h"

struct StatusLayer {
    Layer* root_layer;
    TextLayer* battery_layer;       // Percentage, drawn inside the battery outline
    TextLayer* battery_fill_layer;  // Same text, clipped to the charge bar and drawn inverted
    TextLayer* battery_time_layer;  // Time since the last charge, below it
    TextLayer* connection_layer;
#if defined(LAYOUT_REFINED_STATUS)
    Layer* bluetooth_layer;     // Drawn glyph instead of the word
#endif
    TextLayer* sunrise_layer;
    TextLayer* sunset_layer;
    Layer* sun_arrows_layer;   // The up/down arrows, kept next to their times
    TextLayer* timezone_layer;
    // Text buffers (moved from function-static for memory safety)
    char batt_buf[24];
    char batt_time_buf[16];
    char rise_buf[8];
    char set_buf[8];
    char tz_buf[20];
#if defined(PBL_HEALTH)
    BitmapLayer* health_icon_layer;
    GBitmap* health_icon_steps;
    GBitmap* health_icon_sleep;
    TextLayer* health_text_layer;
    Layer* health_trend_layer;
    char health_buf[20];
#endif
};

// The battery outline is the original MSS7 size (42x15 on the classic
// screens) with the percentage drawn inside it, inverted over the charge bar.
#if defined(LAYOUT_LARGE_DISPLAY)
#define STATUS_BATTERY_FONT FONT_KEY_GOTHIC_18_BOLD
#define STATUS_SMALL_FONT FONT_KEY_GOTHIC_18
// On Gabbro the timezone / health slot is in the bottom cap of the circle (~100px wide).
#define STATUS_MEDIUM_FONT FONT_KEY_GOTHIC_18   // Supporting information under the clock
#define STATUS_SUN_FONT FONT_KEY_GOTHIC_24   // Sunrise / sunset times, same on Emery and Gabbro
#elif defined(PBL_ROUND)
// The timezone / health slot sits in the bottom cap of the circle, ~65px wide.
#define STATUS_BATTERY_FONT FONT_KEY_GOTHIC_14
#define STATUS_SMALL_FONT FONT_KEY_GOTHIC_14
#define STATUS_MEDIUM_FONT FONT_KEY_GOTHIC_14
#else
#define STATUS_BATTERY_FONT FONT_KEY_GOTHIC_14
#define STATUS_SMALL_FONT FONT_KEY_GOTHIC_14
#define STATUS_MEDIUM_FONT FONT_KEY_GOTHIC_18
#endif
#define STATUS_BATTERY_TIME_FONT FONT_KEY_GOTHIC_14
#ifndef STATUS_SUN_FONT
#define STATUS_SUN_FONT STATUS_SMALL_FONT
#endif

// Gap between an arrow's stem and the first / last glyph of its time
#define SUN_ARROW_GAP 4

typedef struct {
    int16_t sunrise_x;
    int16_t sunset_x;
    int16_t top_y;
    int16_t bottom_y;
    GColor color;
} SunArrows;

static void sun_arrows_update_proc(Layer* layer, GContext* ctx) {
    SunArrows* a = (SunArrows*)layer_get_data(layer);
    graphics_context_set_stroke_color(ctx, a->color);
    // Sunrise: arrow pointing up
    graphics_draw_line(ctx, GPoint(a->sunrise_x, a->top_y), GPoint(a->sunrise_x, a->bottom_y));
    graphics_draw_line(ctx, GPoint(a->sunrise_x - 1, a->top_y + 1), GPoint(a->sunrise_x + 1, a->top_y + 1));
    graphics_draw_line(ctx, GPoint(a->sunrise_x - 2, a->top_y + 2), GPoint(a->sunrise_x + 2, a->top_y + 2));
    // Sunset: arrow pointing down
    graphics_draw_line(ctx, GPoint(a->sunset_x, a->top_y), GPoint(a->sunset_x, a->bottom_y));
    graphics_draw_line(ctx, GPoint(a->sunset_x - 1, a->bottom_y - 1), GPoint(a->sunset_x + 1, a->bottom_y - 1));
    graphics_draw_line(ctx, GPoint(a->sunset_x - 2, a->bottom_y - 2), GPoint(a->sunset_x + 2, a->bottom_y - 2));
}

// The sunset time is right-aligned, so its arrow is placed from the measured
// width of the text; the sunrise arrow sits a fixed gap before its left-aligned text.
static void status_layer_place_sun_arrows(StatusLayer* sl) {
    SunArrows* a = (SunArrows*)layer_get_data(sl->sun_arrows_layer);
    GRect rise = layout_get_rect(LAYOUT_SUNRISE);
    GRect set = layout_get_rect(LAYOUT_SUNSET);
    GSize set_size = graphics_text_layout_get_content_size(sl->set_buf, fonts_get_system_font(STATUS_SUN_FONT),
        GRect(0, 0, set.size.w, set.size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentRight);
    a->sunrise_x = rise.origin.x - SUN_ARROW_GAP;
    a->sunset_x = set.origin.x + set.size.w - set_size.w - SUN_ARROW_GAP;
    a->top_y = layout_get_sunrise_arrow_top().y;
    a->bottom_y = layout_get_sunrise_arrow_bottom().y;
    layer_mark_dirty(sl->sun_arrows_layer);
}

#if defined(LAYOUT_REFINED_STATUS)
typedef struct {
    GColor color;
    bool connected;
} BluetoothGlyph;

// The Bluetooth rune, 9px wide and 15px tall, drawn with pixel-aligned lines
// so it does not depend on any font. Disconnected: same rune, struck through.
static void bluetooth_update_proc(Layer* layer, GContext* ctx) {
    BluetoothGlyph* g = (BluetoothGlyph*)layer_get_data(layer);
    GRect b = layer_get_bounds(layer);
    int16_t x0 = b.origin.x, y0 = b.origin.y, cx = x0 + 4;
    graphics_context_set_stroke_color(ctx, g->color);
    graphics_draw_line(ctx, GPoint(cx, y0), GPoint(cx, y0 + 14));            // stem
    graphics_draw_line(ctx, GPoint(cx, y0), GPoint(x0 + 8, y0 + 4));         // upper bow
    graphics_draw_line(ctx, GPoint(x0 + 8, y0 + 4), GPoint(x0, y0 + 11));
    graphics_draw_line(ctx, GPoint(cx, y0 + 14), GPoint(x0 + 8, y0 + 10));   // lower bow
    graphics_draw_line(ctx, GPoint(x0 + 8, y0 + 10), GPoint(x0, y0 + 3));
    if (!g->connected) {
        graphics_draw_line(ctx, GPoint(x0 - 2, y0 + 15), GPoint(x0 + 10, y0 - 1));   // strike
    }
}
#endif

#if defined(PBL_HEALTH)
static void health_trend_update_proc(Layer* layer, GContext* ctx) {
    AppState* state = state_get_ptr();
    GlobalSettings* settings = settings_get_instance();
    if (settings->HealthInfo == 0 || settings->HealthInfo == 5) return;

    int trend = state->health.trend_display;
    if (trend == 0) return;

    const ColorScheme* scheme = colors_get_scheme(settings->ColorProfile);
    graphics_context_set_stroke_color(ctx, scheme->steps);

    GRect bounds = layer_get_bounds(layer);
    int mid_x = bounds.size.w / 2;
    if (trend > 0) {
        graphics_draw_line(ctx, GPoint(mid_x, bounds.size.h), GPoint(mid_x, 0));
        graphics_draw_line(ctx, GPoint(mid_x, 0), GPoint(1, 3));
        graphics_draw_line(ctx, GPoint(mid_x, 0), GPoint(bounds.size.w - 2, 3));
    } else {
        graphics_draw_line(ctx, GPoint(mid_x, 0), GPoint(mid_x, bounds.size.h));
        graphics_draw_line(ctx, GPoint(mid_x, bounds.size.h), GPoint(1, bounds.size.h - 3));
        graphics_draw_line(ctx, GPoint(mid_x, bounds.size.h), GPoint(bounds.size.w - 2, bounds.size.h - 3));
    }
}
#endif

static void get_remote_local_tm(struct tm* out_tm, time_t utc_time, int utc_offset_seconds) {
    time_t localized_time = utc_time + utc_offset_seconds;
    struct tm* remote_tm = gmtime(&localized_time);
    if (remote_tm) {
        *out_tm = *remote_tm;
    } else {
        memset(out_tm, 0, sizeof(*out_tm));
    }
}

static void format_battery_duration_mode0(char* buffer, size_t size, time_t seconds) {
    int days = seconds / (24 * 3600);
    int hours = (seconds % (24 * 3600)) / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    if (days == 0) {
        if (hours == 0) {
            if (minutes == 0) {
                snprintf(buffer, size, "%d s", secs);
            } else if (minutes < 10) {
                snprintf(buffer, size, "%dm %ds", minutes, secs);
            } else {
                snprintf(buffer, size, "%d m", minutes);
            }
        } else if (hours < 10) {
            snprintf(buffer, size, "%dh %dm", hours, minutes);
        } else {
            snprintf(buffer, size, "%d h", hours);
        }
    } else if (days < 10) {
        snprintf(buffer, size, "%dd %dh", days, hours);
    } else {
        snprintf(buffer, size, "%d d", days);
    }
}

// Narrow the inverted percentage's frame to the charge bar, keeping its
// bounds at the full text box so it renders in exactly the same place as
// the normal copy. Hidden when the bar is empty.
static void status_layer_clip_battery_fill_text(StatusLayer* sl) {
    AppState* state = state_get_ptr();
    GRect text = layout_get_rect(LAYOUT_BATTERY_TEXT);
    GRect fill = layout_get_battery_fill_rect(
        (LAYOUT_BATTERY_FILL_MAX_W * state->battery.charge_percent) / 100);
    int16_t clip_w = fill.origin.x + fill.size.w - text.origin.x;
    Layer* layer = text_layer_get_layer(sl->battery_fill_layer);
    layer_set_hidden(layer, fill.size.w <= 0);
    layer_set_frame(layer, GRect(text.origin.x, text.origin.y, clip_w, text.size.h));
    layer_set_bounds(layer, GRect(0, 0, text.size.w, text.size.h));
}

StatusLayer* status_layer_create(GRect frame) {
    StatusLayer* sl = malloc(sizeof(StatusLayer));
    sl->root_layer = layer_create(frame);
    sl->batt_buf[0] = '\0';
    sl->batt_time_buf[0] = '\0';

    sl->battery_layer = create_text_layer(layout_get_rect(LAYOUT_BATTERY_TEXT), STATUS_BATTERY_FONT, GTextAlignmentCenter, GColorWhite);
    text_layer_set_text(sl->battery_layer, "100%");
    // The inverted copy shares the full text box for layout (so the glyphs
    // line up exactly) but its frame is narrowed to the charge bar each update.
    sl->battery_fill_layer = create_text_layer(layout_get_rect(LAYOUT_BATTERY_TEXT), STATUS_BATTERY_FONT, GTextAlignmentCenter, GColorBlack);
    text_layer_set_text(sl->battery_fill_layer, "100%");
    sl->battery_time_layer = create_text_layer(layout_get_rect(LAYOUT_BATTERY_TIME), STATUS_BATTERY_TIME_FONT, GTextAlignmentCenter, GColorWhite);

    sl->connection_layer = create_text_layer(layout_get_rect(LAYOUT_CONNECTION), STATUS_SMALL_FONT, GTextAlignmentCenter, GColorWhite);
#if defined(LAYOUT_REFINED_STATUS)
    layer_set_hidden(text_layer_get_layer(sl->connection_layer), true);   // the glyph replaces the word
    sl->bluetooth_layer = layer_create_with_data(layout_get_rect(LAYOUT_CONNECTION), sizeof(BluetoothGlyph));
    *(BluetoothGlyph*)layer_get_data(sl->bluetooth_layer) = (BluetoothGlyph){ .color = GColorWhite, .connected = true };
    layer_set_update_proc(sl->bluetooth_layer, bluetooth_update_proc);
    layer_add_child(sl->root_layer, sl->bluetooth_layer);
#endif

    sl->sunrise_layer = create_text_layer(layout_get_rect(LAYOUT_SUNRISE), STATUS_SUN_FONT, GTextAlignmentLeft, GColorWhite);

    // Sunset is flush right so the row mirrors the sunrise on the left.
    sl->sunset_layer = create_text_layer(layout_get_rect(LAYOUT_SUNSET), STATUS_SUN_FONT, GTextAlignmentRight, GColorWhite);
    sl->timezone_layer = create_text_layer(layout_get_rect(LAYOUT_TIMEZONE), STATUS_MEDIUM_FONT,
                                           PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft), GColorWhite);
    
    sl->sun_arrows_layer = layer_create_with_data(frame, sizeof(SunArrows));
    SunArrows* arrows = (SunArrows*)layer_get_data(sl->sun_arrows_layer);
    *arrows = (SunArrows){ .color = GColorWhite };
    layer_set_update_proc(sl->sun_arrows_layer, sun_arrows_update_proc);
    layer_add_child(sl->root_layer, sl->sun_arrows_layer);

    layer_add_child(sl->root_layer, text_layer_get_layer(sl->battery_layer));
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->battery_fill_layer));
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->battery_time_layer));
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->connection_layer));
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->sunrise_layer));
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->sunset_layer));
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->timezone_layer));

#if defined(PBL_HEALTH)
    sl->health_icon_layer = bitmap_layer_create(layout_get_rect(LAYOUT_HEALTH_ICON));
    bitmap_layer_set_alignment(sl->health_icon_layer, GAlignCenter);
    layer_add_child(sl->root_layer, bitmap_layer_get_layer(sl->health_icon_layer));

    sl->health_text_layer = create_text_layer(layout_get_rect(LAYOUT_HEALTH_TEXT), STATUS_MEDIUM_FONT, GTextAlignmentLeft, GColorWhite);
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->health_text_layer));

    sl->health_icon_steps = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEALTH_STEPS);
    sl->health_icon_sleep = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEALTH_SLEEP);

    sl->health_trend_layer = layer_create(layout_get_rect(LAYOUT_HEALTH_TREND));
    layer_set_update_proc(sl->health_trend_layer, health_trend_update_proc);
    layer_add_child(sl->root_layer, sl->health_trend_layer);
#endif
    
    return sl;
}

void status_layer_destroy(StatusLayer* sl) {
    text_layer_destroy(sl->battery_layer);
    text_layer_destroy(sl->battery_fill_layer);
    text_layer_destroy(sl->battery_time_layer);
    text_layer_destroy(sl->connection_layer);
#if defined(LAYOUT_REFINED_STATUS)
    layer_destroy(sl->bluetooth_layer);
#endif
    text_layer_destroy(sl->sunrise_layer);
    text_layer_destroy(sl->sunset_layer);
    layer_destroy(sl->sun_arrows_layer);
    text_layer_destroy(sl->timezone_layer);
#if defined(PBL_HEALTH)
    text_layer_destroy(sl->health_text_layer);
    bitmap_layer_destroy(sl->health_icon_layer);
    gbitmap_destroy(sl->health_icon_steps);
    gbitmap_destroy(sl->health_icon_sleep);
    layer_destroy(sl->health_trend_layer);
#endif
    layer_destroy(sl->root_layer);
    free(sl);
}

Layer* status_layer_get_layer(StatusLayer* sl) {
    return sl->root_layer;
}

void status_layer_update(StatusLayer* sl) {
    AppState* state = state_get_ptr();
    GlobalSettings* settings = settings_get_instance();
    
    // Battery percentage (inside box)
    time_t now = time(NULL);
    time_t battery_duration = 0;
    if (state->battery.charge_state == 2) {
        battery_duration = state->battery.last_period_duration;
    } else {
        time_t start = state->battery.last_state_change_time;
        if (start <= 0) {
            start = now;
        }
        battery_duration = now - start;
        if (battery_duration < 0) {
            battery_duration = 0;
        }
    }
    format_battery_duration_mode0(sl->batt_time_buf, sizeof(sl->batt_time_buf), battery_duration);
    if (state->battery.charge_state == 1) {
        snprintf(sl->batt_buf, sizeof(sl->batt_buf), "*%d%%", state->battery.charge_percent);
    } else {
        snprintf(sl->batt_buf, sizeof(sl->batt_buf), "%d%%", state->battery.charge_percent);
    }
    text_layer_set_text(sl->battery_time_layer, sl->batt_time_buf);
    layer_set_hidden(text_layer_get_layer(sl->battery_time_layer), settings->HideBatteryTime != 0);
    text_layer_set_text(sl->battery_layer, sl->batt_buf);
    text_layer_set_text(sl->battery_fill_layer, sl->batt_buf);
    status_layer_clip_battery_fill_text(sl);

    BatteryPalette battery_palette;
    battery_style_get_palette(settings, state->battery.charge_percent, &battery_palette);
    text_layer_set_text_color(sl->battery_layer, battery_palette.text_color);
    text_layer_set_text_color(sl->battery_fill_layer, battery_palette.fill_text_color);
    text_layer_set_text_color(sl->battery_time_layer, battery_palette.text_color);
    
    // Connection - hide if settings say so
    if (settings->HideBluetooth && state->connection.bluetooth_connected) {
        text_layer_set_text(sl->connection_layer, "");
    } else if (state->connection.bluetooth_connected) {
        text_layer_set_text(sl->connection_layer, "Bluetooth");
    } else {
        text_layer_set_text(sl->connection_layer, "---------");
    }
#if defined(LAYOUT_REFINED_STATUS)
    {
        BluetoothGlyph* g = (BluetoothGlyph*)layer_get_data(sl->bluetooth_layer);
        g->connected = state->connection.bluetooth_connected;
        layer_set_hidden(sl->bluetooth_layer, settings->HideBluetooth && state->connection.bluetooth_connected);
        layer_mark_dirty(sl->bluetooth_layer);
    }
#endif
    
    // Sunrise/Sunset - mask sentinel values
    bool is_24h = clock_is_24h_style();
    
    if (state->weather.sunrise > 0) {
        format_sun_time(sl->rise_buf, sizeof(sl->rise_buf), state->weather.sunrise,
                        state->weather.timezone_utc_offset, is_24h);
    } else {
        snprintf(sl->rise_buf, sizeof(sl->rise_buf), "--:--");
    }
    
    if (state->weather.sunset > 0) {
        format_sun_time(sl->set_buf, sizeof(sl->set_buf), state->weather.sunset,
                        state->weather.timezone_utc_offset, is_24h);
    } else {
        snprintf(sl->set_buf, sizeof(sl->set_buf), "--:--");
    }
    
    text_layer_set_text(sl->sunrise_layer, sl->rise_buf);
    text_layer_set_text(sl->sunset_layer, sl->set_buf);
    status_layer_place_sun_arrows(sl);
    
    // Timezone or Health info
    // HealthInfo: 0=Off, 1=Steps/Sleep auto, 3=Steps, 4=Sleep, 5=Timezone
    bool show_timezone = (settings->HealthInfo == 5);
    layer_set_hidden(text_layer_get_layer(sl->timezone_layer), !show_timezone);
#if defined(PBL_HEALTH)
    bool show_health = (settings->HealthInfo != 0 && settings->HealthInfo != 5);
    layer_set_hidden(text_layer_get_layer(sl->health_text_layer), !show_health);
    layer_set_hidden(bitmap_layer_get_layer(sl->health_icon_layer), !show_health);
    layer_set_hidden(sl->health_trend_layer, !show_health);
#endif

    // Timezone based on format setting
    if (show_timezone) {
        if (settings->TimeZoneFormat == 0) {
        // UTC time
        time_t now = time(NULL);
        struct tm* utc = gmtime(&now);
        strftime(sl->tz_buf, sizeof(sl->tz_buf), "%R UTC", utc);
        } else if (settings->TimeZoneFormat == 1) {
        // Timezone name only
        if (state->weather.timezone_name[0] != '\0') {
            snprintf(sl->tz_buf, sizeof(sl->tz_buf), "%s", state->weather.timezone_name);
        } else {
            snprintf(sl->tz_buf, sizeof(sl->tz_buf), "TZ");
        }
        } else {
        // 12H/24H + timezone name
        time_t now = time(NULL);
        struct tm remote_local;
        get_remote_local_tm(&remote_local, now, state->weather.timezone_utc_offset);
        const char* mode = "24H";
        if (!clock_is_24h_style()) {
            mode = (remote_local.tm_hour >= 12) ? "PM" : "AM";
        }
        if (state->weather.timezone_name[0] != '\0') {
            snprintf(sl->tz_buf, sizeof(sl->tz_buf), "%s, %s", mode, state->weather.timezone_name);
        } else {
            snprintf(sl->tz_buf, sizeof(sl->tz_buf), "%s", mode);
        }
        }
        text_layer_set_text(sl->timezone_layer, sl->tz_buf);
#if defined(LAYOUT_REFINED_STATUS) && !defined(PBL_ROUND)
        {
            GRect tz = layout_get_rect(LAYOUT_TIMEZONE);
            bool centred = settings->ExtraInfoCenter != 0;
            // Centred: span the row (the week has moved to the left slot; seconds stay right)
            layer_set_frame(text_layer_get_layer(sl->timezone_layer),
                            centred ? GRect(60, tz.origin.y, 80, tz.size.h) : tz);
            text_layer_set_text_alignment(sl->timezone_layer, centred ? GTextAlignmentCenter : GTextAlignmentLeft);
        }
#endif
    }

#if defined(PBL_HEALTH)
    if (!show_timezone) {
        bool show_sleep = false;
        if (settings->HealthInfo == 4) {
            show_sleep = true;
        } else if (settings->HealthInfo == 1) {
            // Last night's sleep in the morning, steps for the rest of the day
            time_t now_t = time(NULL);
            struct tm* lt = localtime(&now_t);
            show_sleep = lt && rules_show_sleep(lt->tm_hour, settings->SleepUntilHour);
        }
        if (show_sleep) {
            if (state->health.sleep_seconds < 0) {
                snprintf(sl->health_buf, sizeof(sl->health_buf), "--");   // no health data yet
            } else {
                format_time_elapsed(sl->health_buf, sizeof(sl->health_buf), state->health.sleep_seconds);
            }
            bitmap_layer_set_bitmap(sl->health_icon_layer, sl->health_icon_sleep);
            state->health.trend_display = state->health.sleep_trend;
        } else {
            if (state->health.steps < 0) {
                snprintf(sl->health_buf, sizeof(sl->health_buf), "--");   // no health data yet
            } else {
                snprintf(sl->health_buf, sizeof(sl->health_buf), "%d", state->health.steps);
            }
            bitmap_layer_set_bitmap(sl->health_icon_layer, sl->health_icon_steps);
            state->health.trend_display = state->health.steps_trend;
        }
        text_layer_set_text(sl->health_text_layer, sl->health_buf);
#if defined(LAYOUT_REFINED_STATUS)
        // Icon, value, arrow measured as one group so 8432 and 18432 both keep
        // the same gaps. Emery anchors it on the left inset; Gabbro centres it
        // in the bottom cap.
        {
            GRect text = layout_get_rect(LAYOUT_HEALTH_TEXT);
            GRect trend = layout_get_rect(LAYOUT_HEALTH_TREND);
            GSize size = graphics_text_layout_get_content_size(sl->health_buf, fonts_get_system_font(STATUS_MEDIUM_FONT),
                GRect(0, 0, text.size.w, text.size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
            const int16_t gap = 3;
            GRect icon = layout_get_rect(LAYOUT_HEALTH_ICON);
            // Round: always centred in the bottom cap. Emery: centred in the
            // status row when the user asks for it, otherwise on the left inset.
            bool centred = PBL_IF_ROUND_ELSE(true, settings->ExtraInfoCenter != 0);
            if (centred) {
                int16_t total = icon.size.w + gap + size.w + gap + trend.size.w;
                int16_t x0 = layer_get_bounds(sl->root_layer).size.w / 2 - total / 2;
                layer_set_frame(bitmap_layer_get_layer(sl->health_icon_layer), GRect(x0, icon.origin.y, icon.size.w, icon.size.h));
                layer_set_frame(text_layer_get_layer(sl->health_text_layer), GRect(x0 + icon.size.w + gap, text.origin.y, size.w + 2, text.size.h));
                layer_set_frame(sl->health_trend_layer, GRect(x0 + icon.size.w + gap + size.w + gap, trend.origin.y, trend.size.w, trend.size.h));
            } else {
                layer_set_frame(bitmap_layer_get_layer(sl->health_icon_layer), icon);
                layer_set_frame(text_layer_get_layer(sl->health_text_layer), text);
                int16_t x = text.origin.x + size.w + gap;
                if (x + trend.size.w > text.origin.x + text.size.w) x = text.origin.x + text.size.w - trend.size.w;
                layer_set_frame(sl->health_trend_layer, GRect(x, trend.origin.y, trend.size.w, trend.size.h));
            }
        }
#endif
        layer_mark_dirty(sl->health_trend_layer);
    }
#endif
}

void status_layer_update_colors(StatusLayer* sl) {
    GlobalSettings* settings = settings_get_instance();
    const ColorScheme* scheme = colors_get_scheme(settings->ColorProfile);
    AppState* state = state_get_ptr();
    
    // Connection color: red if disconnected
    GColor connection_color = state->connection.bluetooth_connected
        ? scheme->connection
        // Red vanishes on black-and-white screens; the dashes carry the message there
        : PBL_IF_COLOR_ELSE(GColorRed, scheme->connection);
    text_layer_set_text_color(sl->connection_layer, connection_color);
#if defined(LAYOUT_REFINED_STATUS)
    ((BluetoothGlyph*)layer_get_data(sl->bluetooth_layer))->color = connection_color;
    layer_mark_dirty(sl->bluetooth_layer);
#endif
    
    text_layer_set_text_color(sl->sunrise_layer, scheme->sun);
    text_layer_set_text_color(sl->sunset_layer, scheme->sun);
    ((SunArrows*)layer_get_data(sl->sun_arrows_layer))->color = scheme->sun;
    layer_mark_dirty(sl->sun_arrows_layer);
    text_layer_set_text_color(sl->timezone_layer, scheme->timezone);

    BatteryPalette battery_palette;
    battery_style_get_palette(settings, state->battery.charge_percent, &battery_palette);
    text_layer_set_text_color(sl->battery_layer, battery_palette.text_color);
    text_layer_set_text_color(sl->battery_fill_layer, battery_palette.fill_text_color);
    text_layer_set_text_color(sl->battery_time_layer, battery_palette.text_color);
#if defined(PBL_HEALTH)
    text_layer_set_text_color(sl->health_text_layer, scheme->steps);
#endif
}
