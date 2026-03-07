function sendAppMessage(dict, onSuccess, onFailure) {
    Pebble.sendAppMessage(dict, onSuccess, onFailure);
}

module.exports = {
    sendAppMessage: sendAppMessage
};
