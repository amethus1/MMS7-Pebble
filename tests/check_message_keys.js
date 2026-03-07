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

if (failures.length > 0) {
  console.error('KEY CONTRACT CHECK FAILED');
  failures.forEach((f) => console.error(`- ${f}`));
  process.exit(1);
}

console.log('KEY CONTRACT CHECK PASSED');
