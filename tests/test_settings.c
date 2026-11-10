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
    check("default heart rate off", s->HeartRate, 0);

    TupleValue v[5];
    Tuple tuples[5];
    tuples[0] = make_int_tuple(KEY_SET_EXTRA_INFO_CENTER, &v[0], 1);
    tuples[1] = make_int_tuple(KEY_SET_LEADING_ZERO, &v[1], 0);
    tuples[2] = make_int_tuple(KEY_SET_HIDE_BATTERY_TIME, &v[2], 1);
    tuples[3] = make_int_tuple(KEY_SET_SLEEP_UNTIL_HOUR, &v[3], 8);
    tuples[4] = make_int_tuple(KEY_SET_HEART_RATE, &v[4], 1);
    mock_dict_set(tuples, 5);
    bool changed = settings_handle_app_message(NULL);
    check("message reported as a change", changed ? 1 : 0, 1);
    check("extra info centred after message", s->ExtraInfoCenter, 1);
    check("leading zero after message", s->LeadingZero, 0);
    check("hide battery time after message", s->HideBatteryTime, 1);
    check("sleep cutoff after message", s->SleepUntilHour, 8);
    check("heart rate on after message", s->HeartRate, 1);

    // Persist round trip: save, wipe the defaults back in, load
    settings_save();
    s->ExtraInfoCenter = 0; s->LeadingZero = 1; s->HideBatteryTime = 0; s->SleepUntilHour = 10; s->HeartRate = 0;
    settings_load();
    check("extra info centred after reload", s->ExtraInfoCenter, 1);
    check("leading zero after reload", s->LeadingZero, 0);
    check("hide battery time after reload", s->HideBatteryTime, 1);
    check("sleep cutoff after reload", s->SleepUntilHour, 8);
    check("heart rate on after reload", s->HeartRate, 1);

    // Out-of-range values are clamped
    Tuple bad[2]; TupleValue bv[2];
    bad[0] = make_int_tuple(KEY_SET_SLEEP_UNTIL_HOUR, &bv[0], 99);
    bad[1] = make_int_tuple(KEY_SET_HEART_RATE, &bv[1], 7);
    mock_dict_set(bad, 2);
    settings_handle_app_message(NULL);
    check("sleep cutoff clamped", s->SleepUntilHour <= 23 ? 1 : 0, 1);
    check("heart rate clamped", s->HeartRate, 1);

    // Custom colour scheme: defaults give a readable face, and every colour
    // arrives, is stored, and survives a restart in its own slot.
    static const int defaults[CUSTOM_COLOR_COUNT] = CUSTOM_COLOR_DEFAULTS;
    for (int i = 0; i < CUSTOM_COLOR_COUNT; i++) {
        check("custom colour default", s->CustomColors[i], defaults[i]);
    }
    check("custom background and clock differ by default",
          s->CustomColors[CUSTOM_COLOR_BACKGROUND] != s->CustomColors[CUSTOM_COLOR_CLOCK], 1);
    check("icon coloured by weather by default", s->CustomIconAuto, 1);

    static const uint32_t custom_keys[CUSTOM_COLOR_COUNT] = {
        KEY_SET_CUSTOM_BACKGROUND, KEY_SET_CUSTOM_CLOCK, KEY_SET_CUSTOM_DATE, KEY_SET_CUSTOM_WEATHER,
        KEY_SET_CUSTOM_ICON, KEY_SET_CUSTOM_HEADER_TEXT, KEY_SET_CUSTOM_HEADER_BG, KEY_SET_CUSTOM_SUN,
        KEY_SET_CUSTOM_EXTRA, KEY_SET_CUSTOM_BLUETOOTH, KEY_SET_CUSTOM_BATTERY, KEY_SET_CUSTOM_LINES,
    };
    Tuple ct[CUSTOM_COLOR_COUNT + 2]; TupleValue cv[CUSTOM_COLOR_COUNT + 2];
    for (int i = 0; i < CUSTOM_COLOR_COUNT; i++) {
        // A different colour in every slot, so a crossed wire shows up
        ct[i] = make_int_tuple(custom_keys[i], &cv[i], 0x010203 * (i + 1));
    }
    ct[CUSTOM_COLOR_COUNT] = make_int_tuple(KEY_SET_CUSTOM_ICON_AUTO, &cv[CUSTOM_COLOR_COUNT], 0);
    ct[CUSTOM_COLOR_COUNT + 1] = make_int_tuple(KEY_SET_INVERT_COLOR, &cv[CUSTOM_COLOR_COUNT + 1], COLOR_PROFILE_CUSTOM);
    mock_dict_set(ct, CUSTOM_COLOR_COUNT + 2);
    check("custom colours reported as a change", settings_handle_app_message(NULL) ? 1 : 0, 1);
    check("custom scheme selected", s->ColorProfile, COLOR_PROFILE_CUSTOM);
    for (int i = 0; i < CUSTOM_COLOR_COUNT; i++) {
        check("custom colour after message", s->CustomColors[i], 0x010203 * (i + 1));
    }
    check("icon colour toggle after message", s->CustomIconAuto, 0);

    for (int i = 0; i < CUSTOM_COLOR_COUNT; i++) s->CustomColors[i] = 0;
    s->CustomIconAuto = 1;
    s->ColorProfile = 0;
    settings_load();
    check("custom scheme after reload", s->ColorProfile, COLOR_PROFILE_CUSTOM);
    for (int i = 0; i < CUSTOM_COLOR_COUNT; i++) {
        check("custom colour after reload", s->CustomColors[i], 0x010203 * (i + 1));
    }
    check("icon colour toggle after reload", s->CustomIconAuto, 0);

    // Anything above 0xRRGGBB is dropped rather than passed to the display
    Tuple cb[1]; TupleValue cbv[1];
    cb[0] = make_int_tuple(KEY_SET_CUSTOM_CLOCK, &cbv[0], (int32_t)0x7F00FF55);
    mock_dict_set(cb, 1);
    settings_handle_app_message(NULL);
    check("custom colour masked to 24 bits", s->CustomColors[CUSTOM_COLOR_CLOCK], 0x00FF55);

    if (s_failures == 0) { printf("ALL TESTS PASSED\n"); return 0; }
    return 1;
}
