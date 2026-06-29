typedef void (*SpuCallback)(void);

extern void DMACallback(int event, SpuCallback callback);

void _SpuDataCallback(SpuCallback callback) {
    DMACallback(4, callback);
}
