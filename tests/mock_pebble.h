#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Mock Pebble types
// Colours as the Pebble SDK defines them: one byte, two bits each for alpha,
// red, green and blue. Only the named colours the watchface uses are listed.
typedef union {
    uint8_t argb;
} GColor8;
typedef GColor8 GColor;
#define GColorFromHEX(v) ((GColor){ .argb = (uint8_t)(0xC0 | \
    ((((v) >> 16) & 0xff) >> 6) << 4 | ((((v) >> 8) & 0xff) >> 6) << 2 | (((v) & 0xff) >> 6)) })
static inline bool gcolor_equal(GColor a, GColor b) { return a.argb == b.argb; }
#define GColorBlack                  ((GColor){ .argb = 0xC0 })
#define GColorBlue                   ((GColor){ .argb = 0xC3 })
#define GColorBrightGreen            ((GColor){ .argb = 0xDC })
#define GColorBulgarianRose          ((GColor){ .argb = 0xD0 })
#define GColorCeleste                ((GColor){ .argb = 0xEF })
#define GColorChromeYellow           ((GColor){ .argb = 0xF8 })
#define GColorCyan                   ((GColor){ .argb = 0xCF })
#define GColorDarkCandyAppleRed      ((GColor){ .argb = 0xE0 })
#define GColorDarkGray               ((GColor){ .argb = 0xD5 })
#define GColorDarkGreen              ((GColor){ .argb = 0xC4 })
#define GColorDukeBlue               ((GColor){ .argb = 0xC2 })
#define GColorElectricBlue           ((GColor){ .argb = 0xDF })
#define GColorGreen                  ((GColor){ .argb = 0xCC })
#define GColorImperialPurple         ((GColor){ .argb = 0xD1 })
#define GColorJazzberryJam           ((GColor){ .argb = 0xE1 })
#define GColorKellyGreen             ((GColor){ .argb = 0xD8 })
#define GColorLightGray              ((GColor){ .argb = 0xEA })
#define GColorMagenta                ((GColor){ .argb = 0xF3 })
#define GColorMediumSpringGreen      ((GColor){ .argb = 0xCE })
#define GColorMelon                  ((GColor){ .argb = 0xFA })
#define GColorOrange                 ((GColor){ .argb = 0xF4 })
#define GColorOxfordBlue             ((GColor){ .argb = 0xC1 })
#define GColorPurpureus              ((GColor){ .argb = 0xE6 })
#define GColorRed                    ((GColor){ .argb = 0xF0 })
#define GColorRichBrilliantLavender  ((GColor){ .argb = 0xFB })
#define GColorScreaminGreen          ((GColor){ .argb = 0xDD })
#define GColorShockingPink           ((GColor){ .argb = 0xF7 })
#define GColorSunsetOrange           ((GColor){ .argb = 0xF5 })
#define GColorTiffanyBlue            ((GColor){ .argb = 0xCA })
#define GColorVividCerulean          ((GColor){ .argb = 0xCB })
#define GColorWhite                  ((GColor){ .argb = 0xFF })
#define GColorYellow                 ((GColor){ .argb = 0xFC })
typedef void* GBitmap;
typedef void* Layer;
typedef void* Window;
typedef void* TextLayer;
typedef void* DictionaryIterator;
typedef void* AppMessageResult;

// Tuple type tags matching Pebble SDK (AppMessage)
typedef enum {
    TUPLE_BYTE_ARRAY = 0,
    TUPLE_CSTRING = 1,
    TUPLE_UINT = 2,
    TUPLE_INT = 3,
} TupleType;

// Tuple value union matching Pebble SDK
typedef union {
    char* cstring;
    int32_t int32;
    uint32_t uint32;
} TupleValue;

typedef struct {
    uint32_t key;
    TupleType type;
    uint16_t length;   // For TUPLE_CSTRING this includes the terminating NUL
    TupleValue* value;
} Tuple;

typedef struct {
    uint8_t charge_percent;
    bool is_charging;
    bool is_plugged;
} BatteryChargeState;

typedef void (*BatteryStateHandler)(BatteryChargeState charge_state);

#define APP_MSG_OK 0
#define APP_LOG_LEVEL_ERROR 1
#define APP_LOG_LEVEL_INFO 6
#define APP_LOG_LEVEL_DEBUG 7
#define APP_LOG_LEVEL_WARNING 4
#define APP_LOG(level, ...) do { if (0) { (void)(level); printf(__VA_ARGS__); } } while (0)

void persist_write_int(uint32_t key, int value);
int persist_read_int(uint32_t key);
void persist_write_string(uint32_t key, const char* value);
int persist_read_string(uint32_t key, char* buffer, size_t buffer_size);
bool persist_exists(uint32_t key);

// Dictionary iteration stubs
Tuple* dict_read_first(DictionaryIterator* iter);
Tuple* dict_read_next(DictionaryIterator* iter);

// Battery service stubs
BatteryChargeState battery_state_service_peek(void);
void battery_state_service_subscribe(BatteryStateHandler handler);
void battery_state_service_unsubscribe(void);

// Comm stubs
void comm_request_weather(void);

time_t time(time_t* tloc);

// Simple test assertion helper
#define assert_true(expr) do { if(!(expr)) { printf("FAIL: %s (%s:%d)\n", #expr, __FILE__, __LINE__); return 1; } } while(0)
#define assert_false(expr) do { if(expr) { printf("FAIL: !(%s) (%s:%d)\n", #expr, __FILE__, __LINE__); return 1; } } while(0)

#define _(str) str

// Feed tuples to dict_read_first/next (mock_pebble.c)
void mock_dict_set(Tuple* tuples, int count);
