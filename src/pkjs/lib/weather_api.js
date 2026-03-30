var s_fetch_in_flight = false;

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
    if (code === 0) return isDay ? 'I' : 'N';
    if (code === 1) return isDay ? '"' : '#';
    if (code === 2) return isDay ? '"' : '#';
    if (code === 3) return '!';
    if (code === 45 || code === 48) return 'M';
    if ((code >= 51 && code <= 57)) return "'";
    if ((code >= 61 && code <= 67) || (code >= 80 && code <= 82)) return '$';
    if ((code >= 71 && code <= 77) || (code >= 85 && code <= 86)) return '*';
    if (code >= 95 && code <= 99) return isDay ? 'G' : 'F';
    return 'I';
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
    var current = json.current || {};
    var daily = json.daily || {};
    var now = nowUnix || Math.floor(Date.now() / 1000);
    var weatherCode = Number(current.weather_code || 0);
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

    return dict;
}

function sendFetchError(keys, sendWeatherDict, log) {
    s_fetch_in_flight = false;
    var dict = {};
    dict[keys.KEY_WEATHER_FETCH_ERROR] = 1;
    sendWeatherDict(dict);
    log('Sent fetch error to watch');
}

function resolveCityLocation(city, lang, onSuccess, onError) {
    var url = 'https://geocoding-api.open-meteo.com/v1/search?count=1&format=json&language=' +
        encodeURIComponent(lang) + '&name=' + encodeURIComponent(city);

    httpGetJson(url, function (json) {
        if (!json.results || !json.results.length) {
            onError('No geocoding results');
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
        s_fetch_in_flight = false;
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
    var lang = settings.lang || 'en';
    var fallbackCity = (settings.location || '').trim() || 'Berlin';

    if (s_fetch_in_flight) {
        log('Weather fetch already in flight, skipping duplicate request');
        return;
    }
    s_fetch_in_flight = true;

    if (settings.autodetect) {
        navigator.geolocation.getCurrentPosition(
            function (pos) {
                reverseGeocodeLocation(pos.coords.latitude, pos.coords.longitude, function (location) {
                    fetchForecast(location, keys, log, sendWeatherDict);
                }, function () {
                    fetchForecast({
                        lat: pos.coords.latitude,
                        lon: pos.coords.longitude,
                        name: 'Local'
                    }, keys, log, sendWeatherDict);
                });
            },
            function () {
                log('GPS error, falling back to city');
                resolveCityLocation(fallbackCity, lang, function (location) {
                    fetchForecast(location, keys, log, sendWeatherDict);
                }, function (err) {
                    log('City lookup failed: ' + err);
                    sendFetchError(keys, sendWeatherDict, log);
                });
            },
            { timeout: 15000, maximumAge: 60000 }
        );
        return;
    }

    resolveCityLocation(fallbackCity, lang, function (location) {
        fetchForecast(location, keys, log, sendWeatherDict);
    }, function (err) {
        log('City lookup failed: ' + err);
        sendFetchError(keys, sendWeatherDict, log);
    });
}

module.exports = {
    fetchWeather: fetchWeather,
    buildWeatherPayload: buildWeatherPayload,
    extractReverseGeocodeName: extractReverseGeocodeName,
    getTimezoneDisplayName: getTimezoneDisplayName
};
