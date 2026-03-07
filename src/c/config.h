// Default coordinates - overridden by GPS when available
// NOTE: longitude is positive for East and negative for West
// BERLIN:
// 52.519444°, 13.406667°
#define LATITUDE   52.519444
#define LONGITUDE  13.406667
#define TIMEZONE   +1

// Translation for the calendar week (e.g. "CW%V")
#define TRANSLATION_CW_DE "KW%V"
#define TRANSLATION_CW_EN "CW%V"
#define TRANSLATION_CW_FR "S.%V"

//(un-)comment to change initial settings
#ifdef PBL_COLOR
  #define INVERT_COLORS 2 //2 means colors on Basalt, and white on black on Aplite
#else
  #define INVERT_COLORS 0
#endif
#define LIGHT_ON      1
#define VIBE_ON_DISC 0 //disconnect
#define VIBE_ON_FULL 0
#define VIBE_ON_HOUR 0
#define DEGREE_F     1
#define DATE_FORMAT  "%a, %d.%m.%Y"

// Screen dimensions
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define SCREEN_WIDTH_ROUND 180
#define SCREEN_HEIGHT_ROUND 180

// Time calculation constants
#ifndef SECONDS_PER_MINUTE
#define SECONDS_PER_MINUTE 60
#endif
#ifndef SECONDS_PER_HOUR
#define SECONDS_PER_HOUR 3600
#endif
#ifndef SECONDS_PER_DAY
#define SECONDS_PER_DAY 86400
#endif
#define TEN_YEARS_SECONDS (10 * 365 * SECONDS_PER_DAY)

// Weather update defaults
#define WEATHER_UPDATE_INTERVAL_MINUTE 30

//#ifndef PBL_PLATFORM_APLITE
  #define COMPILE_WITH_SECONDS
//#endif
#define DISPLAY_SECONDS 0
  
// uncomment this to show all colors of the temperature
//#define ITERATE_TEMP

// uncomment to use a different method to get HH:MM:SS:
//#define GET_TIME_FROM_STRING

#ifdef PBL_ROUND
  #define X_OFFSET ((180-144)/2)
  #define Y_OFFSET ((180-168)/2)
#else
  #define X_OFFSET 0
  #define Y_OFFSET 0
#endif

#define MAX_NO_COLOR_PROFILES 15 // Maximum static color profile index

// Buffer size constants
#define BUFFER_SIZE_SMALL    10  // Timezone, short labels
#define BUFFER_SIZE_MEDIUM   32  // Weather strings, location names
#define BUFFER_SIZE_LARGE    64  // Date formats with week numbers
