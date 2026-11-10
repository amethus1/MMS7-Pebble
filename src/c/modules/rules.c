#include "rules.h"

bool rules_icon_is_precipitation(int icon_code) {
    // '$' .. ';' cover rain, showers, downpour, drizzle, sleet, hail, flurries
    // and snow in the Climacons mapping; 'F' .. 'H' are lightning. Fog ('<'..'>')
    // and the cloud/sun/moon glyphs are not precipitation.
    if (icon_code >= '$' && icon_code <= ';') return true;
    if (icon_code >= 'F' && icon_code <= 'H') return true;
    return false;
}

bool rules_show_moon(int moon_mode, bool is_night, bool weather_is_current, int icon_code) {
    switch (moon_mode) {
        case MOON_MODE_ALWAYS: return true;
        case MOON_MODE_NEVER:  return false;
        case MOON_MODE_NIGHT_IF_CLEAR:
            if (!is_night) return false;
            if (weather_is_current && rules_icon_is_precipitation(icon_code)) return false;
            return true;
        default: return is_night;
    }
}

bool rules_show_sleep(int hour_of_day, int sleep_until_hour) {
    return hour_of_day < sleep_until_hour;
}

static bool is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

// Days from February 1 to the Sunday nearest it (-3 .. +3), given the
// weekday February 1 falls on.
static int retail_start_offset(int feb1_wday) {
    return feb1_wday <= 3 ? -feb1_wday : 7 - feb1_wday;
}

int rules_retail_fiscal_week(int year, int yday, int wday) {
    const int feb1_yday = 31;
    int feb1_wday = (((wday - (yday - feb1_yday)) % 7) + 7) % 7;
    int days = yday - (feb1_yday + retail_start_offset(feb1_wday));
    if (days < 0) {
        // Still in the fiscal year that started in the previous calendar year
        int days_between_feb1s = is_leap_year(year - 1) ? 366 : 365;
        int prev_feb1_wday = (((feb1_wday - days_between_feb1s) % 7) + 7) % 7;
        days = (yday - feb1_yday) + days_between_feb1s - retail_start_offset(prev_feb1_wday);
    }
    return days / 7 + 1;
}
