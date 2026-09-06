const assert = require('assert');

const weatherApi = require('../src/pkjs/lib/weather_api');
const keys = require('../package.json').pebble.messageKeys;

const json = {
  current: {
    time: 1700000000,
    temperature_2m: 12.3,
    relative_humidity_2m: 82,
    surface_pressure: 1032,
    wind_speed_10m: 4.2,
    weather_code: 2,
    is_day: 0
  },
  daily: {
    temperature_2m_max: [16.1],
    temperature_2m_min: [7.4],
    sunrise: [1700010000],
    sunset: [1700049600]
  },
  timezone: 'Europe/Berlin',
  timezone_abbreviation: 'CET',
  utc_offset_seconds: 3600
};

const location = {
  name: 'ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890',
  lat: 52.519444,
  lon: 13.406667
};

const payload = weatherApi.buildWeatherPayload(json, location, keys, 1700001234);

assert.strictEqual(payload[keys.KEY_WEATHER_TEMP], 12, 'Temp should round');
assert.strictEqual(payload[keys.KEY_WEATHER_HIGH], 16, 'High should map from daily max');
assert.strictEqual(payload[keys.KEY_WEATHER_LOW], 7, 'Low should map from daily min');
assert.strictEqual(payload[keys.KEY_WEATHER_HUMIDITY], 82, 'Humidity should map');
assert.strictEqual(payload[keys.KEY_WEATHER_PRESSURE], 1032, 'Pressure should map');
assert.strictEqual(payload[keys.KEY_WEATHER_WIND_KMH], 4, 'Wind should round in km/h');
assert.strictEqual(payload[keys.KEY_LOCATION_LAT], 52519444, 'Latitude should be stored in microdegrees');
assert.strictEqual(payload[keys.KEY_LOCATION_LON], 13406667, 'Longitude should be stored in microdegrees');
assert.strictEqual(payload[keys.KEY_LOCATION_NAME].length <= 31, true, 'Location should be truncated to 31 chars');
assert.strictEqual(payload[keys.KEY_TIME_ZONE_NAME], 'CET', 'Timezone abbreviation should come from the forecast payload');
assert.strictEqual(payload[keys.KEY_TIME_UTC_OFFSET], 3600, 'UTC offset should be included for remote time formatting');
assert.strictEqual(payload[keys.KEY_SUN_RISE_UNIX], 1700010000, 'Sunrise should map from the daily payload');
assert.strictEqual(payload[keys.KEY_SUN_SET_UNIX], 1700049600, 'Sunset should map from the daily payload');
assert.strictEqual(payload[keys.KEY_WEATHER_CONDITIONS], 'Partly cloudy', 'Conditions should map from Open-Meteo weather codes');

const expectedIcons = [
  [[0], 'I', 'N'],
  [[1, 2], '"', '#'],
  [[3], '!', '!'],
  [[45, 48], '=', '>'],
  [[51, 53, 55], '.', '/'],
  [[56, 57], '1', '2'],
  [[61, 63, 65], '%', '&'],
  [[66, 67], '1', '2'],
  [[71, 73, 75], ':', ';'],
  [[77], '7', '8'],
  [[80, 81], '(', ')'],
  [[82], '+', ','],
  [[85], '7', '8'],
  [[86], ':', ';'],
  [[95], 'G', 'H'],
  [[96, 99], '4', '5']
];

expectedIcons.forEach(function (entry) {
  entry[0].forEach(function (code) {
    assert.strictEqual(weatherApi.getWeatherIcon(code, true), entry[1], 'Wrong day icon for WMO code ' + code);
    assert.strictEqual(weatherApi.getWeatherIcon(code, false), entry[2], 'Wrong night icon for WMO code ' + code);
  });
});
assert.strictEqual(weatherApi.getWeatherIcon(-1, true), '!', 'Unknown weather should not display as clear sky');

const iconPayload = weatherApi.buildWeatherPayload({
  current: {
    time: 1700000000,
    temperature_2m: 0,
    weather_code: 95,
    is_day: 0
  },
  daily: {}
}, { name: 'Test' }, keys, 1700001234);
assert.strictEqual(
  iconPayload[keys.KEY_WEATHER_ICON],
  'H'.charCodeAt(0),
  'Payload should send the mapped night thunderstorm glyph'
);

const payloadWithoutName = weatherApi.buildWeatherPayload(json, { lat: 52.519444, lon: 13.406667 }, keys, 1700001234);
assert.strictEqual(payloadWithoutName[keys.KEY_LOCATION_NAME], 'Local', 'Fallback location should be generic rather than a wrong city');

assert.strictEqual(
  weatherApi.extractReverseGeocodeName({ address: { city: 'Evanston' } }),
  'Evanston',
  'Reverse geocode helper should prefer city-like address fields'
);

assert.strictEqual(
  weatherApi.getTimezoneDisplayName('America/Chicago', 1772863200, 'GMT-6'),
  'CST',
  'Timezone display should prefer a real short abbreviation over GMT offset labels'
);

console.log('ALL TESTS PASSED');
