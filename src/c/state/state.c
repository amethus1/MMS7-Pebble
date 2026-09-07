#include "state.h"

static AppState s_app_state;

void state_init() {
    // Initialize with defaults
    s_app_state.connection.bluetooth_connected = false; // Assume disconnected until event
    s_app_state.weather.is_stale = true;
    s_app_state.weather.fetch_error = false;
    s_app_state.weather.fetch_error_reason = 0;
    s_app_state.weather.location_unconfirmed = false;
    s_app_state.weather.humidity_percent = 0;
    s_app_state.weather.pressure_hpa = 0;
    s_app_state.weather.wind_kmh = 0;
    s_app_state.weather.conditions_buffer[0] = '\0';
    s_app_state.battery.charge_percent = 0;
    s_app_state.battery.is_charging = false;
    s_app_state.battery.is_plugged = false;
    s_app_state.battery.estimated_hours_remaining = -1;
    s_app_state.battery.charge_state = 0;
    s_app_state.battery.last_state_change_time = 0;
    s_app_state.battery.last_period_duration = 0;
    s_app_state.health.steps = -1;
    s_app_state.health.sleep_seconds = -1;
    s_app_state.health.is_sleeping = false;
    s_app_state.health.steps_trend = 0;
    s_app_state.health.sleep_trend = 0;
    s_app_state.health.trend_display = 0;
}

AppState* state_get_ptr() {
    return &s_app_state;
}
