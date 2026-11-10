#include <pebble.h>
#include <locale.h>
#include <string.h>

#include "state/state.h"
#include "state/events.h"
#include "state/settings.h"
#include "config.h"

#include "modules/comm.h"
#include "modules/weather.h"
#include "modules/battery.h"
#include "modules/health.h"
#include "modules/connection.h"
#include "modules/colors.h"
#include "modules/rules.h"

#include "ui/layers/background.h"
#include "ui/layers/time_layer.h"
#include "ui/layers/date_layer.h"
#include "ui/layers/weather_layer.h"
#include "ui/layers/status_layer.h"
#include "ui/layers/heart_layer.h"
#include "ui/layout.h"

// UI Components
static Window *s_main_window;
static Layer *s_content_layer;   // Holds every face layer; slides up while a Timeline Peek covers the bottom
static BackgroundLayer *s_bg_layer;
static TimeLayer *s_time_layer;
static DateLayer *s_date_layer;
static WeatherLayer *s_weather_layer;
static StatusLayer *s_status_layer;
static HeartLayer *s_heart_layer;

// Shake-to-show seconds state
static bool s_seconds_visible = false;
static int s_seconds_timeout_counter = 0;
static int s_last_display_seconds = -1;
static bool s_light_on = false;
static AppTimer *s_blink_timer = NULL;   // Drives the "Blinking" backlight mode

// --- Backlight ---

// The tick handler only runs once a minute unless seconds are shown, so the
// blinking mode gets its own timer.
static void blink_timer_cb(void *context) {
    s_blink_timer = NULL;
    GlobalSettings* settings = settings_get_instance();
    if (settings->LightOn != 3) return;
    s_light_on = !s_light_on;
    light_enable(s_light_on);
    s_blink_timer = app_timer_register(1000, blink_timer_cb, NULL);
}

static void apply_backlight_mode() {
    GlobalSettings* settings = settings_get_instance();
    AppState* app_state = state_get_ptr();

    if (settings->LightOn == 3) {
        if (!s_blink_timer) {
            s_light_on = true;
            light_enable(true);
            s_blink_timer = app_timer_register(1000, blink_timer_cb, NULL);
        }
        return;
    }
    if (s_blink_timer) {
        app_timer_cancel(s_blink_timer);
        s_blink_timer = NULL;
    }

    bool want_on = (settings->LightOn == 2) ||
                   (settings->LightOn == 1 && app_state->battery.is_charging);
    if (want_on != s_light_on) {
        s_light_on = want_on;
        light_enable(want_on);
    }
}

// --- Event Processing ---

static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void process_events();
static void update_tick_timer_service();
static void update_date_and_week(struct tm *t);

static bool cw_layer_wanted(const GlobalSettings* settings) {
    // With ShowWeekInDate the week is part of the date string instead.
    return !settings->HideCW && !settings->ShowWeekInDate;
}

// Heart rate takes the week label's slot when it is turned on and the watch
// can measure it. Without a sensor (or with heart rate off in Pebble Health)
// the week label stays, so the setting never leaves the slot empty.
static bool heart_rate_wanted(const GlobalSettings* settings) {
    return settings->HeartRate && state_get_ptr()->health.heart_rate_available;
}

// On the round display the week slot and the seconds share the space
// between the sunrise and sunset times, so the slot yields while seconds show.
static void apply_cw_visibility() {
    GlobalSettings* settings = settings_get_instance();
    bool slot_free = true;
#if defined(PBL_ROUND)
    slot_free = !s_seconds_visible;
#endif
    bool heart = heart_rate_wanted(settings);
    if (s_date_layer) date_layer_set_cw_visible(s_date_layer, slot_free && !heart && cw_layer_wanted(settings));
    if (s_heart_layer) layer_set_hidden(heart_layer_get_layer(s_heart_layer), !(slot_free && heart));
}

static void update_heart_rate_ui() {
    if (s_heart_layer) heart_layer_set_bpm(s_heart_layer, state_get_ptr()->health.heart_rate_bpm);
    apply_cw_visibility();
}

// --- Timeline Peek ---

// A peek slides up over the bottom of the screen. Move the whole face up just
// enough to keep the clock clear of it; the top rows go off-screen instead,
// and the status row under the clock is what the peek covers. Called on
// every animation frame, so it follows the peek in and out.
#define PEEK_CLEARANCE 2

static void apply_peek_shift() {
    if (!s_content_layer) return;
    Layer *window_layer = window_get_root_layer(s_main_window);
    GRect full = layer_get_bounds(window_layer);
    GRect visible = layer_get_unobstructed_bounds(window_layer);
    int16_t visible_bottom = visible.origin.y + visible.size.h;
    int16_t shift = 0;
    if (visible_bottom < full.origin.y + full.size.h) {
        GRect digits = layout_get_time_digit_rect(DIGIT_H1);
        shift = digits.origin.y + digits.size.h + PEEK_CLEARANCE - visible_bottom;
        if (shift < 0) shift = 0;
        if (shift > digits.origin.y) shift = digits.origin.y;   // Never push the clock off the top
    }
    GRect frame = full;
    frame.origin.y -= shift;
    layer_set_frame(s_content_layer, frame);
}

static void peek_change(AnimationProgress progress, void *context) {
    apply_peek_shift();
}

static void peek_did_change(void *context) {
    apply_peek_shift();
}

static void set_seconds_visible(bool show) {
    if (s_time_layer) time_layer_set_show_seconds(s_time_layer, show);
    apply_cw_visibility();
}

static const char* get_locale_code() {
    static const char* s_locale = NULL;
    if (!s_locale) {
        s_locale = setlocale(LC_ALL, "");
        if (!s_locale) {
            s_locale = "";
        }
    }
    return s_locale;
}

static bool is_sep_char(char c) {
    return c == '.' || c == '/' || c == '-' || c == ' ' || c == ',';
}

static void strip_year_format(const char* src, char* dst, size_t dst_size) {
    size_t di = 0;
    for (size_t i = 0; src[i] && di + 1 < dst_size; ) {
        if (src[i] == '%' && (src[i + 1] == 'Y' || src[i + 1] == 'y')) {
            if (di > 0 && is_sep_char(dst[di - 1])) {
                di--;
            }
            i += 2;
            if (di == 0) {
                while (src[i] && is_sep_char(src[i])) {
                    i++;
                }
            }
            continue;
        }
        dst[di++] = src[i++];
    }
    dst[di] = '\0';
}

static int get_fiscal_week_number(struct tm* t, int start_month, int start_day) {
    struct tm start = *t;
    struct tm now = *t;
    start.tm_mon = start_month;
    start.tm_mday = start_day;
    start.tm_hour = 12;
    start.tm_min = 0;
    start.tm_sec = 0;
    now.tm_hour = 12;
    now.tm_min = 0;
    now.tm_sec = 0;

    time_t start_time = mktime(&start);
    time_t now_time = mktime(&now);
    if (now_time < start_time) {
        start.tm_year -= 1;
        start_time = mktime(&start);
    }
    int days = (int)((now_time - start_time) / SECONDS_PER_DAY);
    return (days / 7) + 1;
}

static void format_week_text(char* buffer, size_t size, struct tm* t, const GlobalSettings* settings) {
    if (settings->WeekNumberType == WEEK_TYPE_FISCAL_RETAIL) {
        snprintf(buffer, size, "WK%02d", rules_retail_fiscal_week(t->tm_year + 1900, t->tm_yday, t->tm_wday));
        return;
    }
    if (settings->WeekNumberType == WEEK_TYPE_FISCAL_DATE) {
        int month = settings->FiscalStartMonth;
        int day = settings->FiscalStartDay;
        if (month < 0) month = 0;
        if (month > 11) month = 11;
        if (day < 1) day = 1;
        if (day > 31) day = 31;
        int week = get_fiscal_week_number(t, month, day);
        snprintf(buffer, size, "WK%02d", week);
        return;
    }

    const char* locale = get_locale_code();
    if (strncmp(locale, "fr_FR", 5) == 0) {
        strftime(buffer, size, TRANSLATION_CW_FR, t);
    } else if (strncmp(locale, "de_DE", 5) == 0) {
        strftime(buffer, size, TRANSLATION_CW_DE, t);
    } else {
        strftime(buffer, size, TRANSLATION_CW_EN, t);
    }
}

static void update_tick_timer_service() {
    GlobalSettings* settings = settings_get_instance();
    TimeUnits units = MINUTE_UNIT;
    
    // DisplaySeconds: 0=off, 1=always, 2+=shake modes with different timeouts
    if (settings->DisplaySeconds != s_last_display_seconds) {
        if (settings->DisplaySeconds >= 2) {
            s_seconds_visible = false;
            s_seconds_timeout_counter = 0;
        } else if (settings->DisplaySeconds == 0) {
            s_seconds_visible = false;
            s_seconds_timeout_counter = 0;
        } else if (settings->DisplaySeconds == 1) {
            s_seconds_visible = true;
        }
        s_last_display_seconds = settings->DisplaySeconds;
    }

    if (settings->DisplaySeconds == 1) {
        // Always show seconds
        units |= SECOND_UNIT;
        s_seconds_visible = true;
        set_seconds_visible(true);
    } else if (settings->DisplaySeconds >= 2 && s_seconds_visible) {
        // Shake mode and seconds currently visible
        units |= SECOND_UNIT;
        set_seconds_visible(true);
    } else if (settings->DisplaySeconds == 0) {
        // Off
        s_seconds_visible = false;
        set_seconds_visible(false);
    } else {
        // Shake mode but currently hidden
        set_seconds_visible(false);
    }
    
    tick_timer_service_subscribe(units, tick_handler);
}

// Tap handler for shake-to-show seconds
static void tap_handler(AccelAxisType axis, int32_t direction) {
    GlobalSettings* settings = settings_get_instance();
    
    if (settings->DisplaySeconds < 2) return; // Not in shake mode
    
    s_seconds_visible = true;
    s_seconds_timeout_counter = 0;
    set_seconds_visible(true);
    
    // Subscribe to seconds
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    
    // Force immediate update
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int hour = t->tm_hour;
    if (!clock_is_24h_style()) {
        hour = hour % 12;
        if (hour == 0) hour = 12;
    }
    time_layer_update(s_time_layer, hour, t->tm_min, t->tm_sec);
}

// Get timeout based on DisplaySeconds setting
static int get_seconds_timeout() {
    GlobalSettings* settings = settings_get_instance();
    switch (settings->DisplaySeconds) {
        case 2: return 5;   // 5 seconds
        case 3: return 15;  // 15 seconds
        case 4: return 30;  // 30 seconds
        case 5: return 60;  // 60 seconds
        default: return 15;
    }
}

// Helper to update colors for all layers
static void update_colors_all() {
    GlobalSettings *settings = settings_get_instance();
    const ColorScheme *scheme = colors_get_scheme(settings->ColorProfile);
    
    // Only ever seen under a peek, if at all; match the face rather than show white
    window_set_background_color(s_main_window, scheme->clock_bg);
    background_layer_mark_dirty(s_bg_layer);
    time_layer_update_colors(s_time_layer, scheme->clock);
    date_layer_update_color(s_date_layer, scheme->date);
    // Same colour as the week label it replaces; a red heart where the screen has colour
    heart_layer_set_colors(s_heart_layer, scheme->date,
        PBL_IF_COLOR_ELSE(gcolor_equal(scheme->clock_bg, GColorRed) ? scheme->date : GColorRed, scheme->date));
    weather_layer_update_colors(s_weather_layer);
    status_layer_update_colors(s_status_layer);
}

static void update_all_ui() {
    update_colors_all();
    time_layer_set_leading_zero(s_time_layer, settings_get_instance()->LeadingZero != 0);
    date_layer_set_cw_left(s_date_layer, settings_get_instance()->ExtraInfoCenter != 0);
    heart_layer_set_left(s_heart_layer, settings_get_instance()->ExtraInfoCenter != 0);
    
    // Time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    int hour = t->tm_hour;
    if (!clock_is_24h_style()) {
        hour = hour % 12;
        if (hour == 0) hour = 12;
    }
    time_layer_update(s_time_layer, hour, t->tm_min, t->tm_sec);
    
    update_date_and_week(t);
    
    // Data Layers
    weather_layer_update_data(s_weather_layer);
    status_layer_update(s_status_layer);
    update_heart_rate_ui();
    
    layer_mark_dirty(window_get_root_layer(s_main_window));
}

static void process_events() {
    EventFlag flags = events_get_and_clear_flags();
    if (flags == EVENT_NONE) return;

    if (flags & EVENT_SETTINGS_CHANGED) {
        GlobalSettings* settings = settings_get_instance();
        APP_LOG(APP_LOG_LEVEL_INFO, "Settings changed: DisplaySec=%d HealthInfo=%d", 
                settings->DisplaySeconds, settings->HealthInfo);
        update_tick_timer_service();
        apply_backlight_mode();
        update_all_ui(); 
    }
    
    if (flags & EVENT_TIME_TICK) {
        time_t now = time(NULL);
        struct tm *safe_t = localtime(&now);
        struct tm time_copy = *safe_t;
        struct tm *t = &time_copy;
        int hour = t->tm_hour;
        if (!clock_is_24h_style()) {
            hour = hour % 12;
            if (hour == 0) hour = 12;
        }
        time_layer_update(s_time_layer, hour, t->tm_min, t->tm_sec);
        
        GlobalSettings *settings = settings_get_instance();

        // Backlight (blinking runs on its own timer, see apply_backlight_mode)
        apply_backlight_mode();

        // Hourly vibration
        if (settings->vibe_on_hour && t->tm_min == 0 && t->tm_sec == 0) {
            vibes_short_pulse();
        }
        
        // Shake-to-show timeout check
        if (settings->DisplaySeconds >= 2 && s_seconds_visible) {
            s_seconds_timeout_counter++;
            if (s_seconds_timeout_counter > get_seconds_timeout()) {
                s_seconds_visible = false;
                set_seconds_visible(false);
                // Switch back to minute updates
                tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
            }
        }
        
        // Weather staleness and refresh
        weather_check_staleness();

        // Update elapsed time and day/night checks ONCE per new minute
        static int s_last_run_minute = -1;
        if (t->tm_min != s_last_run_minute) {
            s_last_run_minute = t->tm_min;

            // Update date/week only on minute change (throttled from per-second)
            update_date_and_week(t);

            // A reading older than 15 minutes expires without any health
            // event, so look again once a minute
            health_update_heart_rate();

            // Weather auto-update
            int interval = settings->WeatherUpdateInterval;
            if (interval > 0 && t->tm_min % interval == 0) {
                weather_request_update();
            }
        }

        // Single weather/status refresh per tick (covers staleness, minute change, elapsed time)
        weather_layer_update_data(s_weather_layer);
        status_layer_update(s_status_layer);
    }

    if (flags & EVENT_WEATHER_DATA) {
        if (!(flags & EVENT_TIME_TICK)) {
            weather_layer_update_data(s_weather_layer);
            status_layer_update(s_status_layer);
        }
    }

    // Some colours follow the data rather than the settings: the stale / failed
    // header, the temperature, the icon at sunset and sunrise, and the
    // Bluetooth mark. Refresh them whenever what they depend on may have
    // changed, not only when the settings do. (Setting a colour a layer
    // already has costs nothing.)
    if (flags & (EVENT_TIME_TICK | EVENT_WEATHER_DATA)) {
        weather_layer_update_colors(s_weather_layer);
    }
    if (flags & EVENT_CONNECTION) {
        status_layer_update_colors(s_status_layer);
    }
    
    if (flags & EVENT_BATTERY_STATE) {
        background_layer_mark_dirty(s_bg_layer);
    }

    if ((flags & (EVENT_BATTERY_STATE | EVENT_CONNECTION | EVENT_HEALTH_UPDATE)) && !(flags & EVENT_TIME_TICK)) {
        status_layer_update(s_status_layer);
    }

    if (flags & (EVENT_TIME_TICK | EVENT_HEALTH_UPDATE)) {
        update_heart_rate_ui();
    }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    events_set_flag(EVENT_TIME_TICK);
    process_events();
}

static void update_date_and_week(struct tm *t) {
    GlobalSettings *settings = settings_get_instance();
    bool show_week = !settings->HideCW;
    static char week_buffer[8];
    week_buffer[0] = '\0';

    if (show_week) {
        format_week_text(week_buffer, sizeof(week_buffer), t, settings);
    }

    if (settings->ShowWeekInDate && show_week) {
        static char date_format_no_year[24];
        strip_year_format(settings->date_format, date_format_no_year, sizeof(date_format_no_year));
        if (date_format_no_year[0] == '\0') {
            snprintf(date_format_no_year, sizeof(date_format_no_year), "%%a, %%d.%%m");
        }
        static char date_buffer[BUFFER_SIZE_LARGE];
        strftime(date_buffer, sizeof(date_buffer), date_format_no_year, t);
        static char date_with_week[BUFFER_SIZE_LARGE];
        size_t week_len = strlen(week_buffer);
        size_t max_date_len = (sizeof(date_with_week) > week_len + 2)
            ? sizeof(date_with_week) - week_len - 2
            : 0;
        snprintf(date_with_week, sizeof(date_with_week), "%.*s %s", (int)max_date_len, date_buffer, week_buffer);
        date_layer_update_text(s_date_layer, date_with_week);
        apply_cw_visibility();
    } else {
        static char date_buffer[BUFFER_SIZE_LARGE];
        strftime(date_buffer, sizeof(date_buffer), settings->date_format, t);
        date_layer_update_text(s_date_layer, date_buffer);
        if (show_week) {
            date_layer_update_cw(s_date_layer, week_buffer);
        }
        apply_cw_visibility();
    }
}

// --- Window Lifecycle ---

static void main_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // Every layer goes in one full-screen container, so a Timeline Peek can
    // move the whole face at once (apply_peek_shift)
    s_content_layer = layer_create(bounds);
    layer_add_child(window_layer, s_content_layer);
    Layer *root = s_content_layer;
    
    // Initialize dynamic layout engine
    layout_init(bounds);
    
    // Background
    s_bg_layer = background_layer_create(bounds);
    layer_add_child(root, background_layer_get_layer(s_bg_layer));
    
    // Time
    s_time_layer = time_layer_create(bounds);
    layer_add_child(root, time_layer_get_layer(s_time_layer));
    
    // Date 
    s_date_layer = date_layer_create(bounds);
    layer_add_child(root, date_layer_get_layer(s_date_layer));

    // Heart rate, in the week label's slot
    s_heart_layer = heart_layer_create();
    layer_set_hidden(heart_layer_get_layer(s_heart_layer), true);
    layer_add_child(root, heart_layer_get_layer(s_heart_layer));
    
    // Weather
    s_weather_layer = weather_layer_create(bounds);
    layer_add_child(root, weather_layer_get_layer(s_weather_layer));
    
    // Status
    s_status_layer = status_layer_create(bounds);
    layer_add_child(root, status_layer_get_layer(s_status_layer));
    
    // Initial Update
    update_all_ui();

    // Apply seconds visibility once layers exist
    update_tick_timer_service();

    // Follow Timeline Peeks, and catch one that is already showing
    unobstructed_area_service_subscribe((UnobstructedAreaHandlers) {
        .change = peek_change,
        .did_change = peek_did_change,
    }, NULL);
    apply_peek_shift();
}

static void main_window_unload(Window *window) {
    unobstructed_area_service_unsubscribe();
    background_layer_destroy(s_bg_layer);
    time_layer_destroy(s_time_layer);
    date_layer_destroy(s_date_layer);
    heart_layer_destroy(s_heart_layer);
    s_heart_layer = NULL;
    weather_layer_destroy(s_weather_layer);
    status_layer_destroy(s_status_layer);
    layer_destroy(s_content_layer);
    s_content_layer = NULL;
}

static void init() {
    state_init();
    settings_load();
    colors_init();
    weather_init();
    battery_module_init();
    health_module_init();
    connection_module_init();
    events_init();
    
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    window_stack_push(s_main_window, true);
    
    comm_init(process_events);
    
    // Setup Tick Timer with correct units
    update_tick_timer_service();
    
    // Register tap handler for shake-to-show seconds
    accel_tap_service_subscribe(tap_handler);
    
    // Fetch at launch only when the cached weather is due for a refresh. The
    // face restarts every time the user comes back from a menu or an app, and
    // each fetch costs the phone a GPS fix and two web requests.
    if (weather_needs_refresh(settings_get_instance()->WeatherUpdateInterval)) {
        weather_request_update();
    }
}

static void deinit() {
    if (s_blink_timer) {
        app_timer_cancel(s_blink_timer);
        s_blink_timer = NULL;
    }
    light_enable(false);
    accel_tap_service_unsubscribe();
    window_destroy(s_main_window);
    
    comm_deinit();
    connection_module_deinit();
    health_module_deinit();
    battery_module_deinit();
    weather_deinit();
    settings_save();
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
