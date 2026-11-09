#include "mock_pebble.h"
#include "../src/c/mooncalc.h"

// Every moon phase must map to a glyph that exists in moon_phases.ttf.
// A character the font lacks is drawn by the watch's fallback font instead,
// which is how new moon used to show a stray "1".

// The printable ASCII characters in resources/fonts/moon_phases.ttf
static const char FONT_CHARS[] = " *+0=@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static int s_failures = 0;

static bool font_has(int c) {
    for (const char *p = FONT_CHARS; *p; p++) {
        if (*p == c) return true;
    }
    return false;
}

int main(void) {
    for (int phase = 0; phase <= 27; phase++) {
        int c = moonphase_char_number(phase);
        if (!font_has(c)) {
            s_failures++;
            printf("FAIL phase %d maps to '%c' (%d), which is not in the moon font\n", phase, c, c);
        }
    }
    if (moonphase_char_number(0) != '@') {
        s_failures++;
        printf("FAIL new moon should be '@', got '%c'\n", moonphase_char_number(0));
    }
    if (moonphase_char_number(14) != '0') {
        s_failures++;
        printf("FAIL full moon should be '0', got '%c'\n", moonphase_char_number(14));
    }

    if (s_failures == 0) { printf("ALL TESTS PASSED\n"); return 0; }
    return 1;
}
