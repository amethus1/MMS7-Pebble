var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });
var keys = require('message_keys');

var settings = require('./lib/settings');
var weatherApi = require('./lib/weather_api');
var messaging = require('./lib/messaging');

function log(msg) {
    console.log('[MSS] ' + msg);
}

function sendWeather(dict) {
    messaging.sendAppMessage(dict,
        function () { log('Weather sent'); },
        function (e) { log('Weather send failed: ' + JSON.stringify(e)); }
    );
}

function fetchWeather() {
    settings.load(log);

    weatherApi.fetchWeather({
        settings: settings.get(),
        keys: keys,
        log: log,
        sendWeatherDict: sendWeather
    });
}

Pebble.addEventListener('ready', function () {
    log('JS ready');
    settings.load(log);

    settings.migrateLegacyUnitSettings(keys, messaging.sendAppMessage, log);
});

Pebble.addEventListener('showConfiguration', function () {
    Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function (e) {
    if (!e || !e.response || e.response === 'CANCELLED') {
        log('Config cancelled');
        return;
    }

    var rawSettings = clay.getSettings(e.response, false);
    var dict = settings.filterNumericAppMessageKeys(clay.getSettings(e.response));

    settings.applyJsOnlySettings(rawSettings);
    settings.save(log);

    log('Sending watch settings: ' + JSON.stringify(dict));

    messaging.sendAppMessage(dict,
        function () {
            log('Settings sent');
            setTimeout(fetchWeather, 1000);
        },
        function (err) {
            log('Settings send failed: ' + JSON.stringify(err));
            setTimeout(fetchWeather, 1000);
        }
    );
});

Pebble.addEventListener('appmessage', function () {
    fetchWeather();
});
