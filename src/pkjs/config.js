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

// The weather-detail layout, drawn as a small table so the four slot names
// below map onto the face without guesswork.
var LAYOUT_GUIDE_HTML =
    '<div style="font-size:0.85em;line-height:1.35">' +
    'The area under the location line has three columns. Pick what goes in each slot:' +
    '<table style="width:100%;margin-top:6px;border-collapse:collapse;text-align:center;font-family:monospace;font-size:0.9em">' +
    '<tr>' +
      '<td style="border:1px solid #888;padding:4px">weather<br>icon</td>' +
      '<td style="border:1px solid #888;padding:4px">battery</td>' +
      '<td style="border:1px solid #888;padding:4px">temperature<br><b>Right column, top</b><br><b>Right column, bottom</b></td>' +
    '</tr>' +
    '<tr>' +
      '<td colspan="2" style="border:1px solid #888;padding:4px"><b>Left line, first</b> / <b>Left line, second</b></td>' +
      '<td style="border:1px solid #888;padding:4px">&nbsp;</td>' +
    '</tr>' +
    '</table>' +
    'The two left values share one line, separated by a slash.' +
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
                "label": "Right column, top",
                "defaultValue": 5,
                "options": WEATHER_DETAIL_OPTIONS
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_2",
                "label": "Right column, bottom",
                "defaultValue": 2,
                "options": WEATHER_DETAIL_OPTIONS
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_3",
                "label": "Left line, first",
                "defaultValue": 3,
                "options": WEATHER_DETAIL_OPTIONS
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_4",
                "label": "Left line, second",
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
