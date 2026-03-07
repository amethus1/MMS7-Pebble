#include "events.h"

static EventFlag s_flags = EVENT_NONE;

void events_init() {
  s_flags = EVENT_NONE;
}

void events_set_flag(EventFlag flag) {
  s_flags |= flag;
}

EventFlag events_get_and_clear_flags() {
  EventFlag flags = s_flags;
  s_flags = EVENT_NONE;
  return flags;
}
