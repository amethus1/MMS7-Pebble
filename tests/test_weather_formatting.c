#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../src/c/modules/weather_format.h"

static int s_failures = 0;

static void assert_equals(const char *name, const char *actual, const char *expected) {
    if (strcmp(actual, expected) == 0) {
        return;
    }

    s_failures++;
    printf("FAIL %s: expected '%s' got '%s'\n", name, expected, actual);
}

static void assert_int_equals(const char *name, int actual, int expected) {
    if (actual == expected) {
        return;
    }

    s_failures++;
    printf("FAIL %s: expected %d got %d\n", name, expected, actual);
}

int main(void) {
    char buffer[32];

    weather_format_wind(buffer, sizeof(buffer), 10, 0);
    assert_equals("wind kmh", buffer, "10 km/h");

    weather_format_wind(buffer, sizeof(buffer), 10, 1);
    assert_equals("wind mph", buffer, "6 mph");

    weather_format_wind(buffer, sizeof(buffer), 10, 2);
    assert_equals("wind ms", buffer, "3 m/s");

    weather_format_pressure(buffer, sizeof(buffer), 1013, 0);
    assert_equals("pressure hpa", buffer, "1013 hPa");

    weather_format_pressure(buffer, sizeof(buffer), 1013, 1);
    assert_equals("pressure mmhg", buffer, "760 mmHg");

    weather_format_pressure(buffer, sizeof(buffer), 1013, 2);
    assert_equals("pressure inhg", buffer, "29.91 inHg");

    weather_format_hi_lo(buffer, sizeof(buffer), 20, 10, false);
    assert_equals("hilo celsius", buffer, "20\xc2\xb0/10\xc2\xb0");

    weather_format_hi_lo(buffer, sizeof(buffer), 20, 10, true);
    assert_equals("hilo fahrenheit", buffer, "68\xc2\xb0/50\xc2\xb0");

    weather_format_hi_lo(buffer, sizeof(buffer), -999, 10, false);
    assert_equals("hilo sentinel", buffer, "--/--");

    assert_int_equals("c_to_f 0", c_to_f_rounded(0), 32);
    assert_int_equals("c_to_f 20", c_to_f_rounded(20), 68);
    assert_int_equals("c_to_f -1", c_to_f_rounded(-1), 30);
    assert_int_equals("c_to_f -10", c_to_f_rounded(-10), 14);
    assert_int_equals("c_to_f -40", c_to_f_rounded(-40), -40);

    if (s_failures == 0) {
        printf("ALL TESTS PASSED\n");
        return 0;
    }

    printf("%d TEST(S) FAILED\n", s_failures);
    return 1;
}
