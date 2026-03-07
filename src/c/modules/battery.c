#include "battery.h"
#include "../keys.h"
#include "../state/events.h"

static bool s_bootstrap_from_persist = true;
static int s_battery_percent_on_exit = 0;
static int s_last_battery_percent = 0;

static int get_charge_state_code(BatteryChargeState charge_state) {
    if (!charge_state.is_plugged) {
        return 0;  // Discharging
    }
    return charge_state.is_charging ? 1 : 2;  // Charging : Full+plugged
}

static void load_persisted_battery_state(BatteryState* battery) {
    if (persist_exists(KEY_BTY_LAST_STATE)) {
        int persisted_state = persist_read_int(KEY_BTY_LAST_STATE);
        if (persisted_state < 0 || persisted_state > 2) {
            persisted_state = 0;
        }
        battery->charge_state = persisted_state;
    }

    if (persist_exists(KEY_BTY_LAST_CHARGED)) {
        battery->last_state_change_time = (time_t)persist_read_int(KEY_BTY_LAST_CHARGED);
    }

    if (persist_exists(KEY_BTY_LAST_PERIOD)) {
        battery->last_period_duration = (time_t)persist_read_int(KEY_BTY_LAST_PERIOD);
    }

    if (persist_exists(KEY_BTY_LAST_PERCENT)) {
        s_last_battery_percent = persist_read_int(KEY_BTY_LAST_PERCENT);
        if (s_last_battery_percent < 0) {
            s_last_battery_percent = 0;
        } else if (s_last_battery_percent > 100) {
            s_last_battery_percent = 100;
        }
    }

    if (persist_exists(KEY_BTY_PERCENT_EXIT)) {
        s_battery_percent_on_exit = persist_read_int(KEY_BTY_PERCENT_EXIT);
        if (s_battery_percent_on_exit < 0) {
            s_battery_percent_on_exit = 0;
        } else if (s_battery_percent_on_exit > 100) {
            s_battery_percent_on_exit = 100;
        }
    }
}

void battery_module_init() {
    AppState* app_state = state_get_ptr();
    load_persisted_battery_state(&app_state->battery);

    BatteryChargeState state = battery_state_service_peek();
    battery_handle_event(state);
    
    // Subscribe
    battery_state_service_subscribe(battery_handle_event);
}

void battery_module_deinit() {
    AppState* app_state = state_get_ptr();
    BatteryChargeState current_state = battery_state_service_peek();
    BatteryState* battery = &app_state->battery;

    persist_write_int(KEY_BTY_LAST_PERCENT, s_last_battery_percent);
    persist_write_int(KEY_BTY_PERCENT_EXIT, current_state.charge_percent);
    persist_write_int(KEY_BTY_LAST_CHARGED, (int)battery->last_state_change_time);
    persist_write_int(KEY_BTY_LAST_STATE, battery->charge_state);
    persist_write_int(KEY_BTY_LAST_PERIOD, (int)battery->last_period_duration);

    battery_state_service_unsubscribe();
}

void battery_handle_event(BatteryChargeState charge_state) {
    AppState* app_state = state_get_ptr();
    BatteryState* battery = &app_state->battery;
    bool changed = false;
    bool transition_updated = false;
    time_t now = time(NULL);

    if (battery->last_state_change_time <= 0) {
        battery->last_state_change_time = now;
    }

    int old_charge_state = battery->charge_state;
    int new_charge_state = get_charge_state_code(charge_state);
    time_t tdiff = now - battery->last_state_change_time;
    if (tdiff < 0) {
        tdiff = 0;
    }

    // Keep old transition semantics from MMS OLD.
    if ((old_charge_state == 0) && (new_charge_state == 1)) {
        battery->last_period_duration = tdiff;
        battery->last_state_change_time = now;
        transition_updated = true;
    } else if ((old_charge_state == 0) && (new_charge_state == 0)) {
        if (s_bootstrap_from_persist &&
            (s_battery_percent_on_exit + 10 < charge_state.charge_percent)) {
            battery->last_period_duration = tdiff;
            battery->last_state_change_time = now;
            transition_updated = true;
        }
    } else if ((old_charge_state == 1) && (new_charge_state == 2)) {
        if (tdiff > 10 * 60) {
            battery->last_period_duration = tdiff;
        } else {
            battery->last_period_duration += tdiff;
        }
        battery->last_state_change_time = now;
        transition_updated = true;
    } else if ((old_charge_state == 1) && (new_charge_state == 0)) {
        battery->last_period_duration = tdiff;
        battery->last_state_change_time = now;
        transition_updated = true;
    } else if ((old_charge_state == 2) && (new_charge_state == 0)) {
        battery->last_state_change_time = now;
        transition_updated = true;
    }

    // Handle transitions that were not explicitly present in the old code.
    if (!transition_updated && old_charge_state != new_charge_state) {
        battery->last_period_duration = tdiff;
        battery->last_state_change_time = now;
        transition_updated = true;
    }

    if (battery->charge_percent != charge_state.charge_percent) {
        battery->charge_percent = charge_state.charge_percent;
        changed = true;
    }

    if (battery->is_charging != charge_state.is_charging) {
        battery->is_charging = charge_state.is_charging;
        changed = true;
    }

    if (battery->is_plugged != charge_state.is_plugged) {
        battery->is_plugged = charge_state.is_plugged;
        changed = true;
    }

    if (battery->charge_state != new_charge_state) {
        battery->charge_state = new_charge_state;
        changed = true;
    }

    battery->estimated_hours_remaining = -1;  // no longer used by UI
    s_last_battery_percent = charge_state.charge_percent;

    if (transition_updated) {
        changed = true;
    }

    s_bootstrap_from_persist = false;

    if (changed) {
        events_set_flag(EVENT_BATTERY_STATE);
    }
}
