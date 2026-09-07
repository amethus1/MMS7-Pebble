# Rebble App Store submission — MSS Refreshed

Everything in this folder is ready to paste/upload into the
[Rebble Developer Portal](https://dev-portal.rebble.io/). Log in with your
Rebble account (the same one the Pebble app uses), then **Create a new app**.

The original "Multifunctional 7 Segment" (MSS7) by FG is already in the store
under UUID `03adc57a-…` at version 16.2.0. This project uses its own UUID
(`ec182d88-f5ea-4559-8640-86ffd1f74f66`), so it is submitted as a **new,
separate listing** — the portal rejects duplicate UUIDs, and this one is free.

---

## 1. Basic information

| Field | Value |
|---|---|
| **Title** | `MSS Refreshed` |
| **Type** | Watchface |
| **Category** | Faces (automatic for watchfaces) |
| **Website URL** | `https://github.com/amethus1/MMS7-Pebble` (optional) |
| **Source code URL** | `https://github.com/amethus1/MMS7-Pebble` — required in practice: the project is GPL-3.0 (inherited from MSS7), so the source must be public. |
| **Support email** | Defaults to your Rebble account email if left blank. |
| **Developer name** | `Henry Miller` (from `package.json` → `author`) |

## 2. Description (max 1600 characters — this one is 1258)

```
A modern rebuild of the classic Multifunctional 7 Segment watchface: the same
information-dense seven-segment look, refreshed for every current Pebble —
Pebble Time, Time Steel, Time Round, Pebble 2, Pebble Time 2 and Pebble Round 2,
each with its own hand-tuned layout.

At a glance:
• Big seven-segment time with optional seconds (always on, or shake to show)
• Date with ISO calendar week or your own fiscal week
• Current weather, hi/lo, wind, humidity and pressure from Open-Meteo — no API key
• Sunrise and sunset, moon phase at night, timezone or step count
• Battery gauge with time since last charge, Bluetooth status
• 15 colour profiles, with temperature-coloured readouts on colour watches

Settings live inside the Pebble app (no external website): units for
temperature, wind and pressure, date format, weather refresh interval, what to
show in each info slot, hourly vibe, backlight behaviour and more.

Weather uses your phone's location, or a city you type in.

Free and open source (GPL-3.0). This is a fan update of Multifunctional 7
Segment by FG — all credit for the design goes to the original. I'm not a
programmer; the port to the new watches was done with AI assistance, and the
source is published so anyone can check or improve it.
```

## 3. Release notes

For the current release (17.3.0) the notes are:

```
MSS Refreshed 17.3.0

• Settings page reorganised: options only appear when relevant, one clear
  week-number choice, Classic and Clean presets with previews, reset button,
  and a short guide to what the small things mean
• Weather without GPS uses the phone's last position and marks it with ~;
  a typed city is only used if you entered one - no more silent Berlin
• "!" before the weather age when the last fetch failed, "--" instead of
  fake zeros before the first weather, and "--" for steps when there is no
  health data - all readable on black-and-white watches
• New options: hide the time under the battery, drop the leading zero
  (8:30), moon at night unless it is raining or snowing, and last night's
  sleep shown until a time you choose
• Blinking backlight really blinks once a second now
• 17.2.0: presets no longer touch your city, units or alerts; sleep total
  comes from the night's actual sleep sessions
• 17.3.0: refined Pebble Time 2 and Round 2 faces - even clock margins, a
  quieter grid, a drawn Bluetooth symbol, larger sunrise/sunset times, the
  week label in a fixed slot, and an option on Time 2 to centre the
  timezone / steps under the clock
```

### First release (17.0) notes, for reference

```
MSS Refreshed 17.0 — first release.

• Native layouts for Pebble Time 2 and Pebble Round 2, and a redesigned
  Pebble Time Round face that fits the circle properly
• Weather from Open-Meteo (no API key, no account)
• Settings built into the Pebble app via Clay
• Fiscal week support, timezone / step display, hide-able CW and Bluetooth labels
• Battery gauge drawn like the original: percentage inside the outline, inverted over the charge bar
• Smaller and faster than MSS7 16.x
```

## 4. Version

The `.pbw` reports **17.3.0** (from `package.json` → `version`). Every later
upload to this listing must carry a higher version — bump `"version"` in
`package.json` and run `pebble build` before each new release.

## 5. Assets in this folder

| File | Portal field | Notes |
|---|---|---|
| `MSS-Refreshed.pbw` | Release → upload .pbw | Copy of `build/pebble-mss-master.pbw`; SDK 4.33.1 — requires watch firmware ≥ 4.32 (see note below) |
| `banner-720x320.png` | Marketing banner | Optional for watchfaces, but it makes the listing look finished |
| `icon-144x144.png`, `icon-48x48.png` | Large / small icon | The portal marks these "apps only"; included in case it asks |
| `screenshots/basalt/*.png` | Asset collection → Basalt | 144×168, Pebble Time / Time Steel |
| `screenshots/chalk/*.png` | Asset collection → Chalk | 180×180, Pebble Time Round |
| `screenshots/diorite/*.png` | Asset collection → Diorite | 144×168, Pebble 2 (black & white) |
| `screenshots/emery/*.png` | Asset collection → Emery | 200×228, Pebble Time 2 |
| `screenshots/gabbro/*.png` | Asset collection → Gabbro | 260×260, Pebble Round 2 |

Screenshots are the raw framebuffer at native resolution, unframed (the portal
requires unframed images and allows up to 5 per platform). Each platform has
three, captured in the emulator with sample data (Chicago, 64 °F, sunrise
06:41 / sunset 19:27): `1-day` — default colours; `2-night-steps` — moon phase
with the step counter; `3-inverted-seconds` — the "Black on White with Colors"
profile, Celsius, seconds showing. To regenerate them after a UI change, run
`pebble build` then `pebble screenshot --emulator <platform> --no-correction`.

Upload order in the portal: create the app → add a release (upload the .pbw)
→ **Manage Asset Collections** → *Create* for each of the five platforms →
paste the description, add the screenshots (and banner) → *Create Asset
Collection* → **Publish** (or *Publish Privately* to test the listing first).

## 6. Things to decide before you click Publish

1. **Firmware requirement.** This `.pbw` was built with SDK 4.33.1, which the
   SDK changelog says needs watch firmware 4.32+. Anyone who hasn't updated
   their watch through the Pebble app can't install it. To ship the widest
   compatible build instead:
   ```bash
   pebble sdk activate 4.9.127 && pebble clean && pebble build
   ```
   (Gabbro support is unaffected — 4.9.127 already targets it.)
2. **Name on the watch.** `displayName` in `package.json` is now
   `MSS Refreshed`; that is what appears in the watch's face list.
3. **Credits.** The description and README credit FG's Multifunctional 7
   Segment as the original, per your decision.
4. **Source URL.** `https://github.com/amethus1/MMS7-Pebble` — already public, paste it in.
