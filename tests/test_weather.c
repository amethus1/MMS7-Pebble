#include "mock_pebble.h"
#include "../src/c/modules/weather.h"
#include "../src/c/state/state.h"
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
    
    printf("ALL TESTS PASSED\n");
    return 0;
}
