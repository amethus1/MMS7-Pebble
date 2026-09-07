#pragma once

#include <stdbool.h>
#include <stddef.h>

// Reasons carried by KEY_WEATHER_FETCH_ERROR (kept in sync with weather_api.js)
#define WEATHER_ERROR_FETCH_FAILED   1   // Network / API problem; cached weather stays
#define WEATHER_ERROR_NO_LOCATION    2   // GPS unavailable and no city configured
#define WEATHER_ERROR_CITY_NOT_FOUND 3   // The configured city did not geocode

int c_to_f_rounded(int temp_c);

// Location line: "~Name" while the position is unconfirmed, or a short
// instruction when there is no usable location at all.
void weather_format_location(char *buffer, size_t size, const char *name, bool unconfirmed, int error_reason);
// Weather-age field: "!" in front of the age after a failed fetch, so the
// warning does not depend on colour.
void weather_format_age(char *buffer, size_t size, const char *elapsed, bool fetch_failed);

void weather_format_wind(char *buffer, size_t size, int wind_kmh, int speed_unit);
void weather_format_pressure(char *buffer, size_t size, int pressure_hpa, int pressure_unit);
void weather_format_hi_lo(char *buffer, size_t size, int high_c, int low_c, bool degree_f);
