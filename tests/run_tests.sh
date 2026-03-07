#!/usr/bin/env bash
set -euo pipefail

echo "== Key contract check =="
node tests/check_message_keys.js

echo "== JS unit tests =="
node tests/test_settings_js.js
node tests/test_weather_api_js.js

echo "== C unit tests =="
cc -std=c99 -Wall -Wextra -Werror \
  -I src/c \
  tests/test_weather_formatting.c \
  src/c/modules/weather_format.c \
  -o /tmp/test_weather_formatting
/tmp/test_weather_formatting

cc -std=c99 -Wall -Wextra -Werror \
  -I tests -I src/c \
  tests/test_weather.c \
  tests/mock_pebble.c \
  src/c/modules/weather.c \
  src/c/state/state.c \
  src/c/state/events.c \
  -o /tmp/test_weather
/tmp/test_weather

cc -std=c99 -Wall -Wextra -Werror \
  -I tests -I src/c \
  tests/test_battery.c \
  tests/mock_pebble.c \
  src/c/modules/battery.c \
  src/c/state/state.c \
  src/c/state/events.c \
  -o /tmp/test_battery
/tmp/test_battery

echo "== Pebble build =="
build_log="$(mktemp)"
pebble clean >/dev/null 2>&1
pebble build 2>&1 | tee "$build_log"

supported_heap="$(sed -nE 's/.*Free RAM available \(heap\):[[:space:]]*([0-9]+) bytes.*/\1/p' "$build_log" \
  | sort -n \
  | head -n1)"

history_file="tests/supported_heap_history.csv"
if [[ ! -f "$history_file" ]]; then
  echo "timestamp,free_heap_bytes" > "$history_file"
fi

if [[ -n "${supported_heap}" ]]; then
  echo "$(date -u +%Y-%m-%dT%H:%M:%SZ),${supported_heap}" >> "$history_file"
  echo "Minimum supported-platform heap: ${supported_heap} bytes (recorded in $history_file)"
else
  echo "Warning: could not parse supported-platform heap usage from build output" >&2
fi

rm -f "$build_log"

artifact="$(find build -maxdepth 1 -name '*.pbw' | head -n1)"
if [[ -z "${artifact}" ]]; then
  echo "Error: no .pbw artifact found in build/" >&2
  exit 1
fi

cp "$artifact" build/MSS7.pbw
echo "Build artifact: build/MSS7.pbw"
