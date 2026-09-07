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

var ABOUT_HTML =
    '<div style="font-size:0.9em;line-height:1.4">' +
    '<p><b>Battery.</b> The bar shows charge; the small time under it is how long since the watch was last unplugged (or, while charging, how long it has been on the charger). A * before the percentage means charging.</p>' +
    '<p><b>Weather age.</b> The number in the top-right corner is how long ago the weather was fetched. The header turns yellow when the data is over two hours old and red when the last fetch failed; on black-and-white watches the header is inverted instead.</p>' +
    '<p><b>Steps and the arrow.</b> With steps selected under Extra information, the arrow next to the count compares today with your recent daily average: up means ahead, down means behind.</p>' +
    '<p><b>Week number.</b> CW is the ISO calendar week. Fiscal week (WK) counts weeks from a start date you choose - useful if your work year does not start in January.</p>' +
    '<p><b>Seconds.</b> Showing seconds all the time costs battery. "On shake" shows them for a few seconds when you flick your wrist.</p>' +
    '<p><b>Moon.</b> At night the weather icon is replaced by the current moon phase. You can turn that off above.</p>' +
    '<p>MSS Refreshed is a fan update of Multifunctional 7 Segment by FG. Source and downloads: github.com/amethus1/MMS7-Pebble</p>' +
    '</div>';

module.exports = [
    {
        "type": "heading",
        "defaultValue": "MSS Refreshed"
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
                "type": "select",
                "messageKey": "KEY_SET_MOON_PHASE",
                "label": "Moon phase",
                "defaultValue": 0,
                "options": [
                    { "label": "Replaces the weather icon at night", "value": 0 },
                    { "label": "Always shown", "value": 1 },
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
                "defaultValue": "Berlin",
                "description": "Used when the phone's location is unavailable"
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
                    { "label": "Steps by day, sleep at night", "value": 1 },
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
