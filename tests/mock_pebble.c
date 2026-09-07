#include "mock_pebble.h"

// Simple in-memory storage for mocks
static int s_int_store[100];
static char s_str_store[100][64];

void persist_write_int(uint32_t key, int value) {
    if (key < 100) s_int_store[key] = value;
}

int persist_read_int(uint32_t key) {
    if (key < 100) return s_int_store[key];
    return 0;
}

void persist_write_string(uint32_t key, const char* value) {
    if (key < 100) strncpy(s_str_store[key], value, 64);
}

int persist_read_string(uint32_t key, char* buffer, size_t buffer_size) {
    if (key < 100) {
        strncpy(buffer, s_str_store[key], buffer_size);
        return strlen(s_str_store[key]);
    }
    return 0;
}

bool persist_exists(uint32_t key) {
    (void)key;
    return true; // Simplification
}

// Battery service stubs
BatteryChargeState battery_state_service_peek(void) {
    return (BatteryChargeState){ .charge_percent = 50, .is_charging = false, .is_plugged = false };
}

void battery_state_service_subscribe(BatteryStateHandler handler) {
    (void)handler;
}

void battery_state_service_unsubscribe(void) {}

// Dictionary iteration: tests hand in an array of tuples via mock_dict_set()
static Tuple* s_mock_tuples = NULL;
static int s_mock_tuple_count = 0;
static int s_mock_tuple_index = 0;

void mock_dict_set(Tuple* tuples, int count) {
    s_mock_tuples = tuples;
    s_mock_tuple_count = count;
    s_mock_tuple_index = 0;
}

Tuple* dict_read_first(DictionaryIterator* iter) {
    (void)iter;
    s_mock_tuple_index = 0;
    return s_mock_tuple_count > 0 ? &s_mock_tuples[0] : NULL;
}

Tuple* dict_read_next(DictionaryIterator* iter) {
    (void)iter;
    s_mock_tuple_index++;
    return s_mock_tuple_index < s_mock_tuple_count ? &s_mock_tuples[s_mock_tuple_index] : NULL;
}

// Comm stubs
void comm_request_weather(void) {}
