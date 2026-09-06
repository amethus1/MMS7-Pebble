#include "formatting.h"
#include "../modules/weather_format.h"

void format_temperature(char* buffer, size_t buf_size, int temp_c, bool is_metric) {
    if (is_metric) {
        snprintf(buffer, buf_size, "%d°", temp_c);
    } else {
        snprintf(buffer, buf_size, "%d°", c_to_f_rounded(temp_c));
    }
}

void format_condition_text(char* buffer, size_t buf_size, const char* raw_desc) {
    // Pass-through for now, or map lookup later
    snprintf(buffer, buf_size, "%s", raw_desc);
}

void format_battery_status(char* buffer, size_t buf_size, BatteryState* component) {
    if (component->is_charging) {
        snprintf(buffer, buf_size, "CHG");
    } else {
        snprintf(buffer, buf_size, "%d%%", component->charge_percent);
    }
}

void format_health_string(char* buffer, size_t buf_size, HealthState* component) {
    // Example: "5000"
    snprintf(buffer, buf_size, "%d", component->steps);
}

void format_time_elapsed(char* buffer, size_t buf_size, time_t seconds) {
    int days = seconds / (24 * 3600);
    int hours = (seconds % (24 * 3600)) / 3600;
    int minutes = (seconds % 3600) / 60;
    
    if (days > 0) {
        if (days < 10) {
            snprintf(buffer, buf_size, "%dd %dh", days, hours);
        } else {
            snprintf(buffer, buf_size, "%d d", days);
        }
    } else if (hours > 0) {
        if (hours < 10) {
            snprintf(buffer, buf_size, "%dh %dm", hours, minutes);
        } else {
            snprintf(buffer, buf_size, "%d h", hours);
        }
    } else if (minutes > 0) {
        snprintf(buffer, buf_size, "%d m", minutes);
    } else {
        snprintf(buffer, buf_size, "< 1m");
    }
}

void format_sun_time(char* buffer, size_t buf_size, time_t unix_time, int utc_offset_seconds, bool is_24h) {
    time_t localized_time = unix_time + utc_offset_seconds;
    struct tm* t = gmtime(&localized_time);
    if (!t) {
        snprintf(buffer, buf_size, "--:--");
        return;
    }
    if (is_24h) {
        strftime(buffer, buf_size, "%H:%M", t);
    } else {
        strftime(buffer, buf_size, "%I:%M", t);
    }
}

TextLayer* create_text_layer(GRect frame, const char* font_key, GTextAlignment align, GColor text_color) {
    TextLayer* tl = text_layer_create(frame);
    text_layer_set_font(tl, fonts_get_system_font(font_key));
    text_layer_set_text_alignment(tl, align);
    text_layer_set_background_color(tl, GColorClear);
    text_layer_set_text_color(tl, text_color);
    return tl;
}
