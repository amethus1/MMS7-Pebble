#include <stdio.h>
#include "../src/c/modules/rules.h"

static int s_failures = 0;
static void check(const char *name, bool actual, bool expected) {
    if (actual == expected) return;
    s_failures++;
    printf("FAIL %s: expected %d got %d\n", name, expected, actual);
}

int main(void) {
    // Precipitation glyphs per the Climacons mapping in weather_api.js
    check("rain is precipitation", rules_icon_is_precipitation('%'), true);
    check("snow is precipitation", rules_icon_is_precipitation(':'), true);
    check("lightning is precipitation", rules_icon_is_precipitation('G'), true);
    check("fog is not", rules_icon_is_precipitation('='), false);
    check("sun is not", rules_icon_is_precipitation('I'), false);
    check("cloud is not", rules_icon_is_precipitation('!'), false);
    check("moon is not", rules_icon_is_precipitation('N'), false);

    // Moon modes
    check("always", rules_show_moon(MOON_MODE_ALWAYS, false, true, '%'), true);
    check("never", rules_show_moon(MOON_MODE_NEVER, true, true, 'I'), false);
    check("night mode by day", rules_show_moon(MOON_MODE_NIGHT, false, true, 'I'), false);
    check("night mode at night", rules_show_moon(MOON_MODE_NIGHT, true, true, '%'), true);
    check("clear night shows moon", rules_show_moon(MOON_MODE_NIGHT_IF_CLEAR, true, true, 'N'), true);
    check("rainy night shows weather", rules_show_moon(MOON_MODE_NIGHT_IF_CLEAR, true, true, '&'), false);
    check("rainy day shows weather", rules_show_moon(MOON_MODE_NIGHT_IF_CLEAR, false, true, '&'), false);
    check("stale rain icon does not veto the moon", rules_show_moon(MOON_MODE_NIGHT_IF_CLEAR, true, false, '&'), true);

    // Morning sleep
    check("sleep at 7 with cutoff 10", rules_show_sleep(7, 10), true);
    check("steps at 10 with cutoff 10", rules_show_sleep(10, 10), false);
    check("steps at 22", rules_show_sleep(22, 10), false);
    check("cutoff 0 never shows sleep", rules_show_sleep(3, 0), false);

    if (s_failures == 0) { printf("ALL TESTS PASSED\n"); return 0; }
    return 1;
}
