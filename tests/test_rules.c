#include <stdio.h>
#include <time.h>
#include "../src/c/modules/rules.h"

static int s_failures = 0;
static void check(const char *name, bool actual, bool expected) {
    if (actual == expected) return;
    s_failures++;
    printf("FAIL %s: expected %d got %d\n", name, expected, actual);
}

// Retail fiscal week for a calendar date (month 1-12)
static int retail_week(int year, int month, int day) {
    struct tm t = { .tm_year = year - 1900, .tm_mon = month - 1, .tm_mday = day, .tm_hour = 12 };
    mktime(&t);   // fills in tm_yday and tm_wday
    return rules_retail_fiscal_week(year, t.tm_yday, t.tm_wday);
}

static void check_int(const char *name, int actual, int expected) {
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

    // Retail fiscal weeks. Year starts: Jan 29 2023, Feb 4 2024, Feb 2 2025,
    // Feb 1 2026, Jan 31 2027 (each the Sunday nearest February 1).
    check_int("2026 starts on Sunday Feb 1", retail_week(2026, 2, 1), 1);
    check_int("first week runs through Saturday", retail_week(2026, 2, 7), 1);
    check_int("second week starts on Sunday", retail_week(2026, 2, 8), 2);
    check_int("Nov 9 2026", retail_week(2026, 11, 9), 41);
    check_int("Jan 1 2027 is still fiscal 2026", retail_week(2027, 1, 1), 48);
    check_int("fiscal 2026 ends Sat Jan 30 2027 in week 52", retail_week(2027, 1, 30), 52);
    check_int("fiscal 2027 starts Sun Jan 31 2027", retail_week(2027, 1, 31), 1);
    check_int("Feb 1 2027 is in week 1, not a new start", retail_week(2027, 2, 1), 1);
    check_int("fiscal 2025 starts Sun Feb 2 2025", retail_week(2025, 2, 2), 1);
    check_int("Sat Feb 1 2025 closes fiscal 2024 in week 52", retail_week(2025, 2, 1), 52);
    check_int("fiscal 2024 starts Sun Feb 4 2024", retail_week(2024, 2, 4), 1);
    check_int("fiscal 2023 is a 53-week year", retail_week(2024, 2, 3), 53);
    check_int("week 53 starts Sun Jan 28 2024", retail_week(2024, 1, 28), 53);
    check_int("fiscal 2023 starts Sun Jan 29 2023", retail_week(2023, 1, 29), 1);
    // Every week must be 7 days long and every year 52 or 53 weeks
    {
        int prev = retail_week(2020, 1, 1);
        struct tm d = { .tm_year = 120, .tm_mon = 0, .tm_mday = 1, .tm_hour = 12 };
        time_t day = mktime(&d);
        int run = 0;
        for (int i = 1; i < 366 * 20; i++) {
            day += 86400;
            struct tm *t = gmtime(&day);
            int week = rules_retail_fiscal_week(t->tm_year + 1900, t->tm_yday, t->tm_wday);
            if (week == prev) { run++; continue; }
            bool rollover = (week == 1 && (prev == 52 || prev == 53));
            if (i > 7 && run != 6) { s_failures++; printf("FAIL week %d lasted %d days\n", prev, run + 1); break; }
            if (week != prev + 1 && !rollover) { s_failures++; printf("FAIL week went %d -> %d\n", prev, week); break; }
            if (t->tm_wday != 0) { s_failures++; printf("FAIL week %d started on weekday %d\n", week, t->tm_wday); break; }
            prev = week;
            run = 0;
        }
    }

    if (s_failures == 0) { printf("ALL TESTS PASSED\n"); return 0; }
    return 1;
}
