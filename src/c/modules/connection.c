#include "connection.h"
#include "../state/events.h"
#include "../state/settings.h"

static void bluetooth_callback(bool connected) {
    AppState* state = state_get_ptr();
    GlobalSettings* settings = settings_get_instance();
    
    if (state->connection.bluetooth_connected != connected) {
        state->connection.bluetooth_connected = connected;
        
        if (!connected) {
             if (settings->vibe_on_disconnect && !state->connection.vibed_on_disconnect) {
                 vibes_long_pulse();
                 state->connection.vibed_on_disconnect = true;
             }
        } else {
            state->connection.vibed_on_disconnect = false; // Reset
        }
        
        events_set_flag(EVENT_CONNECTION);
    }
}

void connection_module_init() {
    connection_service_subscribe((ConnectionHandlers) {
        .pebble_app_connection_handler = bluetooth_callback
    });
    
    // Initial check
    bluetooth_callback(connection_service_peek_pebble_app_connection());
}

void connection_module_deinit() {
    connection_service_unsubscribe();
}
