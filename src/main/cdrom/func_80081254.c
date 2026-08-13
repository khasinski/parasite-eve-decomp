
extern int D_8009B6D0;

typedef struct CdCallbackDataPage {
    int callback;
    char reserved04[0x492C];
} CdCallbackDataPage;

register CdCallbackDataPage *g_CdCallbackWritePage asm("$1");

int func_80081254(int callback) {
    int old;

    old = D_8009B6D0;
    g_CdCallbackWritePage = (CdCallbackDataPage *)0x800A0000;
    g_CdCallbackWritePage[-1].callback = callback;
    return old;
}
