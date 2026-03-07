#include "settings.h"
#include "../keys.h"
#include <stdlib.h>

static GlobalSettings s_settings;

// Helper to extract int from tuple - handles both INT and CSTRING types
// This is critical because Clay may send values as strings!
static int tuple_to_int(Tuple *t, int fallback) {
    if (!t) return fallback;
    switch (t->type) {
        case TUPLE_INT:
        case TUPLE_UINT:
            return (int)t->value->int32;
        case TUPLE_CSTRING:
            return atoi(t->value->cstring);
        default:
            return fallback;
    }
}

static int clamp_int(int value, int min_value, int max_value) {
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

static void sanitize_settings(void) {
    s_settings.SpeedUnit = clamp_int(s_settings.SpeedUnit, 0, 2);
    s_settings.PressureUnit = clamp_int(s_settings.PressureUnit, 0, 2);
    s_settings.WeatherLabelIndex1 = clamp_int(s_settings.WeatherLabelIndex1, 0, 5);
    s_settings.WeatherLabelIndex2 = clamp_int(s_settings.WeatherLabelIndex2, 0, 5);
    s_settings.WeatherLabelIndex3 = clamp_int(s_settings.WeatherLabelIndex3, 0, 5);
    s_settings.WeatherLabelIndex4 = clamp_int(s_settings.WeatherLabelIndex4, 0, 5);
    s_settings.WeatherUpdateInterval = clamp_int(s_settings.WeatherUpdateInterval, 0, 360);
    s_settings.FiscalStartMonth = clamp_int(s_settings.FiscalStartMonth, 0, 11);
    s_settings.FiscalStartDay = clamp_int(s_settings.FiscalStartDay, 1, 31);
    s_settings.ShowGridLines = clamp_int(s_settings.ShowGridLines, 0, 1);
}

GlobalSettings* settings_get_instance() {
  return &s_settings;
}

void settings_load() {
  // Initialize defaults - MUST match Clay config.js defaults!
  s_settings.ColorProfile = INVERT_COLORS;
  s_settings.ColoredTemperature = 1;
  s_settings.LightOn = LIGHT_ON;
  s_settings.DisplaySeconds = DISPLAY_SECONDS;
  s_settings.vibe_on_disconnect = VIBE_ON_DISC;
  s_settings.vibe_on_hour = VIBE_ON_HOUR;
  s_settings.degree_f = DEGREE_F;
  snprintf(s_settings.date_format, sizeof(s_settings.date_format), "%s", DATE_FORMAT);
  s_settings.WeatherUpdateInterval = WEATHER_UPDATE_INTERVAL_MINUTE;
  s_settings.SpeedUnit = 0;
  s_settings.PressureUnit = 0;
  s_settings.ShowGridLines = 1;
  s_settings.WeatherLabelIndex1 = 5;
  s_settings.WeatherLabelIndex2 = 2;
  s_settings.WeatherLabelIndex3 = 3;
  s_settings.WeatherLabelIndex4 = 4;
  s_settings.ShowTimeSinceStationData = 0;
  s_settings.TimeZoneFormat = 1;    // Default: Timezone Name (matches Clay)
  s_settings.ShowWeekInDate = 0;
  s_settings.WeekNumberType = 0;    // Default: Calendar/ISO (matches Clay)
  s_settings.FiscalStartMonth = 0;
  s_settings.FiscalStartDay = 1;
  s_settings.HealthInfo = 5;        // Default: Timezone (matches Clay)
  s_settings.AppFirstStart = 1;
  s_settings.MoonPhase = 0;
  s_settings.HideCW = 0;
  s_settings.HideBluetooth = 0;
  s_settings.warning_color_location = 0;

  // Load from storage
  if (persist_exists(KEY_SET_INVERT_COLOR)) s_settings.ColorProfile = persist_read_int(KEY_SET_INVERT_COLOR);
  if (persist_exists(KEY_SET_COLORED_TMP)) s_settings.ColoredTemperature = persist_read_int(KEY_SET_COLORED_TMP);
  if (persist_exists(KEY_SET_LIGHT_ON)) s_settings.LightOn = persist_read_int(KEY_SET_LIGHT_ON);
  if (persist_exists(KEY_SET_DISPLAY_SEC)) s_settings.DisplaySeconds = persist_read_int(KEY_SET_DISPLAY_SEC);
  if (persist_exists(KEY_SET_VIBE_DISC)) s_settings.vibe_on_disconnect = persist_read_int(KEY_SET_VIBE_DISC);
  if (persist_exists(KEY_SET_VIBE_HOUR)) s_settings.vibe_on_hour = persist_read_int(KEY_SET_VIBE_HOUR);
  if (persist_exists(KEY_SET_DEGREE_F)) s_settings.degree_f = persist_read_int(KEY_SET_DEGREE_F);
  if (persist_exists(KEY_SET_DATE_FORMAT)) persist_read_string(KEY_SET_DATE_FORMAT, s_settings.date_format, sizeof(s_settings.date_format));
  if (persist_exists(KEY_SET_TZ_FORMAT)) s_settings.TimeZoneFormat = persist_read_int(KEY_SET_TZ_FORMAT);
  if (persist_exists(KEY_SET_SPEED_UNIT)) s_settings.SpeedUnit = persist_read_int(KEY_SET_SPEED_UNIT);
  if (persist_exists(KEY_SET_PRESSURE_UNIT)) s_settings.PressureUnit = persist_read_int(KEY_SET_PRESSURE_UNIT);
  if (persist_exists(KEY_SET_SHOW_GRID)) s_settings.ShowGridLines = persist_read_int(KEY_SET_SHOW_GRID);
  if (persist_exists(KEY_SET_HEALTH)) s_settings.HealthInfo = persist_read_int(KEY_SET_HEALTH);
  if (persist_exists(KEY_WEATHER_UPDATE_INT)) s_settings.WeatherUpdateInterval = persist_read_int(KEY_WEATHER_UPDATE_INT);
  if (persist_exists(KEY_SET_LABEL_INDEX_1)) s_settings.WeatherLabelIndex1 = persist_read_int(KEY_SET_LABEL_INDEX_1);
  if (persist_exists(KEY_SET_LABEL_INDEX_2)) s_settings.WeatherLabelIndex2 = persist_read_int(KEY_SET_LABEL_INDEX_2);
  if (persist_exists(KEY_SET_LABEL_INDEX_3)) s_settings.WeatherLabelIndex3 = persist_read_int(KEY_SET_LABEL_INDEX_3);
  if (persist_exists(KEY_SET_LABEL_INDEX_4)) s_settings.WeatherLabelIndex4 = persist_read_int(KEY_SET_LABEL_INDEX_4);
  if (persist_exists(KEY_SET_UPDATE_TIME)) s_settings.ShowTimeSinceStationData = persist_read_int(KEY_SET_UPDATE_TIME);
  if (persist_exists(KEY_SET_WEEK_IN_DATE)) s_settings.ShowWeekInDate = persist_read_int(KEY_SET_WEEK_IN_DATE);
  if (persist_exists(KEY_SET_WEEK_TYPE)) s_settings.WeekNumberType = persist_read_int(KEY_SET_WEEK_TYPE);
  if (persist_exists(KEY_SET_FISCAL_START_MONTH)) s_settings.FiscalStartMonth = persist_read_int(KEY_SET_FISCAL_START_MONTH);
  if (persist_exists(KEY_SET_FISCAL_START_DAY)) s_settings.FiscalStartDay = persist_read_int(KEY_SET_FISCAL_START_DAY);
  if (persist_exists(KEY_SET_MOON_PHASE)) s_settings.MoonPhase = persist_read_int(KEY_SET_MOON_PHASE);
  if (persist_exists(KEY_HIDE_CW)) s_settings.HideCW = persist_read_int(KEY_HIDE_CW);
  if (persist_exists(KEY_HIDE_BLUETOOTH)) s_settings.HideBluetooth = persist_read_int(KEY_HIDE_BLUETOOTH);
  
  if (persist_exists(KEY_DETECT_FIRST_START)) s_settings.AppFirstStart = persist_read_int(KEY_DETECT_FIRST_START);
  else s_settings.AppFirstStart = 1;
  
  if (persist_exists(KEY_WARN_LOCATION)) s_settings.warning_color_location = persist_read_int(KEY_WARN_LOCATION);
  sanitize_settings();
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Loaded: DisplaySeconds=%d, HealthInfo=%d", s_settings.DisplaySeconds, s_settings.HealthInfo);
}

void settings_save() {
  persist_write_int(KEY_SET_INVERT_COLOR, s_settings.ColorProfile);
  persist_write_int(KEY_SET_COLORED_TMP, s_settings.ColoredTemperature);
  persist_write_int(KEY_SET_LIGHT_ON, s_settings.LightOn);
  persist_write_int(KEY_SET_DISPLAY_SEC, s_settings.DisplaySeconds);
  persist_write_int(KEY_SET_VIBE_DISC, s_settings.vibe_on_disconnect);
  persist_write_int(KEY_SET_VIBE_HOUR, s_settings.vibe_on_hour);
  persist_write_int(KEY_SET_DEGREE_F, s_settings.degree_f);
  persist_write_string(KEY_SET_DATE_FORMAT, s_settings.date_format);
  persist_write_int(KEY_SET_TZ_FORMAT, s_settings.TimeZoneFormat);
  persist_write_int(KEY_SET_SPEED_UNIT, s_settings.SpeedUnit);
  persist_write_int(KEY_SET_PRESSURE_UNIT, s_settings.PressureUnit);
  persist_write_int(KEY_SET_SHOW_GRID, s_settings.ShowGridLines);
  persist_write_int(KEY_SET_HEALTH, s_settings.HealthInfo);
  persist_write_int(KEY_WEATHER_UPDATE_INT, s_settings.WeatherUpdateInterval);
  persist_write_int(KEY_SET_LABEL_INDEX_1, s_settings.WeatherLabelIndex1);
  persist_write_int(KEY_SET_LABEL_INDEX_2, s_settings.WeatherLabelIndex2);
  persist_write_int(KEY_SET_LABEL_INDEX_3, s_settings.WeatherLabelIndex3);
  persist_write_int(KEY_SET_LABEL_INDEX_4, s_settings.WeatherLabelIndex4);
  persist_write_int(KEY_SET_UPDATE_TIME, s_settings.ShowTimeSinceStationData);
  persist_write_int(KEY_SET_WEEK_IN_DATE, s_settings.ShowWeekInDate);
  persist_write_int(KEY_SET_WEEK_TYPE, s_settings.WeekNumberType);
  persist_write_int(KEY_SET_FISCAL_START_MONTH, s_settings.FiscalStartMonth);
  persist_write_int(KEY_SET_FISCAL_START_DAY, s_settings.FiscalStartDay);
  persist_write_int(KEY_SET_MOON_PHASE, s_settings.MoonPhase);
  persist_write_int(KEY_HIDE_CW, s_settings.HideCW);
  persist_write_int(KEY_HIDE_BLUETOOTH, s_settings.HideBluetooth);
  
  persist_write_int(KEY_DETECT_FIRST_START, s_settings.AppFirstStart);
  persist_write_int(KEY_WARN_LOCATION, s_settings.warning_color_location);
}

bool settings_handle_app_message(DictionaryIterator *iterator) {
  Tuple *t = dict_read_first(iterator);
  bool changed = false;
  
  APP_LOG(APP_LOG_LEVEL_INFO, "=== SETTINGS MESSAGE RECEIVED ===");
  
  while(t != NULL) {
    int val = tuple_to_int(t, -999);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Key %d = %d (type %d)", (int)t->key, val, t->type);
    
    switch(t->key) {
      case KEY_SET_INVERT_COLOR:
        s_settings.ColorProfile = tuple_to_int(t, s_settings.ColorProfile);
        changed = true;
        break;
      case KEY_SET_COLORED_TMP:
        s_settings.ColoredTemperature = tuple_to_int(t, s_settings.ColoredTemperature);
        changed = true;
        break;
      case KEY_SET_LIGHT_ON:
        s_settings.LightOn = tuple_to_int(t, s_settings.LightOn);
        changed = true;
        break;
      case KEY_SET_DISPLAY_SEC:
        s_settings.DisplaySeconds = tuple_to_int(t, s_settings.DisplaySeconds);
        APP_LOG(APP_LOG_LEVEL_INFO, "DisplaySeconds = %d", s_settings.DisplaySeconds);
        changed = true;
        break;
      case KEY_SET_VIBE_DISC:
        s_settings.vibe_on_disconnect = tuple_to_int(t, s_settings.vibe_on_disconnect);
        changed = true;
        break;
      case KEY_SET_VIBE_HOUR:
        s_settings.vibe_on_hour = tuple_to_int(t, s_settings.vibe_on_hour);
        changed = true;
        break;
      case KEY_SET_DEGREE_F:
        s_settings.degree_f = tuple_to_int(t, s_settings.degree_f);
        changed = true;
        break;
      case KEY_SET_DATE_FORMAT:
        if (t->type == TUPLE_CSTRING) {
          snprintf(s_settings.date_format, sizeof(s_settings.date_format), "%s", t->value->cstring);
          changed = true;
        }
        break;
      case KEY_SET_TZ_FORMAT:
        s_settings.TimeZoneFormat = tuple_to_int(t, s_settings.TimeZoneFormat);
        changed = true;
        break;
      case KEY_SET_SPEED_UNIT:
        s_settings.SpeedUnit = tuple_to_int(t, s_settings.SpeedUnit);
        changed = true;
        break;
      case KEY_SET_PRESSURE_UNIT:
        s_settings.PressureUnit = tuple_to_int(t, s_settings.PressureUnit);
        changed = true;
        break;
      case KEY_SET_SHOW_GRID:
        s_settings.ShowGridLines = tuple_to_int(t, s_settings.ShowGridLines);
        changed = true;
        break;
      case KEY_SET_HEALTH:
        s_settings.HealthInfo = tuple_to_int(t, s_settings.HealthInfo);
        APP_LOG(APP_LOG_LEVEL_INFO, "HealthInfo = %d", s_settings.HealthInfo);
        changed = true;
        break;
      case KEY_WEATHER_UPDATE_INT:
        s_settings.WeatherUpdateInterval = tuple_to_int(t, s_settings.WeatherUpdateInterval);
        changed = true;
        break;
      case KEY_SET_LABEL_INDEX_1:
        s_settings.WeatherLabelIndex1 = tuple_to_int(t, s_settings.WeatherLabelIndex1);
        changed = true;
        break;
      case KEY_SET_LABEL_INDEX_2:
        s_settings.WeatherLabelIndex2 = tuple_to_int(t, s_settings.WeatherLabelIndex2);
        changed = true;
        break;
      case KEY_SET_LABEL_INDEX_3:
        s_settings.WeatherLabelIndex3 = tuple_to_int(t, s_settings.WeatherLabelIndex3);
        changed = true;
        break;
      case KEY_SET_LABEL_INDEX_4:
        s_settings.WeatherLabelIndex4 = tuple_to_int(t, s_settings.WeatherLabelIndex4);
        changed = true;
        break;
      case KEY_SET_UPDATE_TIME:
        s_settings.ShowTimeSinceStationData = tuple_to_int(t, s_settings.ShowTimeSinceStationData);
        changed = true;
        break;
      case KEY_SET_WEEK_IN_DATE:
        s_settings.ShowWeekInDate = tuple_to_int(t, s_settings.ShowWeekInDate);
        APP_LOG(APP_LOG_LEVEL_INFO, "ShowWeekInDate = %d", s_settings.ShowWeekInDate);
        changed = true;
        break;
      case KEY_SET_WEEK_TYPE:
        s_settings.WeekNumberType = tuple_to_int(t, s_settings.WeekNumberType);
        APP_LOG(APP_LOG_LEVEL_INFO, "WeekNumberType = %d (0=Cal, 1=Fiscal)", s_settings.WeekNumberType);
        changed = true;
        break;
      case KEY_SET_FISCAL_START_MONTH:
        s_settings.FiscalStartMonth = tuple_to_int(t, s_settings.FiscalStartMonth);
        APP_LOG(APP_LOG_LEVEL_INFO, "FiscalStartMonth = %d", s_settings.FiscalStartMonth);
        changed = true;
        break;
      case KEY_SET_FISCAL_START_DAY:
        s_settings.FiscalStartDay = tuple_to_int(t, s_settings.FiscalStartDay);
        APP_LOG(APP_LOG_LEVEL_INFO, "FiscalStartDay = %d", s_settings.FiscalStartDay);
        changed = true;
        break;
      case KEY_SET_MOON_PHASE:
        s_settings.MoonPhase = tuple_to_int(t, s_settings.MoonPhase);
        changed = true;
        break;
      case KEY_HIDE_CW:
        s_settings.HideCW = tuple_to_int(t, s_settings.HideCW);
        changed = true;
        break;
      case KEY_HIDE_BLUETOOTH:
        s_settings.HideBluetooth = tuple_to_int(t, s_settings.HideBluetooth);
        changed = true;
        break;
      case KEY_DETECT_FIRST_START:
        s_settings.AppFirstStart = tuple_to_int(t, s_settings.AppFirstStart);
        break;
      case KEY_WARN_LOCATION:
        s_settings.warning_color_location = tuple_to_int(t, s_settings.warning_color_location);
        changed = true;
        break;
      default:
        // Ignore weather/timezone keys - they're handled by weather module
        break;
    }
    t = dict_read_next(iterator);
  }
  
  if (changed) {
      sanitize_settings();
      APP_LOG(APP_LOG_LEVEL_INFO, "Settings changed, saving...");
      settings_save();
  }
  return changed;
}
