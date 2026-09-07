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

#if defined(PBL_HEALTH)
struct SleepSum {
    time_t window_start;
    time_t window_end;
    time_t seconds;
    int sessions;
};

// Adds the overlap of one sleep session with the window. Only
// HealthActivitySleep is iterated; restful sleep is a subset of it and would
// double count.
static bool sleep_session_cb(HealthActivity activity, time_t time_start, time_t time_end, void *context) {
    (void)activity;
    struct SleepSum *sum = (struct SleepSum *)context;
    time_t from = time_start > sum->window_start ? time_start : sum->window_start;
    time_t to = time_end < sum->window_end ? time_end : sum->window_end;
    if (to > from) {
        sum->seconds += to - from;
        sum->sessions++;
    }
    return true;
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
    
    // Sleep. health_service_sum() is documented as "based on daily totals,
    // weighted for the length of the time range", so it cannot give an
    // overnight session. Walk the actual sleep sessions instead and add up
    // the part of each that falls after 18:00 yesterday.
    HealthMetric sleep_metric = HealthMetricSleepSeconds;
    time_t sleep_window_start = start - 6 * 3600;
    struct SleepSum sum = { .window_start = sleep_window_start, .window_end = end, .seconds = 0, .sessions = 0 };
    health_service_activities_iterate(HealthActivitySleep, sleep_window_start, end,
                                      HealthIterationDirectionPast, sleep_session_cb, &sum);
    HealthServiceAccessibilityMask sleep_mask = health_service_metric_accessible(sleep_metric, start, end);
    if (sum.sessions > 0 || (sleep_mask & HealthServiceAccessibilityMaskAvailable)) {
        state->health.sleep_seconds = sum.seconds;
        state->health.sleep_trend = 0;
        // Baseline: the watch's average sleep total for a whole day, which is
        // how it credits a night's sleep. Same quantity, so the arrow compares
        // like with like; no arrow until an average exists.
        time_t day_end = start + 24 * 3600;
        HealthServiceAccessibilityMask sleep_avg_mask = health_service_metric_averaged_accessible(sleep_metric, start, day_end, scope);
        if (sum.sessions > 0 && (sleep_avg_mask & HealthServiceAccessibilityMaskAvailable)) {
            HealthValue avg = health_service_sum_averaged(sleep_metric, start, day_end, scope);
            if (avg > 0) {
                if (state->health.sleep_seconds > (time_t)avg) state->health.sleep_trend = 1;
                else if (state->health.sleep_seconds < (time_t)avg) state->health.sleep_trend = -1;
            }
        }
    } else {
        state->health.sleep_seconds = -1;
        state->health.sleep_trend = 0;
    }
    #endif
}
