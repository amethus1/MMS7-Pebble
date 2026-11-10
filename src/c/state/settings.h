#pragma once
#include <pebble.h>
#include "../config.h"
#include "../modules/custom_colors.h"

// Defined in config.h but useful to have visible here
#ifndef DATE_FORMAT
#define DATE_FORMAT "%a, %d.%m.%Y"
#endif

typedef struct {
  // Appearance
  int ColorProfile;
  int ColoredTemperature;
  int LightOn;
  int DisplaySeconds;
  
  // Vibration
  int vibe_on_disconnect;
  int vibe_on_hour;
  
  // Units/Formats
  int degree_f;
  char date_format[20];
  int TimeZoneFormat;
  int SpeedUnit;
  int PressureUnit;
  int ShowGridLines;
  
  // Data Customization
  int WeatherUpdateInterval;
  int WeatherLabelIndex1;
  int WeatherLabelIndex2;
  int WeatherLabelIndex3;
  int WeatherLabelIndex4;
  int ShowTimeSinceStationData;
  int ShowWeekInDate;
  int WeekNumberType;
  int FiscalStartMonth;
  int FiscalStartDay;
  int HealthInfo;
  int MoonPhase;
  
  // Hiding Elements
  int HideCW;
  int HideBluetooth;
  int HideBatteryTime;   // 1 hides the time-since-charge under the battery
  int LeadingZero;       // 0 drops the leading zero of single-digit hours
  int SleepUntilHour;    // Auto health mode: sleep until this hour, steps after
  int ExtraInfoCenter;   // Emery: timezone / health centred in the status row, week on the left
  int HeartRate;         // 1 shows heart rate in the week label's slot (the week can still go in the date line)
  
  // Custom colour scheme, used while ColorProfile is COLOR_PROFILE_CUSTOM
  int CustomColors[CUSTOM_COLOR_COUNT];   // 0xRRGGBB, indexed by CustomColor
  int CustomIconAuto;                     // 1 colours the weather icon by conditions

  // State
  int AppFirstStart;
  int warning_color_location;
} GlobalSettings;

// Get the singleton settings instance
GlobalSettings* settings_get_instance();

// Load all settings from persistent storage
void settings_load();

// Save all settings to persistent storage
void settings_save();

// Handle Clay configuration messages
// Returns true if settings were updated
bool settings_handle_app_message(DictionaryIterator *iterator);
