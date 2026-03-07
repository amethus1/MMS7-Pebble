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

// Dictionary iteration stubs
Tuple* dict_read_first(DictionaryIterator* iter) {
    (void)iter;
    return NULL;
}

Tuple* dict_read_next(DictionaryIterator* iter) {
    (void)iter;
    return NULL;
}

// Comm stubs
void comm_request_weather(void) {}
