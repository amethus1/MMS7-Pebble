var s_fetch_in_flight = false;
var s_fetch_watchdog = null;

// Reasons sent with KEY_WEATHER_FETCH_ERROR; the watch words them differently.
var ERROR_FETCH_FAILED = 1;     // Network / API problem; cached weather stays
var ERROR_NO_LOCATION = 2;      // GPS unavailable and no city configured
var ERROR_CITY_NOT_FOUND = 3;   // The configured city did not geocode

// Mark a fetch as started. A watchdog guarantees the in-flight flag is cleared
// even if a callback never fires (e.g. geolocation that never resolves), so
// weather updates can never get permanently wedged.
function beginFetch() {
    s_fetch_in_flight = true;
    if (s_fetch_watchdog !== null) {
        clearTimeout(s_fetch_watchdog);
    }
    s_fetch_watchdog = setTimeout(function () {
        s_fetch_watchdog = null;
        s_fetch_in_flight = false;
    }, 60000);
}

// Mark a fetch as finished (success or failure). Idempotent.
function endFetch() {
    s_fetch_in_flight = false;
    if (s_fetch_watchdog !== null) {
        clearTimeout(s_fetch_watchdog);
        s_fetch_watchdog = null;
    }
}

function httpGetJson(url, onSuccess, onError) {
    var xhr = new XMLHttpRequest();
    xhr.timeout = 30000;
    xhr.onload = function () {
        if (xhr.status < 200 || xhr.status >= 300) {
            onError('HTTP ' + xhr.status);
            return;
        }

        try {
            onSuccess(JSON.parse(xhr.responseText));
        } catch (e) {
            onError('Parse error: ' + e);
        }
    };
    xhr.onerror = function () {
        onError('Network error');
    };
    xhr.ontimeout = function () {
        onError('Timeout');
    };
    xhr.open('GET', url);
    xhr.send();
}

function buildLocationName(result, fallbackName) {
    if (!result) {
        return (fallbackName || 'Unknown').substring(0, 31);
    }

    var parts = [];
    if (result.name) {
        parts.push(result.name);
    }
    if (result.admin1 && result.admin1 !== result.name) {
        parts.push(result.admin1);
    } else if (result.country_code && result.country_code !== result.name) {
        parts.push(result.country_code);
    }

    return (parts.join(', ') || fallbackName || 'Unknown').substring(0, 31);
}

function extractReverseGeocodeName(json) {
    if (!json) {
        return '';
    }

    if (json.name) {
        return String(json.name).substring(0, 31);
    }

    var address = json.address || {};
    return String(
        address.city ||
        address.town ||
        address.village ||
        address.hamlet ||
        address.county ||
        ''
    ).substring(0, 31);
}

function getTimezoneDisplayName(timezoneName, unixTime, fallbackName) {
    if (!timezoneName) {
        return (fallbackName || 'UTC').substring(0, 31);
    }

    try {
        var fmt = new Intl.DateTimeFormat('en-US', {
            timeZone: timezoneName,
            timeZoneName: 'short'
        });
        var parts = fmt.formatToParts(new Date(unixTime * 1000));
        for (var i = 0; i < parts.length; i++) {
            if (parts[i].type === 'timeZoneName' && parts[i].value) {
                var derivedName = String(parts[i].value);
                if (!/^GMT[+-]/.test(derivedName)) {
                    return derivedName.substring(0, 31);
                }
            }
        }
    } catch (e) {
        // Fall through to the provided fallback.
    }

    return String(fallbackName || timezoneName || 'UTC').substring(0, 31);
}

function getWeatherIcon(code, isDay) {
    // Climacons ASCII map used by resources/fonts/Climacons.ttf:
    // ! cloud, "/# cloud+sun/moon, $/%/& rain, '/(/) showers,
    // */+/, downpour, -/./ drizzle, 0/1/2 sleet, 3/4/5 hail,
    // 6/7/8 flurries, 9/:/; snow, </=/> fog, F/G/H lightning,
    // I sun, N moon.
    if (code === 0) return isDay ? 'I' : 'N';
    if (code === 1 || code === 2) return isDay ? '"' : '#';
    if (code === 3) return '!';
    if (code === 45 || code === 48) return isDay ? '=' : '>';
    if (code >= 51 && code <= 55) return isDay ? '.' : '/';
    if (code === 56 || code === 57) return isDay ? '1' : '2';
    if (code >= 61 && code <= 65) return isDay ? '%' : '&';
    if (code === 66 || code === 67) return isDay ? '1' : '2';
    if (code >= 71 && code <= 75) return isDay ? ':' : ';';
    if (code === 77) return isDay ? '7' : '8';
    if (code === 80 || code === 81) return isDay ? '(' : ')';
    if (code === 82) return isDay ? '+' : ',';
    if (code === 85) return isDay ? '7' : '8';
    if (code === 86) return isDay ? ':' : ';';
    if (code === 95) return isDay ? 'G' : 'H';
    if (code === 96 || code === 99) return isDay ? '4' : '5';
    return '!';
}

function getWeatherDescription(code) {
    var descriptions = {
        0: 'Clear sky',
        1: 'Mainly clear',
        2: 'Partly cloudy',
        3: 'Overcast',
        45: 'Fog',
        48: 'Rime fog',
        51: 'Light drizzle',
        53: 'Drizzle',
        55: 'Dense drizzle',
        56: 'Freezing drizzle',
        57: 'Heavy freezing drizzle',
        61: 'Light rain',
        63: 'Rain',
        65: 'Heavy rain',
        66: 'Freezing rain',
        67: 'Heavy freezing rain',
        71: 'Light snow',
        73: 'Snow',
        75: 'Heavy snow',
        77: 'Snow grains',
        80: 'Rain showers',
        81: 'Heavy showers',
        82: 'Violent showers',
        85: 'Snow showers',
        86: 'Heavy snow showers',
        95: 'Thunderstorm',
        96: 'Storm with hail',
        99: 'Severe hailstorm'
    };

    return descriptions[code] || 'Unknown';
}

function firstOr(value, fallback) {
    return value && value.length ? value[0] : fallback;
}

function buildWeatherPayload(json, location, keys, nowUnix) {
    json = json || {};
    location = location || {};
    var current = json.current || {};
    var daily = json.daily || {};
    var now = nowUnix || Math.floor(Date.now() / 1000);
    var hasWeatherCode = current.weather_code !== undefined && current.weather_code !== null;
    var weatherCode = hasWeatherCode ? Number(current.weather_code) : -1;
    var isDay = Number(current.is_day || 0) === 1;

    var dict = {};
    dict[keys.KEY_WEATHER_TEMP] = Math.round(Number(current.temperature_2m || 0));
    dict[keys.KEY_WEATHER_ICON] = getWeatherIcon(weatherCode, isDay).charCodeAt(0);
    var displayName = location && location.name ? location.name : 'Local';
    dict[keys.KEY_LOCATION_NAME] = buildLocationName(location, displayName);
    dict[keys.KEY_LOCATION_LAT] = Math.round(Number(location.lat || 0) * 1000000);
    dict[keys.KEY_LOCATION_LON] = Math.round(Number(location.lon || 0) * 1000000);
    dict[keys.KEY_SUN_RISE_UNIX] = Number(firstOr(daily.sunrise, 0));
    dict[keys.KEY_SUN_SET_UNIX] = Number(firstOr(daily.sunset, 0));
    dict[keys.KEY_TIME_LAST_UPDATE] = now;
    dict[keys.KEY_WEATHER_DATA_TIME] = Number(current.time || now);
    dict[keys.KEY_TIME_ZONE_NAME] = getTimezoneDisplayName(
        json.timezone,
        Number(current.time || now),
        json.timezone_abbreviation || json.timezone || 'UTC'
    );
    dict[keys.KEY_TIME_UTC_OFFSET] = Number(json.utc_offset_seconds || 0);

    dict[keys.KEY_WEATHER_HIGH] = Math.round(Number(firstOr(daily.temperature_2m_max, current.temperature_2m || 0)));
    dict[keys.KEY_WEATHER_LOW] = Math.round(Number(firstOr(daily.temperature_2m_min, current.temperature_2m || 0)));
    dict[keys.KEY_WEATHER_HUMIDITY] = Math.round(Number(current.relative_humidity_2m || 0));
    dict[keys.KEY_WEATHER_PRESSURE] = Math.round(Number(current.surface_pressure || 0));
    dict[keys.KEY_WEATHER_WIND_KMH] = Math.round(Number(current.wind_speed_10m || 0));
    dict[keys.KEY_WEATHER_CONDITIONS] = getWeatherDescription(weatherCode).substring(0, 31);
    // Fresh weather, but for a remembered position rather than a live fix
    dict[keys.KEY_LOCATION_UNCONFIRMED] = location.unconfirmed ? 1 : 0;

    return dict;
}

function sendFetchError(keys, sendWeatherDict, log, reason) {
    endFetch();
    var dict = {};
    dict[keys.KEY_WEATHER_FETCH_ERROR] = reason || ERROR_FETCH_FAILED;
    sendWeatherDict(dict);
    log('Sent fetch error ' + (reason || ERROR_FETCH_FAILED) + ' to watch');
}

function resolveCityLocation(city, lang, onSuccess, onError) {
    var url = 'https://geocoding-api.open-meteo.com/v1/search?count=1&format=json&language=' +
        encodeURIComponent(lang) + '&name=' + encodeURIComponent(city);

    httpGetJson(url, function (json) {
        if (!json.results || !json.results.length) {
            onError('No geocoding results');  // matched by name in fetchWeather
            return;
        }

        var result = json.results[0];
        onSuccess({
            name: buildLocationName(result, city),
            lat: result.latitude,
            lon: result.longitude
        });
    }, onError);
}

function reverseGeocodeLocation(lat, lon, onSuccess, onError) {
    // Match the working approach from Details: Open-Meteo for forecast, Nominatim for GPS place names.
    var url = 'https://nominatim.openstreetmap.org/reverse?format=json&zoom=10' +
        '&lat=' + encodeURIComponent(lat) +
        '&lon=' + encodeURIComponent(lon) +
        '&email=daybreakpebble@example.com';

    httpGetJson(url, function (json) {
        var name = extractReverseGeocodeName(json);
        if (!name) {
            onError('No reverse geocode results');
            return;
        }

        onSuccess({
            name: name,
            lat: lat,
            lon: lon
        });
    }, onError);
}

function fetchForecast(location, keys, log, sendWeatherDict) {
    var url = 'https://api.open-meteo.com/v1/forecast?latitude=' +
        encodeURIComponent(location.lat) +
        '&longitude=' + encodeURIComponent(location.lon) +
        '&timezone=auto&timeformat=unixtime&forecast_days=1' +
        '&current=temperature_2m,relative_humidity_2m,surface_pressure,wind_speed_10m,weather_code,is_day' +
        '&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset' +
        '&wind_speed_unit=kmh';

    log('Fetching weather for ' + location.name);

    httpGetJson(url, function (json) {
        // Guard against malformed/empty responses (e.g. API error bodies). Without
        // this a missing "current" block would be sent to the watch as a bogus
        // "0°, Clear sky" reading instead of a fetch error.
        if (!json || !json.current || json.current.temperature_2m === undefined ||
                json.current.temperature_2m === null ||
                json.current.weather_code === undefined || json.current.weather_code === null ||
                json.current.is_day === undefined || json.current.is_day === null) {
            log('Forecast response missing current data');
            sendFetchError(keys, sendWeatherDict, log);
            return;
        }
        endFetch();
        sendWeatherDict(buildWeatherPayload(json, location, keys));
    }, function (err) {
        log('Forecast fetch failed: ' + err);
        sendFetchError(keys, sendWeatherDict, log);
    });
}

function fetchWeather(options) {
    var settings = options.settings;
    var keys = options.keys;
    var log = options.log;
    var sendWeatherDict = options.sendWeatherDict;
    var lastLocation = options.lastLocation || null;
    var saveLastLocation = options.saveLastLocation || function () {};
    var lang = settings.lang || 'en';
    var city = (settings.location || '').trim();

    if (s_fetch_in_flight) {
        log('Weather fetch already in flight, skipping duplicate request');
        return;
    }
    beginFetch();

    // The configured city is only ever used once the user has typed one.
    function fetchForCity() {
        if (!city) {
            log('No location: GPS unavailable and no city configured');
            sendFetchError(keys, sendWeatherDict, log, ERROR_NO_LOCATION);
            return;
        }
        resolveCityLocation(city, lang, function (location) {
            fetchForecast(location, keys, log, sendWeatherDict);
        }, function (err) {
            log('City lookup failed: ' + err);
            var reason = (err === 'No geocoding results') ? ERROR_CITY_NOT_FOUND : ERROR_FETCH_FAILED;
            sendFetchError(keys, sendWeatherDict, log, reason);
        });
    }

    if (!settings.autodetect) {
        fetchForCity();
        return;
    }

    navigator.geolocation.getCurrentPosition(
        function (pos) {
            var lat = pos.coords.latitude;
            var lon = pos.coords.longitude;
            var onLocation = function (location) {
                saveLastLocation(location);
                fetchForecast(location, keys, log, sendWeatherDict);
            };
            reverseGeocodeLocation(lat, lon, onLocation, function () {
                onLocation({ lat: lat, lon: lon, name: 'Local' });
            });
        },
        function () {
            // Prefer where the phone last was: the weather will be fresh, only
            // the position is unconfirmed, and the watch marks it as such.
            if (lastLocation) {
                log('GPS unavailable, using last known location ' + lastLocation.name);
                fetchForecast({
                    lat: lastLocation.lat,
                    lon: lastLocation.lon,
                    name: lastLocation.name,
                    unconfirmed: true
                }, keys, log, sendWeatherDict);
                return;
            }
            log('GPS unavailable and no last location, trying the configured city');
            fetchForCity();
        },
        { timeout: 15000, maximumAge: 60000 }
    );
}

module.exports = {
    ERROR_FETCH_FAILED: ERROR_FETCH_FAILED,
    ERROR_NO_LOCATION: ERROR_NO_LOCATION,
    ERROR_CITY_NOT_FOUND: ERROR_CITY_NOT_FOUND,
    fetchWeather: fetchWeather,
    buildWeatherPayload: buildWeatherPayload,
    getWeatherIcon: getWeatherIcon,
    extractReverseGeocodeName: extractReverseGeocodeName,
    getTimezoneDisplayName: getTimezoneDisplayName
};
