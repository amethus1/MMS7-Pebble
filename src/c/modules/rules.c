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
