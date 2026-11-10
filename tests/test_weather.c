#include "mock_pebble.h"
#include "../src/c/modules/weather.h"
#include "../src/c/state/state.h"
#include "../src/c/modules/weather_format.h"
#include "../src/c/keys.h"
// Include source directly to access static state if needed, or link against it.
// For simplicity in this environment, we'll assume we link against weather.c 
// but we need to supply the missing Pebble SDK symbols via mock_pebble.c

int main() {
    printf("Running Weather Tests...\n");
    
    // Test 1: Initialize and get state via state_get_ptr()
    state_init();
    weather_init();
    AppState* app_state = state_get_ptr();
    WeatherState* w = &app_state->weather;
    
    // Simulate freshness - 10 seconds ago
    w->last_update_time = time(NULL) - 10;
    
    // Check - should NOT be stale
    bool changed = weather_check_staleness();
    assert_false(w->is_stale);
    
    // Test 2: Stale data - 2 hours + 1 second ago
    w->last_update_time = time(NULL) - 7201;
    
    changed = weather_check_staleness();
    
    // Should BE stale
    assert_true(w->is_stale);
    assert_true(changed); // State changed from false to true
    
    // Test 3: Recovery - fresh data again
    w->last_update_time = time(NULL);
    changed = weather_check_staleness();
    
    assert_false(w->is_stale);
    assert_true(changed); // State changed from true to false
    
    // Test 4: Fetch error flag
    assert_false(w->fetch_error);
    w->fetch_error = true;
    assert_true(w->fetch_error);
    w->fetch_error = false;
    assert_false(w->fetch_error);

    // Test 5: Stale conditions must not be presented as current.
    w->temp_c = 20;
    w->is_stale = false;
    w->fetch_error = false;
    assert_true(weather_has_current_data());

    w->is_stale = true;
    assert_false(weather_has_current_data());

    // A failed send says nothing about the age of the cached reading, so it
    // must not blank the icon while every other cached field stays visible.
    w->is_stale = false;
    w->fetch_error = true;
    assert_true(weather_has_current_data());
    w->fetch_error = false;

    // Test 6: Night follows the sun, including the day after the last fetch.
    time_t now = time(NULL);
    w->sunrise = now - 3 * 3600;   // Sun rose 3 hours ago
    w->sunset = now + 5 * 3600;    // and sets in 5
    assert_false(weather_is_night());
    w->sunrise = now + 2 * 3600;   // Before sunrise
    w->sunset = now + 12 * 3600;
    assert_true(weather_is_night());
    w->sunrise = now - 12 * 3600;  // After sunset, same day
    w->sunset = now - 1 * 3600;
    assert_true(weather_is_night());
    // Phone out of reach overnight: yesterday's times, and it is mid-morning.
    // This used to read as "after sunset" until the next fetch.
    w->sunrise = now - 27 * 3600;
    w->sunset = now - 19 * 3600;
    assert_false(weather_is_night());
    // Three days without a fetch, in the small hours
    w->sunrise = now - (3 * 24 - 2) * 3600;   // Sunrise is 2 hours away by the clock
    w->sunset = now - (3 * 24 - 12) * 3600;
    assert_true(weather_is_night());
    // No sun data at all
    w->sunrise = 0;
    w->sunset = 0;
    assert_false(weather_is_night());

    // Test 7: Launch only fetches when the cached weather is due.
    w->last_update_time = 0;
    assert_true(weather_needs_refresh(20));        // Never fetched
    w->last_update_time = time(NULL) - 5 * 60;
    assert_false(weather_needs_refresh(20));       // 5 minutes old, 20 minute interval
    w->last_update_time = time(NULL) - 21 * 60;
    assert_true(weather_needs_refresh(20));        // Overdue
    w->last_update_time = time(NULL) - 29 * 60;
    assert_false(weather_needs_refresh(0));        // No interval set: the 30 minute default
    w->last_update_time = time(NULL) + 3600;
    assert_true(weather_needs_refresh(20));        // Clock moved back

    // Test 8: A Celsius reading without its Fahrenheit twin must not keep the
    // Fahrenheit value of an earlier reading.
    TupleValue v[6];
    Tuple full[6] = {
        { .key = KEY_WEATHER_TEMP, .type = TUPLE_INT, .length = 4, .value = &v[0] },
        { .key = KEY_WEATHER_HIGH, .type = TUPLE_INT, .length = 4, .value = &v[1] },
        { .key = KEY_WEATHER_LOW, .type = TUPLE_INT, .length = 4, .value = &v[2] },
        { .key = KEY_WEATHER_TEMP_F, .type = TUPLE_INT, .length = 4, .value = &v[3] },
        { .key = KEY_WEATHER_HIGH_F, .type = TUPLE_INT, .length = 4, .value = &v[4] },
        { .key = KEY_WEATHER_LOW_F, .type = TUPLE_INT, .length = 4, .value = &v[5] },
    };
    v[0].int32 = 15; v[1].int32 = 20; v[2].int32 = 7;
    v[3].int32 = 60; v[4].int32 = 69; v[5].int32 = 45;
    mock_dict_set(full, 6);
    assert_true(weather_handle_app_message(NULL));
    assert_true(w->temp_c == 15 && w->temp_f == 60);
    assert_true(w->temp_high_f == 69 && w->temp_low_f == 45);
    v[0].int32 = 22; v[1].int32 = 25; v[2].int32 = 11;
    mock_dict_set(full, 3);   // Celsius only, as an older phone app would send
    assert_true(weather_handle_app_message(NULL));
    assert_true(w->temp_c == 22);
    assert_true(w->temp_f == WEATHER_TEMP_UNKNOWN);
    assert_true(w->temp_high_f == WEATHER_TEMP_UNKNOWN && w->temp_low_f == WEATHER_TEMP_UNKNOWN);

    // Fahrenheit survives a restart
    v[0].int32 = 15; v[3].int32 = 60;
    mock_dict_set(full, 6);
    weather_handle_app_message(NULL);
    weather_deinit();
    w->temp_f = WEATHER_TEMP_UNKNOWN;
    weather_init();
    assert_true(w->temp_f == 60);

    printf("ALL TESTS PASSED\n");
    return 0;
}
