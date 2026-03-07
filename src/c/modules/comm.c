#include "comm.h"
#include "weather.h"
#include "../state/settings.h"
#include "../state/state.h"
#include "../keys.h"

#include "../state/events.h"

static void (*s_callback)(void) = NULL;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    bool weather_changed = weather_handle_app_message(iterator);
    bool settings_changed = settings_handle_app_message(iterator);
    
    // Clear fetch error on successful receive
    if (weather_changed) {
        AppState* state = state_get_ptr();
        state->weather.fetch_error = false;
        events_set_flag(EVENT_WEATHER_DATA);
    }
    if (settings_changed) {
        events_set_flag(EVENT_SETTINGS_CHANGED);
    }
    
    if (s_callback) {
        s_callback();
    }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
#ifdef DEBUG
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
#endif
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
#ifdef DEBUG
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
#endif
    // Set weather fetch error state so UI can indicate
    AppState* state = state_get_ptr();
    state->weather.fetch_error = true;
    events_set_flag(EVENT_WEATHER_DATA);
    if (s_callback) {
        s_callback();
    }
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
#ifdef DEBUG
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
#endif
}

void comm_init(void (*callback)(void)) {
    s_callback = callback;
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void comm_deinit() {
    app_message_deregister_callbacks();
}

void comm_request_weather() {
    DictionaryIterator *iter;
    AppMessageResult result = app_message_outbox_begin(&iter);

    if (result == APP_MSG_OK && iter) {
        int dummy = 0;
        dict_write_int(iter, KEY_FETCH_TRIGGER, &dummy, sizeof(int), true);
        app_message_outbox_send();
        return;
    }

    AppState* state = state_get_ptr();
    state->weather.fetch_error = true;
    events_set_flag(EVENT_WEATHER_DATA);
    if (s_callback) {
        s_callback();
    }
}
