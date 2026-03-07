#include "weather_layer.h"
#include "../../state/state.h"
#include "../../state/settings.h"
#include "../formatting.h"
#include "../../modules/colors.h"
#include "../../modules/weather.h"
#include "../../modules/weather_format.h"
#include "../../mooncalc.h"
#include "../layout.h"

struct WeatherLayer {
    Layer* root_layer;
    TextLayer* icon_layer;
    TextLayer* temp_layer;
    TextLayer* location_layer;
    TextLayer* last_update_layer;
    TextLayer* string_1_layer;
    TextLayer* string_2_layer;
    TextLayer* string_3_layer;
    // Text buffers (moved from function-static for memory safety)
    char temp_buffer[12];
    char icon_buffer[2];
    char moon_buffer[2];
    char update_buffer[12];
    char info_line_1[BUFFER_SIZE_MEDIUM];
    char info_line_2[BUFFER_SIZE_MEDIUM];
    char info_line_3[BUFFER_SIZE_MEDIUM];
    char info_line_4[BUFFER_SIZE_MEDIUM];
    char left_info_buffer[70];
};

// Custom font for weather icons
static GFont s_weather_font = NULL;
static GFont s_moon_font = NULL;

static GColor temperature_color(int temp_c) {
#if defined(PBL_COLOR)
    if (temp_c <= 0) return GColorBlue;
    if (temp_c <= 5) return GColorVividCerulean;
    if (temp_c <= 10) return GColorCyan;
    if (temp_c <= 15) return GColorGreen;
    if (temp_c <= 20) return GColorScreaminGreen;
    if (temp_c <= 25) return GColorYellow;
    if (temp_c <= 30) return GColorOrange;
    if (temp_c <= 35) return GColorRed;
    return GColorDarkCandyAppleRed;
#else
    return GColorWhite;
#endif
}

static bool should_show_moon(const GlobalSettings* settings) {
    if (settings->MoonPhase == 2) {
        return false;
    }
    if (settings->MoonPhase == 1) {
        return true;
    }
    return weather_is_night();
}

static void format_weather_info_line(char *buffer, size_t size, int label_index,
                                     const AppState *state, const GlobalSettings *settings) {
    switch (label_index) {
        case 1:
            snprintf(buffer, size, "%s", state->weather.conditions_buffer);
            break;
        case 2:
            weather_format_wind(buffer, size, state->weather.wind_kmh, settings->SpeedUnit);
            break;
        case 3:
            snprintf(buffer, size, "%d %%", state->weather.humidity_percent);
            break;
        case 4:
            weather_format_pressure(buffer, size, state->weather.pressure_hpa, settings->PressureUnit);
            break;
        case 5:
            weather_format_hi_lo(buffer, size, state->weather.temp_high_c, state->weather.temp_low_c, settings->degree_f);
            break;
        default:
            buffer[0] = '\0';
            break;
    }
}

WeatherLayer* weather_layer_create(GRect frame) {
    if (!s_weather_font) {
        s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CLIMACONS_32));
    }
    if (!s_moon_font) {
        s_moon_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MOON_PHASES_SUBSET_24));
    }

    WeatherLayer* wl = malloc(sizeof(WeatherLayer));
    wl->root_layer = layer_create(frame);
    
    // Weather icon / moon area (uses custom font, can't use helper)
    wl->icon_layer = text_layer_create(layout_get_rect(LAYOUT_WEATHER_ICON_DAY));
    text_layer_set_font(wl->icon_layer, s_weather_font);
    text_layer_set_text_alignment(wl->icon_layer, GTextAlignmentCenter);
    text_layer_set_background_color(wl->icon_layer, GColorClear);

    // Temperature (uses bold 28pt, can't use helper)
    wl->temp_layer = text_layer_create(layout_get_rect(LAYOUT_WEATHER_TEMP));
    text_layer_set_font(wl->temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    text_layer_set_text_alignment(wl->temp_layer, GTextAlignmentRight);
    text_layer_set_background_color(wl->temp_layer, GColorClear);

    wl->location_layer = create_text_layer(layout_get_rect(LAYOUT_WEATHER_LOCATION), FONT_KEY_GOTHIC_14_BOLD, GTextAlignmentCenter, GColorWhite);
    wl->last_update_layer = create_text_layer(layout_get_rect(LAYOUT_WEATHER_LAST_UPDATE), FONT_KEY_GOTHIC_14, GTextAlignmentCenter, GColorWhite);
    wl->string_1_layer = create_text_layer(layout_get_rect(LAYOUT_WEATHER_STRING1), FONT_KEY_GOTHIC_14, GTextAlignmentRight, GColorWhite);
    wl->string_2_layer = create_text_layer(layout_get_rect(LAYOUT_WEATHER_STRING2), FONT_KEY_GOTHIC_14, GTextAlignmentRight, GColorWhite);
    wl->string_3_layer = create_text_layer(layout_get_rect(LAYOUT_WEATHER_STRING3), FONT_KEY_GOTHIC_14, GTextAlignmentCenter, GColorWhite);

    layer_add_child(wl->root_layer, text_layer_get_layer(wl->icon_layer));
    layer_add_child(wl->root_layer, text_layer_get_layer(wl->temp_layer));
    layer_add_child(wl->root_layer, text_layer_get_layer(wl->location_layer));
    layer_add_child(wl->root_layer, text_layer_get_layer(wl->last_update_layer));
    layer_add_child(wl->root_layer, text_layer_get_layer(wl->string_1_layer));
    layer_add_child(wl->root_layer, text_layer_get_layer(wl->string_2_layer));
    layer_add_child(wl->root_layer, text_layer_get_layer(wl->string_3_layer));
    
    return wl;
}

void weather_layer_destroy(WeatherLayer* wl) {
    text_layer_destroy(wl->icon_layer);
    text_layer_destroy(wl->temp_layer);
    text_layer_destroy(wl->location_layer);
    text_layer_destroy(wl->last_update_layer);
    text_layer_destroy(wl->string_1_layer);
    text_layer_destroy(wl->string_2_layer);
    text_layer_destroy(wl->string_3_layer);
    layer_destroy(wl->root_layer);
    free(wl);
    
    if (s_weather_font) {
        fonts_unload_custom_font(s_weather_font);
        s_weather_font = NULL;
    }
    if (s_moon_font) {
        fonts_unload_custom_font(s_moon_font);
        s_moon_font = NULL;
    }
}

Layer* weather_layer_get_layer(WeatherLayer* wl) {
    return wl->root_layer;
}

void weather_layer_update_data(WeatherLayer* wl) {
    AppState* state = state_get_ptr();
    GlobalSettings* settings = settings_get_instance();
    
    // Temperature - mask sentinel values
    if (state->weather.temp_c > -100 && state->weather.temp_c < 100) {
        format_temperature(wl->temp_buffer, sizeof(wl->temp_buffer), state->weather.temp_c, !settings->degree_f);
    } else {
        snprintf(wl->temp_buffer, sizeof(wl->temp_buffer), "--");
    }
    text_layer_set_text(wl->temp_layer, wl->temp_buffer);
    
    // Weather icon / moon phase
    bool show_moon = should_show_moon(settings);
    if (show_moon) {
        text_layer_set_font(wl->icon_layer, s_moon_font);
        layer_set_frame(text_layer_get_layer(wl->icon_layer), layout_get_rect(LAYOUT_WEATHER_ICON_MOON));
        wl->moon_buffer[0] = ' ';
        wl->moon_buffer[1] = '\0';
        double latitude = (double)state->weather.location_latitude / 1000000.0;
        int moonphase = calc_moonphase_number(latitude);
        wl->moon_buffer[0] = (char)moonphase_char_number(moonphase);
        text_layer_set_text(wl->icon_layer, wl->moon_buffer);
    } else {
        text_layer_set_font(wl->icon_layer, s_weather_font);
        layer_set_frame(text_layer_get_layer(wl->icon_layer), layout_get_rect(LAYOUT_WEATHER_ICON_DAY));
        wl->icon_buffer[0] = ' ';
        wl->icon_buffer[1] = '\0';
        if (state->weather.icon_id >= 33 && state->weather.icon_id <= 106) {
            wl->icon_buffer[0] = (char)state->weather.icon_id;
        } else {
            wl->icon_buffer[0] = 'I'; // Default sun icon
        }
        text_layer_set_text(wl->icon_layer, wl->icon_buffer);
    }
    
    // Location
    if (state->weather.location_name[0] != '\0') {
        text_layer_set_text(wl->location_layer, state->weather.location_name);
    } else {
        text_layer_set_text(wl->location_layer, "");
    }
    
    // Last Update Time - based on ShowTimeSinceStationData setting
    // 0 = time since phone update, 1 = time since weather data (API)
    time_t now = time(NULL);
    time_t ref_time = state->weather.last_update_time; // Default: phone update time
    if (settings->ShowTimeSinceStationData == 1 && state->weather.station_data_time > 0) {
        ref_time = state->weather.station_data_time;
    }
    time_t elapsed = now - ref_time;
    if (elapsed >= 0 && elapsed < TEN_YEARS_SECONDS) {
        format_time_elapsed(wl->update_buffer, sizeof(wl->update_buffer), elapsed);
    } else {
        snprintf(wl->update_buffer, sizeof(wl->update_buffer), "--:--");
    }
    text_layer_set_text(wl->last_update_layer, wl->update_buffer);
    
    // Weather info lines are generated on-watch from raw weather metrics.
    format_weather_info_line(wl->info_line_1, sizeof(wl->info_line_1), settings->WeatherLabelIndex1, state, settings);
    format_weather_info_line(wl->info_line_2, sizeof(wl->info_line_2), settings->WeatherLabelIndex2, state, settings);
    format_weather_info_line(wl->info_line_3, sizeof(wl->info_line_3), settings->WeatherLabelIndex3, state, settings);
    format_weather_info_line(wl->info_line_4, sizeof(wl->info_line_4), settings->WeatherLabelIndex4, state, settings);

    text_layer_set_text(wl->string_1_layer, wl->info_line_1);
    text_layer_set_text(wl->string_2_layer, wl->info_line_2);

    // Left row 1/2 are rendered together on one line to match the classic reference layout.
    if (wl->info_line_3[0] != '\0' && wl->info_line_4[0] != '\0') {
        snprintf(wl->left_info_buffer, sizeof(wl->left_info_buffer), "%s / %s",
                 wl->info_line_3, wl->info_line_4);
    } else if (wl->info_line_3[0] != '\0') {
        snprintf(wl->left_info_buffer, sizeof(wl->left_info_buffer), "%s", wl->info_line_3);
    } else if (wl->info_line_4[0] != '\0') {
        snprintf(wl->left_info_buffer, sizeof(wl->left_info_buffer), "%s", wl->info_line_4);
    } else {
        wl->left_info_buffer[0] = '\0';
    }
    text_layer_set_text(wl->string_3_layer, wl->left_info_buffer);
}

void weather_layer_update_colors(WeatherLayer* wl) {
    GlobalSettings* settings = settings_get_instance();
    const ColorScheme* scheme = colors_get_scheme(settings->ColorProfile);
    AppState* state = state_get_ptr();
    bool show_moon = should_show_moon(settings);
    
    GColor temp_color = scheme->weather;
    if (settings->ColoredTemperature &&
        state->weather.temp_c > -100 && state->weather.temp_c < 100) {
        temp_color = temperature_color(state->weather.temp_c);
    }
    text_layer_set_text_color(wl->temp_layer, temp_color);
    
    // Icon color based on weather condition or moon
    if (show_moon) {
        text_layer_set_text_color(wl->icon_layer, scheme->moon);
    } else {
        GColor icon_fg, icon_bg;
        colors_get_weather_icon_colors(settings->ColorProfile, state->weather.icon_id, weather_is_night(), &icon_fg, &icon_bg);
        text_layer_set_text_color(wl->icon_layer, icon_fg);
    }

    // Location/last-update with warning backgrounds
    // Fetch error indicator: red background when API fetch failed
    if (state->weather.fetch_error && settings->warning_color_location == 0) {
        text_layer_set_text_color(wl->location_layer, GColorWhite);
        text_layer_set_text_color(wl->last_update_layer, GColorWhite);
        text_layer_set_background_color(wl->location_layer, GColorRed);
        text_layer_set_background_color(wl->last_update_layer, GColorRed);
    // Stale weather indicator: yellow background when data is over 2 hours old
    } else if (state->weather.is_stale && settings->warning_color_location == 0) {
        text_layer_set_text_color(wl->location_layer, GColorBlack);
        text_layer_set_text_color(wl->last_update_layer, GColorBlack);
        text_layer_set_background_color(wl->location_layer, GColorYellow);
        text_layer_set_background_color(wl->last_update_layer, GColorYellow);
    } else if (settings->warning_color_location == 1) {
        text_layer_set_text_color(wl->location_layer, GColorWhite);
        text_layer_set_text_color(wl->last_update_layer, GColorWhite);
        text_layer_set_background_color(wl->location_layer, GColorRed);
        text_layer_set_background_color(wl->last_update_layer, GColorRed);
    } else if (settings->warning_color_location == 2) {
        text_layer_set_text_color(wl->location_layer, GColorWhite);
        text_layer_set_text_color(wl->last_update_layer, GColorWhite);
        text_layer_set_background_color(wl->location_layer, GColorBlack);
        text_layer_set_background_color(wl->last_update_layer, GColorBlack);
    } else {
        text_layer_set_text_color(wl->location_layer, scheme->location);
        text_layer_set_text_color(wl->last_update_layer, scheme->last_update);
        text_layer_set_background_color(wl->location_layer, scheme->location_bg);
        text_layer_set_background_color(wl->last_update_layer, scheme->last_update_bg);
    }

    text_layer_set_text_color(wl->string_1_layer, scheme->weather);
    text_layer_set_text_color(wl->string_2_layer, scheme->weather);
    text_layer_set_text_color(wl->string_3_layer, scheme->weather);
}
