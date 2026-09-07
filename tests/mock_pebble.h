#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Mock Pebble types
typedef uint32_t GColor;
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
