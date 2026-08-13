
extern int _spu_IRQCallback;
void _SpuCallback(int arg0);

typedef struct SpuTransferModePage {
    int callback;
    char reserved04[0x4BC4];
} SpuTransferModePage;

register SpuTransferModePage *g_SpuTransferModePage asm("$1");

int Spu_SetTransferMode(int arg0) {
    int old = _spu_IRQCallback;

    if (arg0 != old) {
        g_SpuTransferModePage = (SpuTransferModePage *)0x800A0000;
        g_SpuTransferModePage[-1].callback = arg0;
        _SpuCallback(arg0);
    }
    return old;
}
