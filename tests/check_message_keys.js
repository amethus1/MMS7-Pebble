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

// KEY_SET_CUSTOM_<part>, one per colour picker; not the icon toggle
function isCustomColourKey(name) {
  return /^KEY_SET_CUSTOM_/.test(name) && name !== 'KEY_SET_CUSTOM_ICON_AUTO';
}

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
// The custom colour pickers must mirror src/c/modules/custom_colors.h: same
// order, same defaults. Otherwise the page would show one colour while the
// watch, before the first Save, uses another.
const customHeader = fs.readFileSync(path.join(__dirname, '..', 'src', 'c', 'modules', 'custom_colors.h'), 'utf8');
const enumNames = [];
const enumBody = customHeader.slice(customHeader.indexOf('typedef enum'), customHeader.indexOf('CUSTOM_COLOR_COUNT'));
enumBody.replace(/CUSTOM_COLOR_([A-Z_]+)/g, (m, name) => { enumNames.push(name); return m; });
const defaultsBody = customHeader.slice(customHeader.indexOf('#define CUSTOM_COLOR_DEFAULTS'));
const headerDefaults = (defaultsBody.match(/0x[0-9A-Fa-f]{6}/g) || []).map((h) => h.slice(2).toUpperCase());
const pickers = [];
(function collect(items) {
  items.forEach((item) => {
    if (item.type === 'section') { collect(item.items || []); return; }
    if (item.type === 'color') pickers.push(item);
  });
})(config);
if (pickers.length !== enumNames.length || headerDefaults.length !== enumNames.length) {
  failures.push(`custom colours: ${enumNames.length} in the enum, ${headerDefaults.length} defaults, ${pickers.length} pickers on the settings page`);
} else {
  enumNames.forEach((name, i) => {
    if (pickers[i].messageKey !== `KEY_SET_CUSTOM_${name}`) {
      failures.push(`custom colour ${i}: page has '${pickers[i].messageKey}', watch expects KEY_SET_CUSTOM_${name}`);
    }
    if (String(pickers[i].defaultValue).toUpperCase() !== headerDefaults[i]) {
      failures.push(`custom colour ${name}: page default ${pickers[i].defaultValue}, watch default ${headerDefaults[i]}`);
    }
    if (pkgKeys[pickers[i].messageKey] !== pkgKeys.KEY_SET_CUSTOM_BACKGROUND + i ||
        cKeys[`KEY_SET_CUSTOM_${name}`] !== cKeys.KEY_SET_CUSTOM_BACKGROUND + i) {
      failures.push(`custom colour ${name}: message keys must be consecutive from KEY_SET_CUSTOM_BACKGROUND`);
    }
  });
}
const schemeSelect = [];
(function collect(items) {
  items.forEach((item) => {
    if (item.type === 'section') { collect(item.items || []); return; }
    if (item.messageKey === 'KEY_SET_INVERT_COLOR') schemeSelect.push(item);
  });
})(config);
const customIndex = Number((customHeader.match(/#define COLOR_PROFILE_CUSTOM\s+(\d+)/) || [])[1]);
if (!schemeSelect.some((sel) => (sel.options || []).some((o) => o.value === customIndex))) {
  failures.push(`config.js has no colour scheme option with value ${customIndex} (COLOR_PROFILE_CUSTOM)`);
}
const customClay = fs.readFileSync(path.join(__dirname, '..', 'src', 'pkjs', 'custom-clay.js'), 'utf8');
if (!new RegExp(`=== ${customIndex}\\b`).test(customClay)) {
  failures.push(`custom-clay.js does not show the colour pickers for scheme ${customIndex}`);
}

['week_mode', 'about_toggle', 'reset_defaults', 'city'].forEach((id) => {
  if (!JSON.stringify(config).includes(`"id":"${id}"`)) {
    failures.push(`config.js is missing the item id '${id}' that custom-clay.js relies on`);
  }
});

// Every setting key the phone can send must be handled on the watch; a
// key that is only sent (or only persisted) silently does nothing.
const settingsC = fs.readFileSync(path.join(__dirname, '..', 'src', 'c', 'state', 'settings.c'), 'utf8');
Object.keys(pkgKeys).forEach((pkgKey) => {
  const cKeyName = aliasToCKey[pkgKey] || pkgKey;
  const isSetting = cKeyName.startsWith('KEY_SET_') || cKeyName.startsWith('KEY_HIDE_');
  if (!isSetting) return;
  // The custom colour keys are handled as one consecutive range rather than a
  // case each. The checks further down pin their order and numbering, and
  // test_settings.c sends, stores and reloads every one of them.
  if (isCustomColourKey(cKeyName)) {
    if (!/t->key >= KEY_SET_CUSTOM_BACKGROUND && t->key < KEY_SET_CUSTOM_BACKGROUND \+ CUSTOM_COLOR_COUNT/.test(settingsC) ||
        !/persist_write_int\(KEY_SET_CUSTOM_BACKGROUND \+ i,/.test(settingsC)) {
      failures.push(`settings.c no longer handles the custom colour key range (${cKeyName})`);
    }
    return;
  }
  if (!new RegExp(`case\\s+${cKeyName}\\s*:`).test(settingsC)) {
    failures.push(`settings.c has no 'case ${cKeyName}:' - the watch would ignore this setting`);
  }
  if (!new RegExp(`persist_write_(int|string)\\(${cKeyName},`).test(settingsC)) {
    failures.push(`settings.c never persists ${cKeyName} - the setting would be lost on restart`);
  }
});

if (failures.length > 0) {
  console.error('KEY CONTRACT CHECK FAILED');
  failures.forEach((f) => console.error(`- ${f}`));
  process.exit(1);
}

console.log('KEY CONTRACT CHECK PASSED');
