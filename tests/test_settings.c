#include "mock_pebble.h"
#include "../src/c/state/settings.h"
#include "../src/c/keys.h"

// Settings sent by the phone must be handled, and survive a save/load.

static int s_failures = 0;
static void check(const char *name, int actual, int expected) {
    if (actual == expected) return;
    s_failures++;
    printf("FAIL %s: expected %d got %d\n", name, expected, actual);
}

static Tuple make_int_tuple(uint32_t key, TupleValue* storage, int32_t value) {
    storage->int32 = value;
    Tuple t = { .key = key, .type = TUPLE_INT, .length = 4, .value = storage };
    return t;
}

int main(void) {
    settings_load();
    GlobalSettings* s = settings_get_instance();
    check("default extra-info position", s->ExtraInfoCenter, 0);
    check("default leading zero", s->LeadingZero, 1);
    check("default sleep cutoff", s->SleepUntilHour, 10);

    TupleValue v[4];
    Tuple tuples[4];
    tuples[0] = make_int_tuple(KEY_SET_EXTRA_INFO_CENTER, &v[0], 1);
    tuples[1] = make_int_tuple(KEY_SET_LEADING_ZERO, &v[1], 0);
    tuples[2] = make_int_tuple(KEY_SET_HIDE_BATTERY_TIME, &v[2], 1);
    tuples[3] = make_int_tuple(KEY_SET_SLEEP_UNTIL_HOUR, &v[3], 8);
    mock_dict_set(tuples, 4);
    bool changed = settings_handle_app_message(NULL);
    check("message reported as a change", changed ? 1 : 0, 1);
    check("extra info centred after message", s->ExtraInfoCenter, 1);
    check("leading zero after message", s->LeadingZero, 0);
    check("hide battery time after message", s->HideBatteryTime, 1);
    check("sleep cutoff after message", s->SleepUntilHour, 8);

    // Persist round trip: save, wipe the defaults back in, load
    settings_save();
    s->ExtraInfoCenter = 0; s->LeadingZero = 1; s->HideBatteryTime = 0; s->SleepUntilHour = 10;
    settings_load();
    check("extra info centred after reload", s->ExtraInfoCenter, 1);
    check("leading zero after reload", s->LeadingZero, 0);
    check("hide battery time after reload", s->HideBatteryTime, 1);
    check("sleep cutoff after reload", s->SleepUntilHour, 8);

    // Out-of-range values are clamped
    Tuple bad[1]; TupleValue bv;
    bad[0] = make_int_tuple(KEY_SET_SLEEP_UNTIL_HOUR, &bv, 99);
    mock_dict_set(bad, 1);
    settings_handle_app_message(NULL);
    check("sleep cutoff clamped", s->SleepUntilHour <= 23 ? 1 : 0, 1);

    if (s_failures == 0) { printf("ALL TESTS PASSED\n"); return 0; }
    return 1;
}
