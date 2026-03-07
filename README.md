# MSS Refreshed

A highly configurable, information-rich Seven Segment watchface for Pebble Time, Pebble Time Round, and Pebble 2. Legacy `aplite` devices are no longer targeted.

**Original Creator:** [SetPebble](https://apps.rebble.io/en_US/application/5569c2084bebd0b90400000e) (Attribution)
**Refined & Updated by:** Henry Miller

![Watchface Preview](https://assets.rebble.io/7d/5569c2084bebd0b90400000e/screenshot.png?80x80)

## Overview

This project is a modernize and refactored version of the classic MSS watchface. It retains the distinct seven-segment aesthetic while significantly improving code quality, configuration, and feature set.

## New Features & Improvements

### 🔧 Modern Configuration (Clay)
- **Built-in Settings:** No longer relies on external websites. All settings are built directly into the Pebble app using `pebble-clay`.
- **Instant Updates:** Settings apply immediately upon save.

### 📅 Fiscal Week Support
- **Customizable Start:** Define your fiscal year start month and day.
- **Display Options:** Choose between ISO Calendar Week (`CW`) or Fiscal Week (`WK`).
- **Integration:** Fiscal week number can be appended to the date line or shown in the weather info area.

### 🎨 Customization
- **Visibility Toggles:** Option to hide the Calendar Week and Bluetooth connection label for a cleaner look.
- **Health Info:** Toggle between showing Timezone name, Steps, Sleep, or Auto-switch (Steps day / Sleep night) in the bottom-left corner.
- **Timezone:** Flexible display formats (UTC, TZ Name, AM/PM + TZ).
- **Unit Support:** Speed (km/h, mph, m/s) and Pressure (hPa, mmHg, inHg).

### 🛠 Technical Refactoring
- **Unified Codebase:** Merged split source files into a unified, maintainable C structure.
- **Optimization:** Significant RAM reduction (~5KB saved) and storage optimization (-45% size) by removing unused assets.
- **Stability:** Robust settings parsing prevents invalid configurations (e.g., "Color Cycling" bugs).
- **Fixes:** Resolved layout issues, font memory leaks, and buffer overflows.

## Build Instructions

This project uses the standard Pebble SDK.

```bash
# Build the project
pebble build

# Install to phone/watch
pebble install
```

## Credits

- Based on the original work by **SetPebble**.
- Refactoring and new features by **Henry Miller**.
- Weather data provided by Open-Meteo.
