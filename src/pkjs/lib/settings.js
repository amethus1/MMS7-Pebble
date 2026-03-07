var SETTINGS_STORAGE_KEY = 'mss-js-settings-v2';
var SETTINGS_MIGRATION_KEY = 'mss-settings-migrated-v2';

var s_settings = {
    location: 'Berlin',
    autodetect: true,
    lang: 'en'
};

function parseIntOr(value, fallback) {
    var parsed = parseInt(value, 10);
    return isNaN(parsed) ? fallback : parsed;
}

function getSettingValue(settings, key, defaultValue) {
    if (!settings || settings[key] === undefined) return defaultValue;
    var value = settings[key];
    if (typeof value === 'object' && value !== null && value.value !== undefined) {
        return value.value;
    }
    return value;
}

function findSetting(settings, keysToCheck) {
    if (!settings || !keysToCheck) return null;

    for (var i = 0; i < keysToCheck.length; i++) {
        var key = keysToCheck[i];
        if (settings[key] !== undefined) {
            return {
                key: key,
                value: getSettingValue(settings, key, settings[key])
            };
        }
    }

    return null;
}

function extractLegacyValue(obj, keysToCheck) {
    if (!obj || !keysToCheck) return null;

    for (var i = 0; i < keysToCheck.length; i++) {
        var key = keysToCheck[i];
        if (obj[key] === undefined) continue;
        return getSettingValue(obj, key, obj[key]);
    }

    return null;
}

function filterNumericAppMessageKeys(dict) {
    var clean = {};
    if (!dict) return clean;

    Object.keys(dict).forEach(function (k) {
        if (/^\d+$/.test(String(k))) {
            clean[k] = dict[k];
        }
    });

    return clean;
}

function load(log) {
    try {
        var raw = localStorage.getItem(SETTINGS_STORAGE_KEY);
        if (!raw) return;

        var parsed = JSON.parse(raw);
        if (!parsed || typeof parsed !== 'object') return;

        if (parsed.location !== undefined) {
            s_settings.location = String(parsed.location || s_settings.location);
        }
        if (parsed.autodetect !== undefined) {
            s_settings.autodetect = !!parsed.autodetect;
        }
        if (parsed.lang !== undefined) {
            s_settings.lang = String(parsed.lang || s_settings.lang);
        }
    } catch (e) {
        if (log) log('Load settings error: ' + e);
    }
}

function save(log) {
    try {
        localStorage.setItem(SETTINGS_STORAGE_KEY, JSON.stringify(s_settings));
    } catch (e) {
        if (log) log('Save settings error: ' + e);
    }
}

function applyJsOnlySettings(rawSettings) {
    var setting = findSetting(rawSettings, ['default_loc']);
    if (setting) {
        s_settings.location = String(setting.value || '');
    }

    setting = findSetting(rawSettings, ['autodetect_loc']);
    if (setting) {
        s_settings.autodetect = parseIntOr(setting.value, s_settings.autodetect ? 1 : 0) === 1;
    }

    setting = findSetting(rawSettings, ['lang_id']);
    if (setting) {
        var lang = String(setting.value || '').trim();
        s_settings.lang = lang || 'en';
    }
}

function migrateLegacyUnitSettings(keys, sendFn, log) {
    try {
        if (localStorage.getItem(SETTINGS_MIGRATION_KEY) === '1') {
            return;
        }

        var speedUnit = null;
        var pressureUnit = null;

        var oldJsRaw = localStorage.getItem('mss-js-settings');
        if (oldJsRaw) {
            var oldJs = JSON.parse(oldJsRaw);
            speedUnit = extractLegacyValue(oldJs, ['speedUnit', 'speed_unit']);
            pressureUnit = extractLegacyValue(oldJs, ['pressureUnit', 'pressure_unit']);
        }

        var oldClayRaw = localStorage.getItem('clay-settings');
        if (oldClayRaw) {
            var oldClay = JSON.parse(oldClayRaw);
            if (speedUnit === null) {
                speedUnit = extractLegacyValue(oldClay, ['speed_unit', 'speedUnit']);
            }
            if (pressureUnit === null) {
                pressureUnit = extractLegacyValue(oldClay, ['pressure_unit', 'pressureUnit']);
            }
        }

        var dict = {};
        if (speedUnit !== null && speedUnit !== undefined) {
            dict[keys.KEY_SET_SPEED_UNIT] = parseIntOr(speedUnit, 0);
        }
        if (pressureUnit !== null && pressureUnit !== undefined) {
            dict[keys.KEY_SET_PRESSURE_UNIT] = parseIntOr(pressureUnit, 0);
        }

        if (Object.keys(dict).length === 0) {
            localStorage.setItem(SETTINGS_MIGRATION_KEY, '1');
            return;
        }

        sendFn(dict, function () {
            if (log) log('Legacy unit settings migrated');
            localStorage.setItem(SETTINGS_MIGRATION_KEY, '1');
        }, function (e) {
            if (log) log('Legacy migration failed: ' + JSON.stringify(e));
        });
    } catch (e) {
        if (log) log('Legacy migration error: ' + e);
    }
}

function get() {
    return {
        location: s_settings.location,
        autodetect: s_settings.autodetect,
        lang: s_settings.lang
    };
}

module.exports = {
    filterNumericAppMessageKeys: filterNumericAppMessageKeys,
    load: load,
    save: save,
    applyJsOnlySettings: applyJsOnlySettings,
    migrateLegacyUnitSettings: migrateLegacyUnitSettings,
    get: get
};
