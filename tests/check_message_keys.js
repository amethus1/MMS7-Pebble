const fs = require('fs');
const path = require('path');

const packageJsonPath = path.join(__dirname, '..', 'package.json');
const keysHeaderPath = path.join(__dirname, '..', 'src', 'c', 'keys.h');

const pkg = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'));
const pkgKeys = pkg.pebble.messageKeys || {};
const header = fs.readFileSync(keysHeaderPath, 'utf8');

const cKeys = {};
const defineRegex = /^\s*#define\s+([A-Za-z0-9_]+)\s+([0-9]+)/gm;
let match;
while ((match = defineRegex.exec(header)) !== null) {
  cKeys[match[1]] = Number(match[2]);
}

const aliasToCKey = {
  hideCW: 'KEY_HIDE_CW',
  hideBluetooth: 'KEY_HIDE_BLUETOOTH',
  FETCH_TRIGGER: 'KEY_FETCH_TRIGGER'
};

const failures = [];

Object.keys(pkgKeys).forEach((pkgKey) => {
  const cKeyName = aliasToCKey[pkgKey] || pkgKey;
  if (!cKeyName.startsWith('KEY_')) {
    return;
  }

  if (!(cKeyName in cKeys)) {
    failures.push(`Missing C key define for package key '${pkgKey}' (expected '${cKeyName}')`);
    return;
  }

  const pkgVal = Number(pkgKeys[pkgKey]);
  const cVal = Number(cKeys[cKeyName]);
  if (pkgVal !== cVal) {
    failures.push(`Value mismatch for '${pkgKey}'/'${cKeyName}': package=${pkgVal}, keys.h=${cVal}`);
  }
});

// Every messageKey used on the settings page must exist in package.json,
// and every option value must be a plain number or string the watch can parse.
const config = require('../src/pkjs/config.js');
// Handled on the phone in settings.js; never sent to the watch.
const JS_ONLY_KEYS = new Set(['default_loc', 'autodetect_loc', 'lang_id']);
const seenKeys = new Map();
function walk(items) {
  items.forEach((item) => {
    if (item.type === 'section') { walk(item.items || []); return; }
    if (item.messageKey) {
      if (!(item.messageKey in pkgKeys) && !JS_ONLY_KEYS.has(item.messageKey)) {
        failures.push(`config.js uses messageKey '${item.messageKey}' which is not in package.json`);
      }
      // The same key may appear more than once only when the copies are
      // restricted to different watch capabilities.
      const caps = JSON.stringify(item.capabilities || []);
      const prev = seenKeys.get(item.messageKey);
      if (prev !== undefined && (prev === '[]' || caps === '[]' || prev === caps)) {
        failures.push(`config.js has two items for messageKey '${item.messageKey}' that can both be on the page`);
      }
      seenKeys.set(item.messageKey, caps);
    }
    (item.options || []).forEach((opt) => {
      if (typeof opt.value !== 'number' && typeof opt.value !== 'string') {
        failures.push(`config.js option '${opt.label}' under '${item.label}' has a non-scalar value`);
      }
    });
  });
}
walk(config);
['week_mode', 'about_toggle', 'reset_defaults', 'city'].forEach((id) => {
  if (!JSON.stringify(config).includes(`"id":"${id}"`)) {
    failures.push(`config.js is missing the item id '${id}' that custom-clay.js relies on`);
  }
});

if (failures.length > 0) {
  console.error('KEY CONTRACT CHECK FAILED');
  failures.forEach((f) => console.error(`- ${f}`));
  process.exit(1);
}

console.log('KEY CONTRACT CHECK PASSED');
