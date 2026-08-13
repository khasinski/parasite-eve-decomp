#include "pe1/psyq_spu_internal.h"

extern void EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern s32 OpenEvent(s32, s32, s32, void *);
extern void EnableEvent(s32);
extern void _SpuDataCallback(SpuCallback callback);

typedef struct SpuStartDataPage {
    s32 event;
    char reserved04[0x64];
    s32 called;
    char reserved6C[0x4C10];
} SpuStartDataPage;

typedef struct SpuCallbackPage {
    char reserved00[0x29EC];
} SpuCallbackPage;

register SpuStartDataPage *g_SpuStartDataPage asm("$1");
register SpuCallbackPage *g_SpuStartCallbackPage asm("$4");

void SpuStart(void) {
    register s32 event asm("$4");
    register s32 called asm("$2");

    if (_spu_isCalled == 0) {
        called = 1;
        g_SpuStartDataPage = (SpuStartDataPage *)0x800A0000;
        g_SpuStartDataPage[-1].called = called;
        EnterCriticalSection();
        g_SpuStartCallbackPage = (SpuCallbackPage *)0x80080000;
        _SpuDataCallback((SpuCallback)&g_SpuStartCallbackPage[-1]);
        event = OpenEvent(0xF0000009, 0x20, 0x2000, 0);
        g_SpuStartDataPage = (SpuStartDataPage *)0x800A0000;
        g_SpuStartDataPage[-1].event = event;
        EnableEvent(event);
        ExitCriticalSection();
    }
}
