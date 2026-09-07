#include "colors.h"

static ColorScheme SCHEMES[MAX_COLOR_PROFILES + 1];

void colors_init(void) {
  // 0: White on Black (Default)
  SCHEMES[0] = (ColorScheme){
    .clock = GColorWhite, .seconds = GColorWhite, .timezone = GColorWhite, 
    .steps = GColorWhite, .calendar_week = GColorWhite, .clock_bg = GColorBlack,
    .date = GColorWhite, .date_bg = GColorBlack,
    .weather = GColorWhite, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorWhite, .connection = GColorWhite, .status_bg = GColorBlack,
    .location = GColorWhite, .location_bg = GColorBlack,
    .last_update = GColorWhite, .last_update_bg = GColorBlack,
    .lines_bg = GColorWhite, .lines_minor = GColorDarkGray,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorLightGray
  };
  
  // 1: Black on White
  SCHEMES[1] = (ColorScheme){
    .clock = GColorBlack, .seconds = GColorBlack, .timezone = GColorBlack,
    .steps = GColorBlack, .calendar_week = GColorBlack, .clock_bg = GColorWhite,
    .date = GColorBlack, .date_bg = GColorWhite,
    .weather = GColorBlack, .weather_bg = GColorWhite,
    .moon = GColorBlack, .moon_bg = GColorWhite,
    .sun = GColorBlack, .connection = GColorBlack, .status_bg = GColorWhite,
    .location = GColorBlack, .location_bg = GColorWhite,
    .last_update = GColorBlack, .last_update_bg = GColorWhite,
    .lines_bg = GColorBlack, .lines_minor = GColorLightGray,
    .battery_text = GColorBlack, .battery_bg = GColorWhite,
    .battery_fill = GColorLightGray
  };
  
  // 2: White on Black with Colors
  SCHEMES[2] = (ColorScheme){
    // Dark gray on black is barely legible; the same slot shows steps in
    // white, so match the neighbouring calendar week instead.
    .clock = GColorWhite, .seconds = GColorWhite, .timezone = GColorLightGray,
    .steps = GColorWhite, .calendar_week = GColorLightGray, .clock_bg = GColorBlack,
    .date = GColorWhite, .date_bg = GColorBlack,
    .weather = GColorWhite, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorYellow, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorWhite, .location_bg = GColorBlack,
    .last_update = GColorWhite, .last_update_bg = GColorBlack,
    .lines_bg = GColorLightGray, .lines_minor = GColorDarkGray,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorKellyGreen
  };

  // 3: Black on White with Colors
  SCHEMES[3] = (ColorScheme){
    .clock = GColorBlack, .seconds = GColorBlack, .timezone = GColorDarkGray,
    .steps = GColorBlack, .calendar_week = GColorDarkGray, .clock_bg = GColorWhite,
    .date = GColorBlack, .date_bg = GColorWhite,
    .weather = GColorBlack, .weather_bg = GColorWhite,
    .moon = GColorBlack, .moon_bg = GColorWhite,
    .sun = GColorOrange, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorBlack, .location_bg = GColorWhite,
    .last_update = GColorBlack, .last_update_bg = GColorWhite,
    .lines_bg = GColorDarkGray, .lines_minor = GColorDarkGray,
    .battery_text = GColorBlack, .battery_bg = GColorWhite,
    .battery_fill = GColorKellyGreen
  };

  // 4: Red with Black Background
  SCHEMES[4] = (ColorScheme){
    .clock = GColorRed, .seconds = GColorOrange, .timezone = GColorDarkGray,
    .steps = GColorSunsetOrange, .calendar_week = GColorMagenta, .clock_bg = GColorBlack,
    .date = GColorRed, .date_bg = GColorBlack,
    .weather = GColorSunsetOrange, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorYellow, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorMelon, .location_bg = GColorDarkCandyAppleRed,
    .last_update = GColorRichBrilliantLavender, .last_update_bg = GColorJazzberryJam,
    .lines_bg = GColorDarkCandyAppleRed, .lines_minor = GColorDarkCandyAppleRed,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorKellyGreen
  };

  // 5: Green with Black Background
  SCHEMES[5] = (ColorScheme){
    .clock = GColorGreen, .seconds = GColorTiffanyBlue, .timezone = GColorDarkGray,
    .steps = GColorScreaminGreen, .calendar_week = GColorTiffanyBlue, .clock_bg = GColorBlack,
    .date = GColorTiffanyBlue, .date_bg = GColorBlack,
    .weather = GColorMediumSpringGreen, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorYellow, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorChromeYellow, .location_bg = GColorDarkGreen,
    .last_update = GColorWhite, .last_update_bg = GColorDarkGreen,
    .lines_bg = GColorWhite, .lines_minor = GColorDarkGray,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorKellyGreen
  };
  
  // 6: Blue with Black Background
  SCHEMES[6] = (ColorScheme){
    .clock = GColorBlue, .seconds = GColorVividCerulean, .timezone = GColorDarkGray,
    .steps = GColorElectricBlue, .calendar_week = GColorTiffanyBlue, .clock_bg = GColorBlack,
    .date = GColorBlue, .date_bg = GColorBlack,
    .weather = GColorBrightGreen, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorYellow, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorVividCerulean, .location_bg = GColorOxfordBlue,
    .last_update = GColorRichBrilliantLavender, .last_update_bg = GColorJazzberryJam,
    .lines_bg = GColorDukeBlue, .lines_minor = GColorDukeBlue,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorKellyGreen
  };

  // 7: Yellow with Black Background
  SCHEMES[7] = (ColorScheme){
    .clock = GColorChromeYellow, .seconds = GColorYellow, .timezone = GColorDarkGray,
    .steps = GColorYellow, .calendar_week = GColorChromeYellow, .clock_bg = GColorBlack,
    .date = GColorYellow, .date_bg = GColorBlack,
    .weather = GColorYellow, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorYellow, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorChromeYellow, .location_bg = GColorBulgarianRose,
    .last_update = GColorMelon, .last_update_bg = GColorBulgarianRose,
    .lines_bg = GColorWhite, .lines_minor = GColorDarkGray,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorKellyGreen
  };

  // 8: Purple with Black Background
  SCHEMES[8] = (ColorScheme){
    .clock = GColorRichBrilliantLavender, .seconds = GColorMagenta, .timezone = GColorDarkGray,
    .steps = GColorShockingPink, .calendar_week = GColorPurpureus, .clock_bg = GColorBlack,
    .date = GColorRichBrilliantLavender, .date_bg = GColorBlack,
    .weather = GColorRichBrilliantLavender, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorYellow, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorRichBrilliantLavender, .location_bg = GColorImperialPurple,
    .last_update = GColorWhite, .last_update_bg = GColorImperialPurple,
    .lines_bg = GColorWhite, .lines_minor = GColorDarkGray,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorKellyGreen
  };
  
  // 9: Black on Red
  SCHEMES[9] = (ColorScheme){
    .clock = GColorBlack, .seconds = GColorBlack, .timezone = GColorBlack,
    .steps = GColorBlack, .calendar_week = GColorBlack, .clock_bg = GColorRed,
    .date = GColorBlack, .date_bg = GColorRed,
    .weather = GColorBlack, .weather_bg = GColorRed,
    .moon = GColorBlack, .moon_bg = GColorRed,
    .sun = GColorYellow, .connection = GColorBlack, .status_bg = GColorRed,
    .location = GColorBlack, .location_bg = GColorRed,
    .last_update = GColorBlack, .last_update_bg = GColorRed,
    .lines_bg = GColorBlack, .lines_minor = GColorLightGray,
    .battery_text = GColorBlack, .battery_bg = GColorRed,
    .battery_fill = GColorKellyGreen
  };
  
  // 10: Black on Green
  SCHEMES[10] = (ColorScheme){
    .clock = GColorBlack, .seconds = GColorBlack, .timezone = GColorBlack,
    .steps = GColorBlack, .calendar_week = GColorBlack, .clock_bg = GColorGreen,
    .date = GColorBlack, .date_bg = GColorGreen,
    .weather = GColorBlack, .weather_bg = GColorGreen,
    .moon = GColorBlack, .moon_bg = GColorGreen,
    .sun = GColorBlack, .connection = GColorBlack, .status_bg = GColorGreen,
    .location = GColorBlack, .location_bg = GColorGreen,
    .last_update = GColorBlack, .last_update_bg = GColorGreen,
    .lines_bg = GColorBlack, .lines_minor = GColorLightGray,
    .battery_text = GColorBlack, .battery_bg = GColorGreen,
    .battery_fill = GColorWhite
  };

  // 11: Black on Blue
  SCHEMES[11] = (ColorScheme){
    .clock = GColorBlack, .seconds = GColorBlack, .timezone = GColorBlack,
    .steps = GColorBlack, .calendar_week = GColorBlack, .clock_bg = GColorBlue,
    .date = GColorBlack, .date_bg = GColorBlue,
    .weather = GColorBlack, .weather_bg = GColorBlue,
    .moon = GColorBlack, .moon_bg = GColorBlue,
    .sun = GColorYellow, .connection = GColorBlack, .status_bg = GColorBlue,
    .location = GColorBlack, .location_bg = GColorBlue,
    .last_update = GColorBlack, .last_update_bg = GColorBlue,
    .lines_bg = GColorBlack, .lines_minor = GColorLightGray,
    .battery_text = GColorBlack, .battery_bg = GColorBlue,
    .battery_fill = GColorKellyGreen
  };

  // 12: Black on Yellow
  SCHEMES[12] = (ColorScheme){
    .clock = GColorBlack, .seconds = GColorBlack, .timezone = GColorBlack,
    .steps = GColorBlack, .calendar_week = GColorBlack, .clock_bg = GColorChromeYellow,
    .date = GColorBlack, .date_bg = GColorChromeYellow,
    .weather = GColorBlack, .weather_bg = GColorChromeYellow,
    .moon = GColorBlack, .moon_bg = GColorChromeYellow,
    .sun = GColorBlack, .connection = GColorBlack, .status_bg = GColorChromeYellow,
    .location = GColorBlack, .location_bg = GColorChromeYellow,
    .last_update = GColorBlack, .last_update_bg = GColorChromeYellow,
    .lines_bg = GColorBlack, .lines_minor = GColorLightGray,
    .battery_text = GColorBlack, .battery_bg = GColorChromeYellow,
    .battery_fill = GColorKellyGreen
  };
  
  // 13: White on Purple
  SCHEMES[13] = (ColorScheme){
    .clock = GColorWhite, .seconds = GColorWhite, .timezone = GColorWhite,
    .steps = GColorWhite, .calendar_week = GColorWhite, .clock_bg = GColorImperialPurple,
    .date = GColorWhite, .date_bg = GColorImperialPurple,
    .weather = GColorWhite, .weather_bg = GColorImperialPurple,
    .moon = GColorWhite, .moon_bg = GColorImperialPurple,
    .sun = GColorYellow, .connection = GColorWhite, .status_bg = GColorImperialPurple,
    .location = GColorWhite, .location_bg = GColorImperialPurple,
    .last_update = GColorWhite, .last_update_bg = GColorImperialPurple,
    .lines_bg = GColorWhite, .lines_minor = GColorDarkGray,
    .battery_text = GColorWhite, .battery_bg = GColorImperialPurple,
    .battery_fill = GColorKellyGreen
  };
  
  // 14: Blue with Colored BG
  SCHEMES[14] = (ColorScheme){
    .clock = GColorCyan, .seconds = GColorOrange, .timezone = GColorDarkGray,
    .steps = GColorWhite, .calendar_week = GColorTiffanyBlue, .clock_bg = GColorOxfordBlue,
    .date = GColorCeleste, .date_bg = GColorDukeBlue,
    .weather = GColorBrightGreen, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorYellow, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorChromeYellow, .location_bg = GColorDarkCandyAppleRed,
    .last_update = GColorRichBrilliantLavender, .last_update_bg = GColorJazzberryJam,
    .lines_bg = GColorLightGray, .lines_minor = GColorDarkGray,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorKellyGreen
  };
  
  // 15: White with Colored BG
  SCHEMES[15] = (ColorScheme){
    .clock = GColorWhite, .seconds = GColorVividCerulean, .timezone = GColorDarkGray,
    .steps = GColorWhite, .calendar_week = GColorTiffanyBlue, .clock_bg = GColorBlack,
    .date = GColorBlack, .date_bg = GColorLightGray,
    .weather = GColorBrightGreen, .weather_bg = GColorBlack,
    .moon = GColorWhite, .moon_bg = GColorBlack,
    .sun = GColorYellow, .connection = GColorVividCerulean, .status_bg = GColorBlack,
    .location = GColorWhite, .location_bg = GColorDarkGreen,
    .last_update = GColorWhite, .last_update_bg = GColorDarkGreen,
    .lines_bg = GColorLightGray, .lines_minor = GColorDarkGray,
    .battery_text = GColorWhite, .battery_bg = GColorBlack,
    .battery_fill = GColorKellyGreen
  };
}

const ColorScheme* colors_get_scheme(int index) {
#if defined(PBL_BW)
  if (index == 1) return &SCHEMES[1]; // Invert (Black on White)
  return &SCHEMES[0]; // Default (White on Black)
#else
  if (index < 0 || index > MAX_COLOR_PROFILES) return &SCHEMES[0];
  return &SCHEMES[index];
#endif
}

void colors_get_weather_icon_colors(int profile_index, int icon_code, bool is_night, GColor *fg, GColor *bg) {
#if defined(PBL_BW)
  if (profile_index == 1) { // Black on White
     *fg = GColorBlack; *bg = GColorWhite;
  } else {
     *fg = GColorWhite; *bg = GColorBlack;
  }
  return;
#endif

  *fg = GColorWhite;
  *bg = GColorBlack;

  if (profile_index <= 1) { // BW schemes on Color platforms
    *fg = (profile_index == 1) ? GColorBlack : GColorWhite;
    *bg = (profile_index == 1) ? GColorWhite : GColorBlack;
    return;
  }

  // Icon codes range from 33-106 - use lookup table
  // Format: {fg_hex, bg_hex} - 0x000000 means use default (white on black)
  static const uint32_t ICON_COLORS[][2] = {
    // 33-40: Cloud/sun variations
    {0xFFFFFF, 0x00AAFF}, {0xFFFF55, 0x00AAFF}, {0x55AAFF, 0x000055}, {0x55FFFF, 0x555555},
    {0xFFAA00, 0x0055AA}, {0x0055AA, 0x000055}, {0x55FFFF, 0x555555}, {0xFFAA00, 0x0055AA},
    // 41-48
    {0x0055AA, 0x000055}, {0x55AAFF, 0x555555}, {0xFF5500, 0x0055AA}, {0x0055AA, 0x000055},
    {0x55AAAA, 0x555555}, {0xFFAA55, 0x0055AA}, {0x0055AA, 0x000055}, {0xFFFFFF, 0x00FFFF},
    // 49-56
    {0xFFFF55, 0x0055FF}, {0x55AAAA, 0x000055}, {0xFF5555, 0x0055AA}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    // 57-64
    {0xAAFFFF, 0x555555}, {0xFFFF00, 0x0055AA}, {0x00FFFF, 0x000055}, {0xAAAAAA, 0xFFFFFF},
    {0xFFFF55, 0x0055AA}, {0x55AAAA, 0xFFFFFF}, {0xAAAAAA, 0xFFFFFF}, {0xFFAA00, 0xFFFFFF},
    // 65-72
    {0x55AAAA, 0xFFFFFF}, {0xAAFFFF, 0x0055AA}, {0, 0}, {0, 0}, {0, 0}, {0xFF0000, 0x000000},
    {0xFF5500, 0x000000}, {0x0055AA, 0x000000},
    // 73-80
    {0xFFFF00, 0x0055FF}, {0xFF5500, 0x0055AA}, {0xFF5500, 0x0055AA}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    // 81-88
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0xFF5500, 0x555555},
    // 89-96
    {0, 0}, {0xAAFFFF, 0x000055}, {0, 0}, {0, 0}, {0xFF0000, 0xFFFF00}, {0, 0}, {0, 0}, {0, 0},
    // 97-104
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    // 105-106
    {0, 0}, {0, 0}
  };

  if (icon_code >= 33 && icon_code <= 106) {
    int idx = icon_code - 33;
    uint32_t fg_hex = ICON_COLORS[idx][0];
    uint32_t bg_hex = ICON_COLORS[idx][1];
    if (fg_hex != 0 || bg_hex != 0) {
      *fg = GColorFromHEX(fg_hex);
      *bg = GColorFromHEX(bg_hex);
    }
  }
}
