// Runs inside the Clay settings page (injected with .toString(), so no
// require() here). Shows and hides items that only make sense in combination
// with another setting, drives the week-number choice, and wires the About
// and Reset buttons.
module.exports = function (minified) {
    var clayConfig = this;

    function byKey(key) { return clayConfig.getItemByMessageKey(key); }
    function byId(id) { return clayConfig.getItemById(id); }

    function setVisible(item, visible) {
        if (!item) return;
        if (visible) item.show(); else item.hide();
    }

    function intValue(item, fallback) {
        if (!item) return fallback;
        var v = parseInt(item.get(), 10);
        return isNaN(v) ? fallback : v;
    }

    // --- Week number: one visible choice, two hidden toggles the watch reads ---
    function weekModeFromToggles() {
        var hidden = byKey('hideCW');
        var inDate = byKey('KEY_SET_WEEK_IN_DATE');
        if (hidden && hidden.get()) return 0;
        if (inDate && inDate.get()) return 2;
        return 1;
    }

    function applyWeekMode() {
        var mode = intValue(byId('week_mode'), 1);
        var hidden = byKey('hideCW');
        var inDate = byKey('KEY_SET_WEEK_IN_DATE');
        if (hidden) hidden.set(mode === 0);
        if (inDate) inDate.set(mode === 2);
        setVisible(byKey('KEY_SET_WEEK_TYPE'), mode !== 0);
        applyFiscal();
    }

    function applyFiscal() {
        var mode = intValue(byId('week_mode'), 1);
        var fiscal = intValue(byKey('KEY_SET_WEEK_TYPE'), 0) === 1;
        clayConfig.getItemsByGroup('fiscal').forEach(function (item) {
            setVisible(item, mode !== 0 && fiscal);
        });
    }

    // --- Extra information: timezone format only matters for Timezone ---
    function applyExtraInfo() {
        var showTimezone = intValue(byKey('KEY_SET_HEALTH'), 5) === 5;
        setVisible(byKey('KEY_SET_TZ_FORMAT'), showTimezone);
    }

    // --- Weather location: the city is a fallback in GPS mode, the source otherwise ---
    function applyLocation() {
        var city = byId('city');
        if (!city) return;
        var gps = intValue(byKey('autodetect_loc'), 1) === 1;
        try {
            var $desc = city.$element.select('.description');
            if ($desc && $desc.set) {
                $desc.set('innerHTML', gps
                    ? 'Used when the phone\'s location is unavailable'
                    : 'Weather is fetched for this city');
            }
        } catch (e) {
            // Cosmetic only; never let it break the rest of the page setup.
        }
    }

    // --- About ---
    function applyAbout(show) {
        clayConfig.getItemsByGroup('about').forEach(function (item) {
            setVisible(item, show);
        });
    }

    // --- Reset ---
    var SKIP_TYPES = { heading: 1, text: 1, button: 1, submit: 1, section: 1 };
    function resetToDefaults() {
        clayConfig.getAllItems().forEach(function (item) {
            var cfg = item.config || {};
            if (SKIP_TYPES[cfg.type]) return;
            if (cfg.defaultValue === undefined) return;
            item.set(cfg.defaultValue);
        });
        applyWeekMode();
        applyExtraInfo();
        applyLocation();
    }

    clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function () {
        // The two toggles exist only to carry the watch-side keys.
        setVisible(byKey('hideCW'), false);
        setVisible(byKey('KEY_SET_WEEK_IN_DATE'), false);

        var weekMode = byId('week_mode');
        if (weekMode) {
            weekMode.set(weekModeFromToggles());
            weekMode.on('change', applyWeekMode);
        }
        var weekType = byKey('KEY_SET_WEEK_TYPE');
        if (weekType) weekType.on('change', applyFiscal);

        var health = byKey('KEY_SET_HEALTH');
        if (health) health.on('change', applyExtraInfo);

        var locationMode = byKey('autodetect_loc');
        if (locationMode) locationMode.on('change', applyLocation);

        var aboutShown = false;
        var aboutButton = byId('about_toggle');
        if (aboutButton) {
            aboutButton.on('click', function () {
                aboutShown = !aboutShown;
                applyAbout(aboutShown);
                aboutButton.set(aboutShown ? 'Hide the guide' : 'What do the small things mean?');
            });
        }

        var resetButton = byId('reset_defaults');
        if (resetButton) resetButton.on('click', resetToDefaults);

        applyWeekMode();
        applyExtraInfo();
        applyLocation();
        applyAbout(false);
    });
};
