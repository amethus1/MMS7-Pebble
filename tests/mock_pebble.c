#include "mock_pebble.h"

// In-memory persistent storage. Keys go up to 207 (see keys.h); a key only
// "exists" once something has been written to it, like the real store.
#define MOCK_PERSIST_KEYS 256
static int s_int_store[MOCK_PERSIST_KEYS];
static char s_str_store[MOCK_PERSIST_KEYS][64];
static bool s_exists[MOCK_PERSIST_KEYS];

void persist_write_int(uint32_t key, int value) {
    if (key < MOCK_PERSIST_KEYS) { s_int_store[key] = value; s_exists[key] = true; }
}

int persist_read_int(uint32_t key) {
    if (key < MOCK_PERSIST_KEYS) return s_int_store[key];
    return 0;
}

void persist_write_string(uint32_t key, const char* value) {
    if (key < MOCK_PERSIST_KEYS) {
        strncpy(s_str_store[key], value, 63);
        s_str_store[key][63] = '\0';
        s_exists[key] = true;
    }
}

int persist_read_string(uint32_t key, char* buffer, size_t buffer_size) {
    if (key < MOCK_PERSIST_KEYS && buffer_size > 0) {
        strncpy(buffer, s_str_store[key], buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
        return strlen(s_str_store[key]);
    }
    return 0;
}

bool persist_exists(uint32_t key) {
    return key < MOCK_PERSIST_KEYS && s_exists[key];
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
