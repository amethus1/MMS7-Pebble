#include "weather_format.h"

#include <stdio.h>

int c_to_f_rounded(int temp_c) {
    if (temp_c >= 0) {
        return (temp_c * 9 + 2) / 5 + 32;
    }
    return (temp_c * 9 - 2) / 5 + 32;
}

static int round_ratio(int value, int numerator, int denominator) {
    if (value >= 0) {
        return (value * numerator + denominator / 2) / denominator;
    }
    return (value * numerator - denominator / 2) / denominator;
}

void weather_format_wind(char *buffer, size_t size, int wind_kmh, int speed_unit) {
    if (speed_unit == 1) {
        int mph = round_ratio(wind_kmh, 621371, 1000000);
        snprintf(buffer, size, "%d mph", mph);
        return;
    }
    if (speed_unit == 2) {
        int ms = round_ratio(wind_kmh, 1000, 3600);
        snprintf(buffer, size, "%d m/s", ms);
        return;
    }
    snprintf(buffer, size, "%d km/h", wind_kmh);
}

void weather_format_pressure(char *buffer, size_t size, int pressure_hpa, int pressure_unit) {
    if (pressure_unit == 1) {
        int mmhg = round_ratio(pressure_hpa, 750062, 1000000);
        snprintf(buffer, size, "%d mmHg", mmhg);
        return;
    }
    if (pressure_unit == 2) {
        int inhg_hundredths = round_ratio(pressure_hpa, 2953, 1000);
        snprintf(buffer, size, "%d.%02d inHg", inhg_hundredths / 100, inhg_hundredths % 100);
        return;
    }
    snprintf(buffer, size, "%d hPa", pressure_hpa);
}

void weather_format_hi_lo(char *buffer, size_t size, int high_c, int low_c, bool degree_f) {
    if (high_c < -100 || high_c > 100 || low_c < -100 || low_c > 100) {
        snprintf(buffer, size, "--/--");
        return;
    }

    if (degree_f) {
        high_c = c_to_f_rounded(high_c);
        low_c = c_to_f_rounded(low_c);
    }

    snprintf(buffer, size, "%d\xc2\xb0/%d\xc2\xb0", high_c, low_c);
}

void weather_format_location(char *buffer, size_t size, const char *name, bool unconfirmed, int error_reason) {
    if (error_reason == WEATHER_ERROR_NO_LOCATION) {
        snprintf(buffer, size, "%s", "Set a city");
    } else if (error_reason == WEATHER_ERROR_CITY_NOT_FOUND) {
        snprintf(buffer, size, "%s", "City not found");
    } else if (!name || name[0] == '\0') {
        snprintf(buffer, size, "%s", "No weather");
    } else {
        snprintf(buffer, size, "%s%s", unconfirmed ? "~" : "", name);
    }
}

void weather_format_age(char *buffer, size_t size, const char *elapsed, bool fetch_failed) {
    snprintf(buffer, size, "%s%s", fetch_failed ? "!" : "", elapsed ? elapsed : "");
}
