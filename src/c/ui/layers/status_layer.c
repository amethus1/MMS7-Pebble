#include "status_layer.h"
#include <string.h>
#include "../../state/state.h"
#include "../../state/settings.h"
#include "../formatting.h"
#include "../../modules/colors.h"
#include "../../modules/weather.h"
#include "../battery_style.h"
#include "../layout.h"

struct StatusLayer {
    Layer* root_layer;
    TextLayer* battery_layer;
#if defined(LAYOUT_LARGE_DISPLAY)
    TextLayer* battery_time_layer;
#endif
    TextLayer* connection_layer;
    TextLayer* sunrise_layer;
    TextLayer* sunset_layer;
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

#if defined(LAYOUT_LARGE_DISPLAY)
#define STATUS_BATTERY_FONT FONT_KEY_GOTHIC_18_BOLD
#define STATUS_BATTERY_TIME_FONT FONT_KEY_GOTHIC_14
#define STATUS_SMALL_FONT FONT_KEY_GOTHIC_18
// On Gabbro the timezone / health slot is in the bottom cap of the circle (~100px wide).
#define STATUS_MEDIUM_FONT PBL_IF_ROUND_ELSE(FONT_KEY_GOTHIC_18, FONT_KEY_GOTHIC_24)
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

StatusLayer* status_layer_create(GRect frame) {
    StatusLayer* sl = malloc(sizeof(StatusLayer));
    sl->root_layer = layer_create(frame);
    sl->batt_buf[0] = '\0';
    sl->batt_time_buf[0] = '\0';

#if defined(LAYOUT_LARGE_DISPLAY)
    sl->battery_layer = create_text_layer(layout_get_rect(LAYOUT_BATTERY_TEXT), STATUS_BATTERY_FONT, GTextAlignmentCenter, GColorWhite);
    text_layer_set_text(sl->battery_layer, "100%");
    sl->battery_time_layer = create_text_layer(layout_get_rect(LAYOUT_BATTERY_TIME), STATUS_BATTERY_TIME_FONT, GTextAlignmentCenter, GColorWhite);
#else
    sl->battery_layer = create_text_layer(layout_get_rect(LAYOUT_BATTERY_TEXT), STATUS_BATTERY_FONT, GTextAlignmentCenter, GColorWhite);
    text_layer_set_overflow_mode(sl->battery_layer, GTextOverflowModeWordWrap);
    text_layer_set_text(sl->battery_layer, "100%\n0:00 d");
#endif

    sl->connection_layer = create_text_layer(layout_get_rect(LAYOUT_CONNECTION), STATUS_SMALL_FONT, GTextAlignmentCenter, GColorWhite);

    sl->sunrise_layer = create_text_layer(layout_get_rect(LAYOUT_SUNRISE), STATUS_SMALL_FONT, GTextAlignmentLeft, GColorWhite);

    sl->sunset_layer = create_text_layer(layout_get_rect(LAYOUT_SUNSET), STATUS_SMALL_FONT, GTextAlignmentLeft, GColorWhite);
    sl->timezone_layer = create_text_layer(layout_get_rect(LAYOUT_TIMEZONE), STATUS_MEDIUM_FONT,
                                           PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft), GColorWhite);
    
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->battery_layer));
#if defined(LAYOUT_LARGE_DISPLAY)
    layer_add_child(sl->root_layer, text_layer_get_layer(sl->battery_time_layer));
#endif
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
#if defined(LAYOUT_LARGE_DISPLAY)
    text_layer_destroy(sl->battery_time_layer);
#endif
    text_layer_destroy(sl->connection_layer);
    text_layer_destroy(sl->sunrise_layer);
    text_layer_destroy(sl->sunset_layer);
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
#if defined(LAYOUT_LARGE_DISPLAY)
    if (state->battery.charge_state == 1) {
        snprintf(sl->batt_buf, sizeof(sl->batt_buf), "*%d%%", state->battery.charge_percent);
    } else {
        snprintf(sl->batt_buf, sizeof(sl->batt_buf), "%d%%", state->battery.charge_percent);
    }
    text_layer_set_text(sl->battery_time_layer, sl->batt_time_buf);
#else
    if (state->battery.charge_state == 1) {
        snprintf(sl->batt_buf, sizeof(sl->batt_buf), "*%d%%\n%s", state->battery.charge_percent, sl->batt_time_buf);
    } else {
        snprintf(sl->batt_buf, sizeof(sl->batt_buf), "%d%%\n%s", state->battery.charge_percent, sl->batt_time_buf);
    }
#endif
    text_layer_set_text(sl->battery_layer, sl->batt_buf);

    BatteryPalette battery_palette;
    battery_style_get_palette(settings, state->battery.charge_percent, &battery_palette);
    text_layer_set_text_color(sl->battery_layer, battery_palette.text_color);
#if defined(LAYOUT_LARGE_DISPLAY)
    text_layer_set_text_color(sl->battery_time_layer, battery_palette.text_color);
#endif
    
    // Connection - hide if settings say so
    if (settings->HideBluetooth && state->connection.bluetooth_connected) {
        text_layer_set_text(sl->connection_layer, "");
    } else if (state->connection.bluetooth_connected) {
        text_layer_set_text(sl->connection_layer, "Bluetooth");
    } else {
        text_layer_set_text(sl->connection_layer, "---------");
    }
    
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
    }

#if defined(PBL_HEALTH)
    if (!show_timezone) {
        bool show_sleep = false;
        if (settings->HealthInfo == 4) {
            show_sleep = true;
        } else if (settings->HealthInfo == 1) {
            show_sleep = weather_is_night();
        }
        if (show_sleep) {
            format_time_elapsed(sl->health_buf, sizeof(sl->health_buf), state->health.sleep_seconds);
            bitmap_layer_set_bitmap(sl->health_icon_layer, sl->health_icon_sleep);
            state->health.trend_display = state->health.sleep_trend;
        } else {
            snprintf(sl->health_buf, sizeof(sl->health_buf), "%d", state->health.steps);
            bitmap_layer_set_bitmap(sl->health_icon_layer, sl->health_icon_steps);
            state->health.trend_display = state->health.steps_trend;
        }
        text_layer_set_text(sl->health_text_layer, sl->health_buf);
        layer_mark_dirty(sl->health_trend_layer);
    }
#endif
}

void status_layer_update_colors(StatusLayer* sl) {
    GlobalSettings* settings = settings_get_instance();
    const ColorScheme* scheme = colors_get_scheme(settings->ColorProfile);
    AppState* state = state_get_ptr();
    
    // Connection color: red if disconnected
    if (!state->connection.bluetooth_connected) {
        text_layer_set_text_color(sl->connection_layer, GColorRed);
    } else {
        text_layer_set_text_color(sl->connection_layer, scheme->connection);
    }
    
    text_layer_set_text_color(sl->sunrise_layer, scheme->sun);
    text_layer_set_text_color(sl->sunset_layer, scheme->sun);
    text_layer_set_text_color(sl->timezone_layer, scheme->timezone);

    BatteryPalette battery_palette;
    battery_style_get_palette(settings, state->battery.charge_percent, &battery_palette);
    text_layer_set_text_color(sl->battery_layer, battery_palette.text_color);
#if defined(LAYOUT_LARGE_DISPLAY)
    text_layer_set_text_color(sl->battery_time_layer, battery_palette.text_color);
#endif
#if defined(PBL_HEALTH)
    text_layer_set_text_color(sl->health_text_layer, scheme->steps);
#endif
}
