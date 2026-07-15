void DMACallback(int channel, int callback);

void CdDataCallback(int callback) {
    DMACallback(3, callback);
}
