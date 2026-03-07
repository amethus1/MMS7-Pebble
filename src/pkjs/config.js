// MSS Refreshed - Clay Configuration
// Streamlined settings with clear organization

module.exports = [
    {
        "type": "heading",
        "defaultValue": "MSS Refreshed"
    },

    // ========== APPEARANCE ==========
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "🎨 Appearance"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_INVERT_COLOR",
                "label": "Color Scheme",
                "defaultValue": 2,
                "options": [
                    { "label": "White on Black", "value": 0 },
                    { "label": "Black on White", "value": 1 },
                    { "label": "White on Black (colors)", "value": 2 },
                    { "label": "Black on White (colors)", "value": 3 },
                    { "label": "Red on Black", "value": 4 },
                    { "label": "Green on Black", "value": 5 },
                    { "label": "Blue on Black", "value": 6 },
                    { "label": "Colorful (Blue clock)", "value": 14 },
                    { "label": "Colorful (White clock)", "value": 15 }
                ]
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_COLORED_TMP",
                "label": "Colored Temperature",
                "defaultValue": true,
                "description": "Temperature changes color based on value"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LIGHT_ON",
                "label": "Background Light",
                "defaultValue": 1,
                "options": [
                    { "label": "Normal", "value": 0 },
                    { "label": "On when charging", "value": 1 },
                    { "label": "Always on", "value": 2 },
                    { "label": "Blinking (1 sec)", "value": 3 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_DISPLAY_SEC",
                "label": "Display Seconds",
                "defaultValue": 0,
                "options": [
                    { "label": "Off", "value": 0 },
                    { "label": "Always On", "value": 1 },
                    { "label": "On shake (5s)", "value": 2 },
                    { "label": "On shake (15s)", "value": 3 },
                    { "label": "On shake (30s)", "value": 4 }
                ]
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_SHOW_GRID",
                "label": "Show Grid Lines",
                "defaultValue": true,
                "description": "Show/hide separator lines between sections"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_MOON_PHASE",
                "label": "Moon Phase",
                "defaultValue": 0,
                "options": [
                    { "label": "Show at night", "value": 0 },
                    { "label": "Always show", "value": 1 },
                    { "label": "Never (weather icon)", "value": 2 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_DATE_FORMAT",
                "label": "Date Format",
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
            }
        ]
    },

    // ========== WEATHER ==========
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "🌤️ Weather"
            },
            {
                "type": "input",
                "messageKey": "default_loc",
                "label": "Location",
                "defaultValue": "Berlin",
                "description": "City name (used if GPS fails or disabled)"
            },
            {
                "type": "select",
                "messageKey": "autodetect_loc",
                "label": "Location Mode",
                "defaultValue": 1,
                "options": [
                    { "label": "Auto-detect (GPS)", "value": 1 },
                    { "label": "Use city name above", "value": 0 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_DEGREE_F",
                "label": "Temperature Unit",
                "defaultValue": 1,
                "options": [
                    { "label": "Celsius (°C)", "value": 0 },
                    { "label": "Fahrenheit (°F)", "value": 1 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_WEATHER_UPDATE_INT",
                "label": "Update Interval",
                "defaultValue": 20,
                "options": [
                    { "label": "10 min", "value": 10 },
                    { "label": "20 min", "value": 20 },
                    { "label": "30 min", "value": 30 },
                    { "label": "1 hour", "value": 60 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_UPDATE_TIME",
                "label": "Show Update Time",
                "defaultValue": 0,
                "options": [
                    { "label": "Time since phone update", "value": 0 },
                    { "label": "Time since weather data", "value": 1 }
                ]
            }
        ]
    },

    // ========== WEATHER INFO LINES ==========
    {
        "type": "section",
        "capabilities": ["NOT_PLATFORM_APLITE"],
        "items": [
            {
                "type": "heading",
                "defaultValue": "Weather Info Lines",
                "size": 6
            },
            {
                "type": "text",
                "defaultValue": "Choose what data to show below the temperature"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_1",
                "label": "Right Side - Row 1",
                "defaultValue": 5,
                "options": [
                    { "label": "Empty", "value": 0 },
                    { "label": "Conditions", "value": 1 },
                    { "label": "Wind Speed", "value": 2 },
                    { "label": "Humidity", "value": 3 },
                    { "label": "Pressure", "value": 4 },
                    { "label": "High/Low Temp", "value": 5 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_2",
                "label": "Right Side - Row 2",
                "defaultValue": 2,
                "options": [
                    { "label": "Empty", "value": 0 },
                    { "label": "Conditions", "value": 1 },
                    { "label": "Wind Speed", "value": 2 },
                    { "label": "Humidity", "value": 3 },
                    { "label": "Pressure", "value": 4 },
                    { "label": "High/Low Temp", "value": 5 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_3",
                "label": "Left Side - Row 1",
                "defaultValue": 3,
                "options": [
                    { "label": "Empty", "value": 0 },
                    { "label": "Conditions", "value": 1 },
                    { "label": "Wind Speed", "value": 2 },
                    { "label": "Humidity", "value": 3 },
                    { "label": "Pressure", "value": 4 },
                    { "label": "High/Low Temp", "value": 5 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_LABEL_INDEX_4",
                "label": "Left Side - Row 2",
                "defaultValue": 4,
                "options": [
                    { "label": "Empty", "value": 0 },
                    { "label": "Conditions", "value": 1 },
                    { "label": "Wind Speed", "value": 2 },
                    { "label": "Humidity", "value": 3 },
                    { "label": "Pressure", "value": 4 },
                    { "label": "High/Low Temp", "value": 5 }
                ]
            }
        ]
    },

    // ========== ADVANCED WEATHER ==========
    {
        "type": "section",
        "capabilities": ["NOT_PLATFORM_APLITE"],
        "items": [
            {
                "type": "heading",
                "defaultValue": "Advanced Weather",
                "size": 6
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_SPEED_UNIT",
                "label": "Wind Speed Unit",
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
                "label": "Pressure Unit",
                "defaultValue": 0,
                "options": [
                    { "label": "hPa", "value": 0 },
                    { "label": "mmHg", "value": 1 },
                    { "label": "inHg", "value": 2 }
                ]
            },
            {
                "type": "input",
                "messageKey": "lang_id",
                "label": "Weather Language",
                "defaultValue": "en",
                "description": "Language code: en, de, fr, es, etc."
            }
        ]
    },

    // ========== BOTTOM LEFT DISPLAY ==========
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "⚙️ Bottom Left Display"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_HEALTH",
                "label": "Show",
                "defaultValue": 5,
                "description": "Choose what to display in the bottom left",
                "options": [
                    { "label": "Off", "value": 0 },
                    { "label": "Steps/Sleep (auto)", "value": 1 },
                    { "label": "Steps only", "value": 3 },
                    { "label": "Sleep only", "value": 4 },
                    { "label": "Timezone", "value": 5 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_TZ_FORMAT",
                "label": "Timezone Format",
                "defaultValue": 1,
                "options": [
                    { "label": "UTC Time (12:54 UTC)", "value": 0 },
                    { "label": "Timezone Name (CST)", "value": 1 },
                    { "label": "AM/PM + Timezone", "value": 2 }
                ],
                "description": "Used when 'Timezone' is selected above"
            }
        ]
    },

    // ========== WEEK NUMBER ==========
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "📅 Week Number",
                "size": 6
            },
            {
                "type": "toggle",
                "messageKey": "hideCW",
                "label": "Hide Week Number",
                "defaultValue": false
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_WEEK_IN_DATE",
                "label": "Show Week in Date Line",
                "defaultValue": false,
                "description": "Replaces year with week number in date"
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_WEEK_TYPE",
                "label": "Week Type",
                "defaultValue": 0,
                "options": [
                    { "label": "Calendar (ISO)", "value": 0 },
                    { "label": "Fiscal", "value": 1 }
                ]
            },
            {
                "type": "select",
                "messageKey": "KEY_SET_FISCAL_START_MONTH",
                "label": "Fiscal Year Starts",
                "defaultValue": 1,
                "description": "Month when fiscal year begins",
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
                "label": "Fiscal Start Day",
                "defaultValue": 1,
                "options": [
                    { "label": "1", "value": 1 },
                    { "label": "2", "value": 2 },
                    { "label": "3", "value": 3 },
                    { "label": "4", "value": 4 },
                    { "label": "5", "value": 5 },
                    { "label": "6", "value": 6 },
                    { "label": "7", "value": 7 },
                    { "label": "8", "value": 8 },
                    { "label": "9", "value": 9 },
                    { "label": "10", "value": 10 },
                    { "label": "11", "value": 11 },
                    { "label": "12", "value": 12 },
                    { "label": "13", "value": 13 },
                    { "label": "14", "value": 14 },
                    { "label": "15", "value": 15 }
                ]
            }
        ]
    },

    // ========== OTHER ==========
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Other",
                "size": 6
            },
            {
                "type": "toggle",
                "messageKey": "hideBluetooth",
                "label": "Hide Bluetooth Icon",
                "defaultValue": false,
                "description": "Hides 'Bluetooth' when connected"
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_VIBE_DISC",
                "label": "Vibrate on Disconnect",
                "defaultValue": false
            },
            {
                "type": "toggle",
                "messageKey": "KEY_SET_VIBE_HOUR",
                "label": "Vibrate on Hour",
                "defaultValue": false
            }
        ]
    },

    {
        "type": "submit",
        "defaultValue": "Save Settings"
    }
];
