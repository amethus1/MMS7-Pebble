// ---- Constants for internet-access (weather) and persistent variables:
#define KEY_LOCATION_NAME       0
#define KEY_LOCATION_LAT        1
#define KEY_LOCATION_LON        2
#define KEY_WEATHER_TEMP        3 
#define KEY_WEATHER_ICON        4
#define KEY_WEATHER_HIGH      112
#define KEY_WEATHER_LOW       113
#define KEY_SET_MOON_PHASE      5
#define KEY_WEATHER_DATA_TIME   6 //Update time of server in Unix local time
#define KEY_WEATHER_UPDATE_INT  7 //Update Interval in minutes
#define KEY_TIME_UTC_OFFSET    10
#define KEY_TIME_LAST_UPDATE   11
#define KEY_TIME_ZONE_NAME     12
#define KEY_WEATHER_HUMIDITY  114 // Humidity in percent
#define KEY_WEATHER_PRESSURE  115 // Pressure in hPa
#define KEY_WEATHER_WIND_KMH  116 // Wind speed in km/h
#define KEY_WEATHER_CONDITIONS 117 // Raw weather conditions text
#define KEY_BTY_LAST_CHARGED   13 //BTY = BATTERY
#define KEY_BTY_LAST_PERCENT   14
#define KEY_BTY_PERCENT_EXIT   15
#define KEY_SUN_RISE           16
#define KEY_SUN_SET            17
#define KEY_BTY_LAST_STATE     18
#define KEY_BTY_LAST_PERIOD    19
#define KEY_SET_INVERT_COLOR   20 //SET = SETTINGS
#define KEY_SET_DISPLAY_SEC    21
#define KEY_SET_LIGHT_ON       22
#define KEY_SET_VIBE_DISC      23
#define KEY_SET_COLORED_TMP    24
#define KEY_SET_DEGREE_F       25
#define KEY_SET_DATE_FORMAT    26
#define KEY_SET_TZ_FORMAT      27
#define KEY_SET_VIBE_HOUR      28
#define KEY_SET_UPDATE_TIME    29
#define KEY_SET_HEALTH         30
#define KEY_SET_WEEK_IN_DATE   42

#define KEY_SET_LABEL_INDEX_1  31
#define KEY_SET_LABEL_INDEX_2  32
#define KEY_SET_LABEL_INDEX_3  33
#define KEY_SET_LABEL_INDEX_4  34

#define KEY_SUN_RISE_UNIX      40
#define KEY_SUN_SET_UNIX       41
#define KEY_HIDE_CW            43
#define KEY_HIDE_BLUETOOTH     44
#define KEY_SET_WEEK_TYPE      46
#define KEY_SET_FISCAL_START_MONTH 47
#define KEY_SET_FISCAL_START_DAY   48
#define KEY_SET_SPEED_UNIT     60
#define KEY_SET_PRESSURE_UNIT  61
#define KEY_SET_SHOW_GRID      62

#define KEY_WARN_LOCATION      50

#define KEY_WEATHER_AVWX_STRING_1  101
#define KEY_WEATHER_AVWX_STRING_2  102
#define KEY_WEATHER_AVWX_STRING_3  103
#define KEY_WEATHER_AVWX_STRING_4  104
#define KEY_WEATHER_AVWX_STRING_5  105
#define KEY_WEATHER_AVWX_STRING_6  106
#define KEY_WEATHER_AVWX_STRING_7  107
#define KEY_WEATHER_AVWX_STRING_8  108
#define KEY_WEATHER_AVWX_STRING_9  109
  
#define KEY_FETCH_TRIGGER          200
#define KEY_DETECT_FIRST_START     201
#define KEY_WEATHER_FETCH_ERROR    202  // value = reason, see weather_format.h
#define KEY_LOCATION_UNCONFIRMED   203  // 1 when the weather is for a remembered position
#define KEY_SET_HIDE_BATTERY_TIME  204  // 1 hides the time-since-charge under the battery
#define KEY_SET_LEADING_ZERO       205  // 0 shows 8:30 instead of 08:30
#define KEY_SET_SLEEP_UNTIL_HOUR   206  // Auto health mode shows sleep until this hour
