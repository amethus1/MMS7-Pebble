#include "health.h"
#include "../state/events.h"

#if defined(PBL_HEALTH)
static void health_handler(HealthEventType event, void *context) {
    if (event == HealthEventMovementUpdate || event == HealthEventSignificantUpdate ||
        event == HealthEventSleepUpdate) {
        health_update();
    }
}
#endif

void health_module_init() {
    #if defined(PBL_HEALTH)
    if(health_service_events_subscribe(health_handler, NULL)) {
        // Force initial update
        health_update();
    }
    #endif
}

void health_module_deinit() {
    #if defined(PBL_HEALTH)
    health_service_events_unsubscribe();
    #endif
}

void health_update() {
    #if defined(PBL_HEALTH)
    AppState* state = state_get_ptr();
    HealthMetric metric = HealthMetricStepCount;
    time_t start = time_start_of_today();
    time_t end = time(NULL);
    HealthServiceTimeScope scope = HealthServiceTimeScopeDailyWeekdayOrWeekend;

    HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
    if(mask & HealthServiceAccessibilityMaskAvailable) {
        state->health.steps = (int)health_service_sum_today(metric);
        state->health.steps_trend = 0;
        HealthServiceAccessibilityMask avg_mask = health_service_metric_averaged_accessible(metric, start, end, scope);
        if (avg_mask & HealthServiceAccessibilityMaskAvailable) {
            HealthValue avg = health_service_sum_averaged(metric, start, end, scope);
            if (state->health.steps > (int)avg) state->health.steps_trend = 1;
            else if (state->health.steps < (int)avg) state->health.steps_trend = -1;
        }
        events_set_flag(EVENT_HEALTH_UPDATE);
    } else {
        state->health.steps = -1;   // Distinguish "no data" from a genuine zero
        state->health.steps_trend = 0;
        events_set_flag(EVENT_HEALTH_UPDATE);
    }
    
    // Sleep: "last night" is summed explicitly from 18:00 yesterday, so it
    // does not depend on which calendar day the firmware files a session under.
    HealthMetric sleep_metric = HealthMetricSleepSeconds;
    time_t sleep_start = start - 6 * 3600;
    HealthServiceAccessibilityMask sleep_mask = health_service_metric_accessible(sleep_metric, sleep_start, end);
    if(sleep_mask & HealthServiceAccessibilityMaskAvailable) {
        state->health.sleep_seconds = health_service_sum(sleep_metric, sleep_start, end);
        state->health.sleep_trend = 0;
        HealthServiceAccessibilityMask sleep_avg_mask = health_service_metric_averaged_accessible(sleep_metric, start, end, scope);
        if (sleep_avg_mask & HealthServiceAccessibilityMaskAvailable) {
            HealthValue avg = health_service_sum_averaged(sleep_metric, start, end, scope);
            if (state->health.sleep_seconds > (time_t)avg) state->health.sleep_trend = 1;
            else if (state->health.sleep_seconds < (time_t)avg) state->health.sleep_trend = -1;
        }
    } else {
        state->health.sleep_seconds = -1;
        state->health.sleep_trend = 0;
    }
    #endif
}
