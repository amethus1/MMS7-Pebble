const assert = require('assert');

function makeLocalStorage() {
  const store = {};
  return {
    getItem(key) {
      return Object.prototype.hasOwnProperty.call(store, key) ? store[key] : null;
    },
    setItem(key, value) {
      store[key] = String(value);
    },
    removeItem(key) {
      delete store[key];
    }
  };
}

global.localStorage = makeLocalStorage();

const settings = require('../src/pkjs/lib/settings');

const logs = [];
function log(msg) {
  logs.push(msg);
}

const keys = {
  KEY_SET_SPEED_UNIT: 60,
  KEY_SET_PRESSURE_UNIT: 61
};

let sentPayloads = [];
function sendFn(dict, okCb) {
  sentPayloads.push(dict);
  if (okCb) okCb();
}

localStorage.setItem('mss-js-settings', JSON.stringify({
  speedUnit: '1',
  pressureUnit: '2'
}));

settings.migrateLegacyUnitSettings(keys, sendFn, log);
assert.strictEqual(sentPayloads.length, 1, 'Migration should send one message');
assert.strictEqual(sentPayloads[0][keys.KEY_SET_SPEED_UNIT], 1, 'Speed unit should migrate');
assert.strictEqual(sentPayloads[0][keys.KEY_SET_PRESSURE_UNIT], 2, 'Pressure unit should migrate');

settings.migrateLegacyUnitSettings(keys, sendFn, log);
assert.strictEqual(sentPayloads.length, 1, 'Migration should run once');

settings.applyJsOnlySettings({
  default_loc: 'Boston',
  autodetect_loc: 0,
  lang_id: 'de'
});

settings.save(log);
settings.load(log);
const current = settings.get();

assert.strictEqual(current.location, 'Boston', 'Location should update');
assert.strictEqual(current.autodetect, false, 'Autodetect should update');
assert.strictEqual(current.lang, 'de', 'Language should update');

assert.strictEqual(settings.getLastLocation(), null, 'No last location until one is saved');
settings.saveLastLocation({ name: 'Chicago', lat: 41.88, lon: -87.63 }, log);
const last = settings.getLastLocation();
assert.strictEqual(last.name, 'Chicago', 'Last location name round-trips');
assert.strictEqual(last.lat, 41.88, 'Last location latitude round-trips');
assert.ok(last.at > 0, 'Last location records when it was saved');
settings.saveLastLocation({ name: 'bad' }, log);
assert.strictEqual(settings.getLastLocation().name, 'Chicago', 'Invalid locations are ignored');

console.log('ALL TESTS PASSED');
