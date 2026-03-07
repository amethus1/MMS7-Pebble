#pragma once

#include <stdbool.h>
#include <stddef.h>

void weather_format_wind(char *buffer, size_t size, int wind_kmh, int speed_unit);
void weather_format_pressure(char *buffer, size_t size, int pressure_hpa, int pressure_unit);
void weather_format_hi_lo(char *buffer, size_t size, int high_c, int low_c, bool degree_f);
