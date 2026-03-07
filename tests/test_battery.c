#include "mock_pebble.h"
#include "../src/c/modules/battery.h"
#include "../src/c/state/state.h"

int main() {
    printf("Running Battery Tests...\n");
    
    // Initialize state and battery module
    state_init();
    battery_module_init();
    AppState* app_state = state_get_ptr();
    BatteryState* b = &app_state->battery;
    
    // Case 1: Discharging
    BatteryChargeState state = {
        .charge_percent = 80,
        .is_charging = false,
        .is_plugged = false
    };
    battery_handle_event(state);
    
    assert_false(b->is_charging);
    assert_true(b->charge_percent == 80);
    
    // Case 2: Plugged in (Charging)
    state.is_plugged = true;
    state.is_charging = true;
    battery_handle_event(state);
    
    assert_true(b->is_charging);
    assert_true(b->is_plugged);
    
    // Case 3: Fully Charged (plugged but not charging)
    state.is_charging = false;
    battery_handle_event(state);
    
    assert_true(b->is_plugged);
    assert_false(b->is_charging);
    
    // Case 4: Low battery threshold
    state.charge_percent = 20;
    state.is_plugged = false;
    state.is_charging = false;
    battery_handle_event(state);
    
    assert_true(b->charge_percent == 20);
    assert_false(b->is_charging);
    
    printf("ALL TESTS PASSED\n");
    return 0;
}
