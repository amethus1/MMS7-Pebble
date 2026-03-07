#include "weather.h"
#include "../keys.h"
#include "../config.h"
#include "../state/events.h"
#include "comm.h"

#define _(str) str

// Internal helper for degree replacement (__ -> °)
static void replace_degree(char *s, int size_s){
    for (int i=1; i<size_s; i++){
        if ((s[i-1] == '_') && (s[i] == '_')){
            s[i-1] = (char)194;
            s[i]   = (char)176;
        }
    }
}

void weather_init() {
    AppState* state = state_get_ptr();
    
    // Set defaults with clear "no data" indicators
    snprintf(state->weather.location_name, sizeof(state->weather.location_name), "...");
    state->weather.temp_c = -999;
    state->weather.temp_f = -999;
    state->weather.temp_high_c = -999;
    state->weather.temp_low_c = -999;
    state->weather.humidity_percent = 0;
    state->weather.pressure_hpa = 0;
    state->weather.wind_kmh = 0;
    state->weather.icon_id = (int)'I';
    state->weather.last_update_time = 0;
    state->weather.station_data_time = 0;
    state->weather.is_stale = true;
    state->weather.fetch_error = false;
    state->weather.timezone_name[0] = '\0';
    state->weather.timezone_utc_offset = 0;
    state->weather.conditions_buffer[0] = '\0';
    state->weather.location_latitude = (int)(LATITUDE * 1000000);
    state->weather.location_longitude = (int)(LONGITUDE * 1000000);
    state->weather.sunrise = 0;
    state->weather.sunset = 0;
    
    // Load persisted data (if any)
    if (persist_exists(KEY_LOCATION_NAME)) {
        persist_read_string(KEY_LOCATION_NAME, state->weather.location_name, sizeof(state->weather.location_name));
    }
    if (persist_exists(KEY_WEATHER_TEMP)) {
        state->weather.temp_c = persist_read_int(KEY_WEATHER_TEMP);
    }
    if (persist_exists(KEY_WEATHER_HIGH)) {
        state->weather.temp_high_c = persist_read_int(KEY_WEATHER_HIGH);
    }
    if (persist_exists(KEY_WEATHER_LOW)) {
        state->weather.temp_low_c = persist_read_int(KEY_WEATHER_LOW);
    }
    if (persist_exists(KEY_WEATHER_ICON)) {
        state->weather.icon_id = persist_read_int(KEY_WEATHER_ICON);
    }
    if (persist_exists(KEY_WEATHER_HUMIDITY)) {
        state->weather.humidity_percent = persist_read_int(KEY_WEATHER_HUMIDITY);
    }
    if (persist_exists(KEY_WEATHER_PRESSURE)) {
        state->weather.pressure_hpa = persist_read_int(KEY_WEATHER_PRESSURE);
    }
    if (persist_exists(KEY_WEATHER_WIND_KMH)) {
        state->weather.wind_kmh = persist_read_int(KEY_WEATHER_WIND_KMH);
    }
    if (persist_exists(KEY_TIME_LAST_UPDATE)) {
        state->weather.last_update_time = persist_read_int(KEY_TIME_LAST_UPDATE);
    }
    if (persist_exists(KEY_WEATHER_DATA_TIME)) {
        state->weather.station_data_time = persist_read_int(KEY_WEATHER_DATA_TIME);
    }
    if (persist_exists(KEY_SUN_RISE_UNIX)) {
        state->weather.sunrise = persist_read_int(KEY_SUN_RISE_UNIX);
    }
    if (persist_exists(KEY_SUN_SET_UNIX)) {
        state->weather.sunset = persist_read_int(KEY_SUN_SET_UNIX);
    }
    if (persist_exists(KEY_TIME_ZONE_NAME)) {
        persist_read_string(KEY_TIME_ZONE_NAME, state->weather.timezone_name, sizeof(state->weather.timezone_name));
    }
    if (persist_exists(KEY_TIME_UTC_OFFSET)) {
        state->weather.timezone_utc_offset = persist_read_int(KEY_TIME_UTC_OFFSET);
    }
    if (persist_exists(KEY_WEATHER_CONDITIONS)) {
        persist_read_string(KEY_WEATHER_CONDITIONS, state->weather.conditions_buffer, sizeof(state->weather.conditions_buffer));
    }
    if (persist_exists(KEY_LOCATION_LAT)) {
        state->weather.location_latitude = persist_read_int(KEY_LOCATION_LAT);
    }
    if (persist_exists(KEY_LOCATION_LON)) {
        state->weather.location_longitude = persist_read_int(KEY_LOCATION_LON);
    }

    // Check staleness
    weather_check_staleness();
}

void weather_deinit() {
    AppState* state = state_get_ptr();
    
    // Persist all weather data for next startup
    persist_write_string(KEY_LOCATION_NAME, state->weather.location_name);
    persist_write_int(KEY_WEATHER_TEMP, state->weather.temp_c);
    persist_write_int(KEY_WEATHER_HIGH, state->weather.temp_high_c);
    persist_write_int(KEY_WEATHER_LOW, state->weather.temp_low_c);
    persist_write_int(KEY_WEATHER_ICON, state->weather.icon_id);
    persist_write_int(KEY_WEATHER_HUMIDITY, state->weather.humidity_percent);
    persist_write_int(KEY_WEATHER_PRESSURE, state->weather.pressure_hpa);
    persist_write_int(KEY_WEATHER_WIND_KMH, state->weather.wind_kmh);
    persist_write_int(KEY_TIME_LAST_UPDATE, (int)state->weather.last_update_time);
    persist_write_int(KEY_WEATHER_DATA_TIME, (int)state->weather.station_data_time);
    persist_write_int(KEY_SUN_RISE_UNIX, (int)state->weather.sunrise);
    persist_write_int(KEY_SUN_SET_UNIX, (int)state->weather.sunset);
    persist_write_string(KEY_TIME_ZONE_NAME, state->weather.timezone_name);
    persist_write_int(KEY_TIME_UTC_OFFSET, state->weather.timezone_utc_offset);
    persist_write_string(KEY_WEATHER_CONDITIONS, state->weather.conditions_buffer);
    persist_write_int(KEY_LOCATION_LAT, state->weather.location_latitude);
    persist_write_int(KEY_LOCATION_LON, state->weather.location_longitude);
}

bool weather_handle_app_message(DictionaryIterator *iterator) {
    AppState* state = state_get_ptr();
    bool changed = false;
    bool weather_payload_changed = false;
    Tuple *t = dict_read_first(iterator);

    while(t != NULL) {
        switch(t->key) {
            // Timezone can arrive independently of weather
            case KEY_TIME_ZONE_NAME:
                if (strlen(t->value->cstring) > 0) {
                    snprintf(state->weather.timezone_name, sizeof(state->weather.timezone_name), "%s", t->value->cstring);
                    changed = true;
                }
                break;
            case KEY_TIME_UTC_OFFSET:
                state->weather.timezone_utc_offset = t->value->int32;
                changed = true;
                break;
                
            // Weather data
            case KEY_LOCATION_NAME:
                snprintf(state->weather.location_name, sizeof(state->weather.location_name), "%s", t->value->cstring);
                state->weather.last_update_time = time(NULL);
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_LOCATION_LAT:
                state->weather.location_latitude = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_LOCATION_LON:
                state->weather.location_longitude = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_TEMP:
                state->weather.temp_c = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_HIGH:
                state->weather.temp_high_c = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_LOW:
                state->weather.temp_low_c = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_ICON:
                state->weather.icon_id = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_HUMIDITY:
                state->weather.humidity_percent = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_PRESSURE:
                state->weather.pressure_hpa = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_WIND_KMH:
                state->weather.wind_kmh = t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_CONDITIONS:
                snprintf(state->weather.conditions_buffer, sizeof(state->weather.conditions_buffer), "%s", t->value->cstring);
                replace_degree(state->weather.conditions_buffer, sizeof(state->weather.conditions_buffer));
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_SUN_RISE_UNIX:
                state->weather.sunrise = (time_t)t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_SUN_SET_UNIX:
                state->weather.sunset = (time_t)t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_DATA_TIME:
                state->weather.station_data_time = (time_t)t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_TIME_LAST_UPDATE:
                state->weather.last_update_time = (time_t)t->value->int32;
                changed = true;
                weather_payload_changed = true;
                break;
            case KEY_WEATHER_FETCH_ERROR:
                if (t->value->int32 != 0) {
                    state->weather.fetch_error = true;
                    changed = true;
                }
                break;
        }
        t = dict_read_next(iterator);
    }
    
    // If we received weather data, clear staleness and error flags
    if (weather_payload_changed) {
        state->weather.is_stale = false;
        state->weather.fetch_error = false;
    }
    
    return changed;
}

bool weather_check_staleness() {
    AppState* state = state_get_ptr();
    bool was_stale = state->weather.is_stale;
    time_t now = time(NULL);
    
    // Consider data stale after 2 hours (7200 seconds)
    if (state->weather.last_update_time == 0) {
        state->weather.is_stale = true;
    } else if (now - state->weather.last_update_time > 7200) {
        state->weather.is_stale = true;
    } else {
        state->weather.is_stale = false;
    }
    
    // Notify if staleness changed
    if (was_stale != state->weather.is_stale) {
        events_set_flag(EVENT_WEATHER_DATA);
        return true;
    }
    return false;
}

void weather_request_update() {
    comm_request_weather();
}

bool weather_is_night() {
    AppState* state = state_get_ptr();
    
    // If no sunrise/sunset data, default to not night
    if (state->weather.sunrise == 0 || state->weather.sunset == 0) {
        return false;
    }
    
    time_t now = time(NULL);
    
    // Night = before sunrise OR after sunset
    return (now < state->weather.sunrise || now > state->weather.sunset);
}

bool weather_has_data() {
    AppState* state = state_get_ptr();
    return state->weather.temp_c > -100 && state->weather.temp_c < 100;
}

const char* weather_get_timezone() {
    AppState* state = state_get_ptr();
    return state->weather.timezone_name;
}
