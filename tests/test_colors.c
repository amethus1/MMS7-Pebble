#include "mock_pebble.h"
#include "../src/c/modules/colors.h"

// The custom colour scheme: each of the user's colours must reach the part of
// the face it is labelled for, and nothing may vanish into the background.

static int s_failures = 0;

static void check_color(const char *name, GColor actual, GColor expected) {
    if (gcolor_equal(actual, expected)) return;
    s_failures++;
    printf("FAIL %s: expected 0x%02X got 0x%02X\n", name, expected.argb, actual.argb);
}

static void check(const char *name, bool ok) {
    if (ok) return;
    s_failures++;
    printf("FAIL %s\n", name);
}

int main(void) {
    colors_init();

    // A different colour in every slot, so a crossed wire shows up
    int rgb[CUSTOM_COLOR_COUNT];
    rgb[CUSTOM_COLOR_BACKGROUND]  = 0x000055;   // Oxford blue
    rgb[CUSTOM_COLOR_CLOCK]       = 0x00FF00;   // Green
    rgb[CUSTOM_COLOR_DATE]        = 0xFFAA00;   // Chrome yellow
    rgb[CUSTOM_COLOR_WEATHER]     = 0x00FFFF;   // Cyan
    rgb[CUSTOM_COLOR_ICON]        = 0xFF00FF;   // Magenta
    rgb[CUSTOM_COLOR_HEADER_TEXT] = 0x000000;   // Black
    rgb[CUSTOM_COLOR_HEADER_BG]   = 0xAAAAAA;   // Light gray
    rgb[CUSTOM_COLOR_SUN]         = 0xFF5500;   // Orange
    rgb[CUSTOM_COLOR_EXTRA]       = 0x00AAAA;   // Tiffany blue
    rgb[CUSTOM_COLOR_BLUETOOTH]   = 0xFF0000;   // Red
    rgb[CUSTOM_COLOR_BATTERY]     = 0xFFFFFF;   // White
    rgb[CUSTOM_COLOR_LINES]       = 0x555555;   // Dark gray
    colors_set_custom(rgb, true);

    const ColorScheme *c = colors_get_scheme(COLOR_PROFILE_CUSTOM);
    check_color("background", c->clock_bg, GColorOxfordBlue);
    check_color("clock", c->clock, GColorGreen);
    check_color("date", c->date, GColorChromeYellow);
    check_color("weather text", c->weather, GColorCyan);
    check_color("moon", c->moon, GColorMagenta);
    check_color("location text", c->location, GColorBlack);
    check_color("weather age text", c->last_update, GColorBlack);
    check_color("location background", c->location_bg, GColorLightGray);
    check_color("weather age background", c->last_update_bg, GColorLightGray);
    check_color("sun", c->sun, GColorOrange);
    check_color("steps", c->steps, GColorTiffanyBlue);
    check_color("timezone", c->timezone, GColorTiffanyBlue);
    check_color("bluetooth", c->connection, GColorRed);
    check_color("battery outline and text", c->battery_text, GColorWhite);
    check_color("battery text over the bar", c->battery_bg, GColorOxfordBlue);
    check_color("grid lines", c->lines_bg, GColorDarkGray);
    check_color("minor grid lines", c->lines_minor, GColorDarkGray);

    // The built-in schemes are untouched, and bad indexes fall back to the first
    check_color("scheme 2 sun", colors_get_scheme(2)->sun, GColorYellow);
    check_color("scheme 4 clock", colors_get_scheme(4)->clock, GColorRed);
    check("index past custom falls back", colors_get_scheme(COLOR_PROFILE_CUSTOM + 1) == colors_get_scheme(0));
    check("negative index falls back", colors_get_scheme(-1) == colors_get_scheme(0));

    // The defaults look like "White on black, with colours"
    static const int defaults[CUSTOM_COLOR_COUNT] = CUSTOM_COLOR_DEFAULTS;
    colors_set_custom(defaults, true);
    check_color("default background", c->clock_bg, colors_get_scheme(2)->clock_bg);
    check_color("default clock", c->clock, colors_get_scheme(2)->clock);
    check_color("default sun", c->sun, colors_get_scheme(2)->sun);
    check_color("default bluetooth", c->connection, colors_get_scheme(2)->connection);
    check_color("default grid", c->lines_bg, colors_get_scheme(2)->lines_bg);

    // Weather icon. 'I' is the sun (yellow in the built-in colour schemes);
    // 'j' has no colour of its own.
    GColor fg, bg;
    colors_get_weather_icon_colors(2, 'I', false, &fg, &bg);
    check_color("built-in scheme: sun icon is yellow", fg, GColorYellow);

    colors_set_custom(rgb, true);
    colors_get_weather_icon_colors(COLOR_PROFILE_CUSTOM, 'I', false, &fg, &bg);
    check_color("custom, by weather: sun icon is yellow", fg, GColorYellow);
    colors_get_weather_icon_colors(COLOR_PROFILE_CUSTOM, 'j', false, &fg, &bg);
    check_color("custom, by weather: an icon with no colour takes the icon colour", fg, GColorMagenta);

    colors_set_custom(rgb, false);
    colors_get_weather_icon_colors(COLOR_PROFILE_CUSTOM, 'I', false, &fg, &bg);
    check_color("custom, single colour: sun icon takes the icon colour", fg, GColorMagenta);

    // A condition colour that matches the background would be invisible
    rgb[CUSTOM_COLOR_BACKGROUND] = 0xFFFF00;
    colors_set_custom(rgb, true);
    colors_get_weather_icon_colors(COLOR_PROFILE_CUSTOM, 'I', false, &fg, &bg);
    check_color("custom: yellow sun on a yellow background takes the icon colour", fg, GColorMagenta);

    if (s_failures == 0) { printf("ALL TESTS PASSED\n"); return 0; }
    return 1;
}
