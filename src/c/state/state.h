#pragma once
#include <pebble.h>
#include "../config.h"

// Sub-state structs
typedef struct {
    int temp_c;
    int temp_f;
    int temp_high_c;
    int temp_low_c;
    int humidity_percent;
    int pressure_hpa;
    int wind_kmh;
    char conditions_buffer[BUFFER_SIZE_MEDIUM]; // Description
    int icon_id;
    bool is_night;
    bool is_stale;
    bool fetch_error;        // True if last weather fetch failed
    time_t last_update_time;     // When phone sent data to watch
    time_t station_data_time;    // When weather API recorded the data
    char location_name[BUFFER_SIZE_MEDIUM];
    int location_latitude;      // Stored as microdegrees
    int location_longitude;     // Stored as microdegrees
    char timezone_name[BUFFER_SIZE_SMALL];     // Timezone abbreviation (e.g., "EST")
    int timezone_utc_offset;    // Offset from UTC in seconds for the weather location
    time_t sunrise;
    time_t sunset;
} WeatherState;

typedef struct {
    uint8_t charge_percent;
    bool is_charging;
    bool is_plugged;
    int estimated_hours_remaining; // Reserved for compatibility
    int charge_state;              // 0=discharging, 1=charging, 2=plugged+full
    time_t last_state_change_time; // Start time of current charge/discharge period
    time_t last_period_duration;   // Previous completed period duration
} BatteryState;

typedef struct {
    int steps;
    time_t sleep_seconds;
    bool is_sleeping;
    int steps_trend;   // -1 below avg, 0 equal/unknown, 1 above avg
    int sleep_trend;   // -1 below avg, 0 equal/unknown, 1 above avg
    int trend_display;
} HealthState;

typedef struct {
    bool bluetooth_connected;
    bool vibed_on_disconnect; 
} ConnectionState;

// Master State
typedef struct {
    WeatherState weather;
    BatteryState battery;
    HealthState health;
    ConnectionState connection;
} AppState;

// Accessors
AppState* state_get_ptr();
void state_init();
