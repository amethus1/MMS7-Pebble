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
    
    printf("ALL TESTS PASSED\n");
    return 0;
}
