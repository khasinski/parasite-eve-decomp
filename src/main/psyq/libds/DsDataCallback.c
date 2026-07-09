int DMACallback(int channel, int callback);

int DsDataCallback(int callback) {
    return DMACallback(3, callback);
}
