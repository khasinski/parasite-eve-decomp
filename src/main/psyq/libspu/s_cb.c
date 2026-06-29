typedef void (*SpuCallback)(void);

extern void InterruptCallback(int event, SpuCallback callback);

void _SpuCallback(SpuCallback callback) {
    InterruptCallback(9, callback);
}
