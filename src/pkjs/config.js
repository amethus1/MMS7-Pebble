// MSS Refreshed - Clay configuration
//
// Layout of the page: Appearance, Time & Date, Weather, Extra information,
// Alerts & Power, About. Items that only make sense in combination with another
// setting (fiscal week, timezone format, ...) are shown and hidden by
// custom-clay.js; items that only make sense on some watches use Clay's
// "capabilities" filter so they are not on the page at all elsewhere.
//
// Every messageKey and every option value here is unchanged from the previous
// layout, so saved settings carry over.

var WEATHER_DETAIL_OPTIONS = [
    { "label": "Empty", "value": 0 },
    { "label": "Conditions (e.g. Partly cloudy)", "value": 1 },
    { "label": "Wind speed", "value": 2 },
    { "label": "Humidity", "value": 3 },
    { "label": "Pressure", "value": 4 },
    { "label": "High / low temperature", "value": 5 }
];

var COLOR_SCHEME_OPTIONS = [
    { "label": "White on black", "value": 0 },
    { "label": "Black on white", "value": 1 },
    { "label": "White on black, with colours", "value": 2 },
    { "label": "Black on white, with colours", "value": 3 },
    { "label": "Red on black", "value": 4 },
    { "label": "Green on black", "value": 5 },
    { "label": "Blue on black", "value": 6 },
    { "label": "Colourful, blue clock", "value": 14 },
    { "label": "Colourful, white clock", "value": 15 }
];

// The weather-detail layout: a crop of the real face with the four slots
// marked, so the numbered choices below map onto the watch without guesswork.
var LAYOUT_GUIDE_IMG = 'data:image/png;base64,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';
var LAYOUT_GUIDE_HTML =
    '<div style="font-size:0.9em;line-height:1.4">' +
    'Three spots in the weather area show extra values you choose:' +
    '<img src="' + LAYOUT_GUIDE_IMG + '" alt="" style="display:block;width:100%;max-width:360px;margin:8px auto 6px">' +
    '<b>1</b> and <b>2</b> are the two lines under the temperature. ' +
    '<b>3</b> is the line under the weather icon; it holds two values side by side, ' +
    'shown as <i>first / second</i>.' +
    '</div>';

// Preset previews: the face as it looks with each preset (Pebble Time shown)
var PRESET_PREVIEWS_HTML =
    '<div style="display:flex;justify-content:space-around;text-align:center;font-size:0.85em">' +
    '<div><img src="data:image/png;base64,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" alt="Classic" style="width:120px;image-rendering:pixelated;display:block;margin:0 auto 4px">Classic</div>' +
    '<div><img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAJAAAACoCAMAAAA4sZdKAAAAP1BMVEUAAAD///+qqqpVqgBV/1X//1X//wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACMGwoNAAAAEHRSTlP/////////AAAAAAAAAAAAwAi+qgAABJlJREFUeNrtm4G6qyAIgGGl7//I99y1DNAUDa2do1+rrW/Tf4BIigBDywseVibQnwTCn/K+fD4ZATVXhLgR5SrQVx6A8H20AYVX4Npklr7UAuFecw1QkNTOhqeXeqCNqAVICguk6GqAjnpr1ZcG4uaOUA1EVNYAJGyIywSp6IcABeNJ21CQEI4BIn4o0cuI8dT1snYb0onwUUNHBY8G6PWwkoR07rqwTZVzC9D6UyqBUFtagNY1ReQ4kBNAWisvAPGmt/fvc0TkuGjYxzfQkitaoJULYweCcD5D4HgcCECeF62E1pURrVmgTUnug+LgDAjE8b8s3DHlgIBoZ82pbBeMcy6y6yzQgRVkpAQKskoZNUWJiPJAsFQCxQYe8TjHZJMF2uxmo/n/HhaqsjaglKboVRKdSmj5SAgqgJgFpYGIigIZocsCRSZUAqJ6glMgoTw90K49fS8LROspkIuAXE5l1APtXmjR+yHmDs+AsoiWnpr7nfXEihLDaQrIYixrBhL3zEZ7QyCbeEgMHRqj7g7EpKLo9n2B0uGHQkSxUVuFsKkA7XzoyHT7wUG+k+X+p44MjikQeYpunIi7DkQnKtk8wz5NUD/Ld1lCyI8AARHoGCDSJCKkgIaq7GhaIH5kg/2nvNNAgidocDxQxBPeY90MkykQpu5gygPcAITwBCCiGta/KNDYqTCYK2MT6BFAtCfEvWI8EB0S1QtjI4F0bqwvUBiR9SPhq/daBgYb0i2R9lbZYTmoU1p/G6pccBsFpA5+B/khfXrDHDom0AT6qkXgWb64aLKnkKSCsCcvRLwDCEmWDAVC7EFUBtozd+S0nLhvmfrClBJylzhBCoie499ZAAndlICYMUW/MwAK05Y00VIAkeY4cvS79lDoTdMAFIgeAwTcHdwCJFoTpnM/EPAsWiMgnk+pBjruW9tQEQipo0k4zG5AxJ8Ih4MpjR33bf0QVwAmbEE6ydR9M0/d4/luAl0EGhi/I84g/49H+ydZJ3fVZMVzrSbM1lO3zoV0obWVB3P/rM7V8fCxlQczsq5Lfj/GX0zVVLNd4YSo7tGUBAQRkdluhIrakAHhpW1c7V9IfvWqu1B+oSD8CTSBJtDvA3qcHxoB9LihIzGWyzG6KXA4tjfWx3mYj/QawjOSvdgj8rSLz78VSKSMXeIpqAxrjfqKKSqMWlUdynULk72wyZpU1aPhQkrBMeIEmkDPA7oWD02gXwpU5alLo4mFp2aTG6U5C9SIMFOTsg+LETEzuKImAs/VpHQqcuntSjhUmEHTPi4AGMwMquYY8TtnYZ88Tz3LLFBex/tcXmFV7+6sw9dxbclDPPxJeFVOr9sC0S12wPOuL2xFMgWC9q1RVkB7ijyyHYDPAcJmjXWVEALcaUMc4hrQyxgI7wU6gkLgmwnQwFM/MWN8llnmJoHfXPx29seZ3LVuBngz7zfee9rO9smTI8Vn+r89+b/RH/cbnzh2lD4Sko2JZjwAF8/7zRAgKAL58LPONuQzpsol5IkeuwGJi4cSkH0vY39R2nIJyPfo9p50k6hLF/yQT+jartt72vn90eAsuvIPKgQeAXd3LiUAAAAASUVORK5CYII=" alt="Clean" style="width:120px;image-rendering:pixelated;display:block;margin:0 auto 4px">Clean</div>' +
    '</div>';

var ABOUT_HTML =
    '<div style="font-size:0.9em;line-height:1.4">' +
    '<p><b>Battery.</b> The bar shows charge; the small time under it is how long since the watch was last unplugged (or, while charging, how long it has been on the charger). A * before the percentage means charging.</p>' +
    '<p><b>Weather age.</b> The number in the top-right corner is how long ago the weather was fetched. A <b>!</b> in front of it means the last fetch failed and you are seeing older data; the header also turns yellow when the data is over two hours old and red after a failed fetch.</p>' +
    '<p><b>~ before the city.</b> The phone could not get a GPS fix, so the weather is fresh but for the place the phone was last seen. If you have typed a city under Weather, it is used when there is no remembered position either.</p>' +
    '<p><b>Steps and the arrow.</b> With steps selected under Extra information, the arrow next to the count compares today with your recent daily average: up means ahead, down means behind.</p>' +
    '<p><b>Week number.</b> CW is the ISO calendar week. Fiscal week (WK) counts weeks from a start date you choose - useful if your work year does not start in January.</p>' +
    '<p><b>Seconds.</b> Showing seconds all the time costs battery. "On shake" shows them for a few seconds when you flick your wrist.</p>' +
    '<p><b>Moon.</b> At night the weather icon can be replaced by the current moon phase. "Unless it is raining or snowing" keeps the weather icon on wet nights, when it is the more useful of the two.</p>' +
    '<p><b>Sleep in the morning, then steps.</b> Shows last night\'s sleep until the time you pick, then steps for the rest of the day.</p>' +
    '<p>MSS Refreshed is a fan update of Multifunctional 7 Segment by FG. Source and downloads: github.com/amethus1/MMS7-Pebble</p>' +
    '</div>';

module.exports = [
    {
        "type": "heading",
        "defaultValue": "MSS Refreshed"
    },

    // ========== PRESETS ==========
    {
        "type": "section",
        "items": [
            { "type": "heading", "defaultValue": "Presets", "size": 5 },
            { "type": "text", "id": "preset_previews", "defaultValue": PRESET_PREVIEWS_HTML },
            {
                "type": "button",
                "id": "preset_classic",
                "defaultValue": "Classic - everything, like the original"
            },
            {
                "type": "button",
                "id": "preset_clean",
                "defaultValue": "Clean - fewer details, more room"
            },
            {
                "type": "text",
                "defaultValue": "A preset fills in the settings below; you can still change any of them before tapping Save."
            }
        ]
    },

    // ========== APPEARANCE ==========
    {
        "type": "section",
        "items": [
            { "type": "heading", "defaultValue": "Appearance", "size": 5 },
            {
                "type": "select",
                "messageKey": "KEY_SET_INVERT_COLOR",
                "capabilities": ["COLOR"],
                "label": "Colour scheme",
                "defaultValue": 2,
                "options": COLOR_SCHEME_OPTIONS
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_INVERT_COLOR",
                "capabilities": ["BW"],
                "label": "Colour scheme",
                "defaultValue": 0,
                "options": [
                    { "label": "White on black", "value": 0 },
                    { "label": "Black on white", "value": 1 }
                ]
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_COLORED_TMP",
                "capabilities": ["COLOR"],
                "label": "Coloured temperature",
                "defaultValue": true,
                "description": "Blue when cold through red when hot"
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_SHOW_GRID",
                "label": "Grid lines",
                "defaultValue": true,
                "description": "The separator lines between sections"
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_HIDE_BATTERY_TIME",
                "label": "Hide the time under the battery",
                "defaultValue": false,
                "description": "That small time is how long since the last charge"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_MOON_PHASE",
                "label": "Moon phase",
                "defaultValue": 0,
                "options": [
                    { "label": "At night, replacing the weather icon", "value": 0 },
                    { "label": "At night, unless it is raining or snowing", "value": 3 },
                    { "label": "Always", "value": 1 },
                    { "label": "Never - always the weather icon", "value": 2 }
                ]
            },
            {
                "type": "toggle",
                "messageKey": "hideBluetooth",
                "label": "Hide the Bluetooth label",
                "defaultValue": false,
                "description": "Only shows the label when the phone is disconnected"
            }
        ]
    },

    // ========== TIME & DATE ==========
    {
        "type": "section",
        "items": [
            { "type": "heading", "defaultValue": "Time & Date", "size": 5 },
            {
                "type": "select",
                "messageKey": "KEY_SET_DATE_FORMAT",
                "label": "Date format",
                "defaultValue": "%a, %d.%m.%Y",
                "options": [
                    { "label": "Wed, 15.01.2025", "value": "%a, %d.%m.%Y" },
                    { "label": "Wed, 15.01.25", "value": "%a, %d.%m.%y" },
                    { "label": "15.01.2025", "value": "%d.%m.%Y" },
                    { "label": "Wed, 15/01/2025", "value": "%a, %d/%m/%Y" },
                    { "label": "Wed, 01/15/2025", "value": "%a, %m/%d/%Y" },
                    { "label": "01/15/2025", "value": "%m/%d/%Y" },
                    { "label": "Wed, 2025-01-15", "value": "%a, %Y-%m-%d" },
                    { "label": "2025-01-15 (ISO)", "value": "%Y-%m-%d" },
                    { "label": "Wed 15 Jan", "value": "%a %d %b" },
                    { "label": "15 Jan", "value": "%d %b" }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_DISPLAY_SEC",
                "label": "Seconds",
                "defaultValue": 0,
                "description": "Always on uses noticeably more battery",
                "options": [
                    { "label": "Off", "value": 0 },
                    { "label": "Always on", "value": 1 },
                    { "label": "On shake, for 5 s", "value": 2 },
                    { "label": "On shake, for 15 s", "value": 3 },
                    { "label": "On shake, for 30 s", "value": 4 }
                ]
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_LEADING_ZERO",
                "label": "Leading zero (08:30)",
                "defaultValue": true,
                "description": "Off shows 8:30"
            },
            {
                // Drives the two hidden toggles below (see custom-clay.js), so
                // the watch-side keys are unchanged.
                "type": "select",
                "id": "week_mode",
                "label": "Week number",
                "defaultValue": 1,
                "options": [
                    { "label": "Off", "value": 0 },
                    { "label": "Separate label (CW36)", "value": 1 },
                    { "label": "In the date line, instead of the year", "value": 2 }
                ]
            },
            {
                "type": "toggle",
                "messageKey": "hideCW",
                "id": "week_hidden",
                "label": "(internal) hide week",
                "defaultValue": false
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_WEEK_IN_DATE",
                "id": "week_in_date",
                "label": "(internal) week in date",
                "defaultValue": false
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_WEEK_TYPE",
                "label": "Week counting",
                "defaultValue": 0,
                "options": [
                    { "label": "Calendar week (ISO 8601)", "value": 0 },
                    { "label": "Fiscal week, from a date you choose", "value": 1 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_FISCAL_START_MONTH",
                "group": "fiscal",
                "label": "Fiscal year starts in",
                "defaultValue": 1,
                "options": [
                    { "label": "January", "value": 0 },
                    { "label": "February", "value": 1 },
                    { "label": "March", "value": 2 },
                    { "label": "April", "value": 3 },
                    { "label": "May", "value": 4 },
                    { "label": "June", "value": 5 },
                    { "label": "July", "value": 6 },
                    { "label": "August", "value": 7 },
                    { "label": "September", "value": 8 },
                    { "label": "October", "value": 9 },
                    { "label": "November", "value": 10 },
                    { "label": "December", "value": 11 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_FISCAL_START_DAY",
                "group": "fiscal",
                "label": "Fiscal year starts on day",
                "defaultValue": 1,
                "options": [
                    { "label": "1", "value": 1 }, { "label": "2", "value": 2 }, { "label": "3", "value": 3 },
                    { "label": "4", "value": 4 }, { "label": "5", "value": 5 }, { "label": "6", "value": 6 },
                    { "label": "7", "value": 7 }, { "label": "8", "value": 8 }, { "label": "9", "value": 9 },
                    { "label": "10", "value": 10 }, { "label": "11", "value": 11 }, { "label": "12", "value": 12 },
                    { "label": "13", "value": 13 }, { "label": "14", "value": 14 }, { "label": "15", "value": 15 }
                ]
            }
        ]
    },

    // ========== WEATHER ==========
    {
        "type": "section",
        "items": [
            { "type": "heading", "defaultValue": "Weather", "size": 5 },
            {
                "type": "select",
                "messageKey": "autodetect_loc",
                "label": "Location",
                "defaultValue": 1,
                "options": [
                    { "label": "Follow the phone (GPS)", "value": 1 },
                    { "label": "Always use the city below", "value": 0 }
                ]
            },
            {
                "type": "input",
                "messageKey": "default_loc",
                "id": "city",
                "label": "City",
                "defaultValue": "",
                "attributes": { "placeholder": "e.g. Chicago" },
                "description": "Optional. Used when the phone's location is unavailable"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_DEGREE_F",
                "label": "Temperature",
                "defaultValue": 1,
                "options": [
                    { "label": "Celsius", "value": 0 },
                    { "label": "Fahrenheit", "value": 1 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_SPEED_UNIT",
                "label": "Wind speed",
                "defaultValue": 0,
                "options": [
                    { "label": "km/h", "value": 0 },
                    { "label": "mph", "value": 1 },
                    { "label": "m/s", "value": 2 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_PRESSURE_UNIT",
                "label": "Pressure",
                "defaultValue": 0,
                "options": [
                    { "label": "hPa", "value": 0 },
                    { "label": "mmHg", "value": 1 },
                    { "label": "inHg", "value": 2 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_WEATHER_UPDATE_INT",
                "label": "Refresh every",
                "defaultValue": 20,
                "options": [
                    { "label": "10 minutes", "value": 10 },
                    { "label": "20 minutes", "value": 20 },
                    { "label": "30 minutes", "value": 30 },
                    { "label": "1 hour", "value": 60 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_UPDATE_TIME",
                "label": "Weather age counts from",
                "defaultValue": 0,
                "description": "The small time in the top-right corner",
                "options": [
                    { "label": "When the phone last sent weather", "value": 0 },
                    { "label": "When the weather was observed", "value": 1 }
                ]
            },
            {
                "type": "input",
                "messageKey": "lang_id",
                "label": "Language for city names",
                "defaultValue": "en",
                "description": "Two-letter code (en, de, fr, es ...). Affects how the location name is shown."
            }
        ]
    },

    // ========== WEATHER DETAILS ==========
    {
        "type": "section",
        "items": [
            { "type": "heading", "defaultValue": "Weather details", "size": 5 },
            { "type": "text", "defaultValue": LAYOUT_GUIDE_HTML },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_1",
                "label": "1 - Right column, top",
                "defaultValue": 5,
                "options": WEATHER_DETAIL_OPTIONS
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_2",
                "label": "2 - Right column, bottom",
                "defaultValue": 2,
                "options": WEATHER_DETAIL_OPTIONS
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_3",
                "label": "3 - Left line, first",
                "defaultValue": 3,
                "options": WEATHER_DETAIL_OPTIONS
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_4",
                "label": "3 - Left line, second",
                "defaultValue": 4,
                "options": WEATHER_DETAIL_OPTIONS
            }
        ]
    },

    // ========== EXTRA INFORMATION ==========
    {
        "type": "section",
        "items": [
            { "type": "heading", "defaultValue": "Extra information", "size": 5 },
            {
                "type": "text",
                "capabilities": ["RECT"],
                "defaultValue": "The slot at the bottom left, next to the week number."
            },
            {
                "type": "text",
                "capabilities": ["ROUND"],
                "defaultValue": "The slot at the bottom of the circle, under the sunrise and sunset times."
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_HEALTH",
                "capabilities": ["HEALTH"],
                "label": "Show",
                "defaultValue": 5,
                "options": [
                    { "label": "Nothing", "value": 0 },
                    { "label": "Sleep in the morning, then steps", "value": 1 },
                    { "label": "Steps", "value": 3 },
                    { "label": "Sleep", "value": 4 },
                    { "label": "Timezone", "value": 5 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_HEALTH",
                "capabilities": ["NOT_HEALTH"],
                "label": "Show",
                "defaultValue": 5,
                "options": [
                    { "label": "Nothing", "value": 0 },
                    { "label": "Timezone", "value": 5 }
                ]
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_EXTRA_INFO_CENTER",
                "capabilities": ["PLATFORM_EMERY"],
                "label": "Show it in the middle of the row",
                "defaultValue": false,
                "description": "The week number moves to the left; seconds stay on the right"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_SLEEP_UNTIL_HOUR",
                "capabilities": ["HEALTH"],
                "label": "Show sleep until",
                "defaultValue": 10,
                "description": "With \"Steps by day, sleep at night\": last night's sleep is shown until this time, steps after",
                "options": [
                    { "label": "06:00", "value": 6 }, { "label": "07:00", "value": 7 }, { "label": "08:00", "value": 8 },
                    { "label": "09:00", "value": 9 }, { "label": "10:00", "value": 10 }, { "label": "11:00", "value": 11 },
                    { "label": "12:00", "value": 12 }, { "label": "13:00", "value": 13 }, { "label": "14:00", "value": 14 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_TZ_FORMAT",
                "label": "Timezone shown as",
                "defaultValue": 1,
                "description": "The timezone of the weather location",
                "options": [
                    { "label": "UTC time (12:54 UTC)", "value": 0 },
                    { "label": "Name (CST)", "value": 1 },
                    { "label": "AM/PM and name (PM, CST)", "value": 2 }
                ]
            }
        ]
    },

    // ========== ALERTS & POWER ==========
    {
        "type": "section",
        "items": [
            { "type": "heading", "defaultValue": "Alerts & Power", "size": 5 },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_VIBE_DISC",
                "label": "Vibrate when the phone disconnects",
                "defaultValue": false
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_VIBE_HOUR",
                "label": "Vibrate on the hour",
                "defaultValue": false
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LIGHT_ON",
                "label": "Backlight",
                "defaultValue": 1,
                "description": "Always on and Blinking keep the backlight lit and shorten battery life considerably",
                "options": [
                    { "label": "Normal (button or wrist flick)", "value": 0 },
                    { "label": "On while charging", "value": 1 },
                    { "label": "Always on", "value": 2 },
                    { "label": "Blinking", "value": 3 }
                ]
            }
        ]
    },

    // ========== ABOUT ==========
    {
        "type": "section",
        "items": [
            { "type": "heading", "defaultValue": "About this face", "size": 5 },
            {
                "type": "button",
                "id": "about_toggle",
                "defaultValue": "What do the small things mean?"
            },
            { "type": "text", "id": "about_text", "group": "about", "defaultValue": ABOUT_HTML },
            {
                "type": "button",
                "id": "reset_defaults",
                "defaultValue": "Reset everything to defaults",
                "description": "Puts every setting on this page back to its default. Nothing is saved until you tap Save."
            }
        ]
    },

    {
        "type": "submit",
        "defaultValue": "Save"
    }
];
