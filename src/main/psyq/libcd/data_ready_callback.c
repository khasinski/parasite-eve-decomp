
#include "pe1/psyq_cd.h"

typedef void (*Callback)(void);

extern int g_CdStreamRingReadSlot;
extern int D_800BE998;
extern int g_CdStreamDataReadyFlag;
extern unsigned char *g_CdRingBufPtr;
extern CdlLOC D_800A3490;
extern Callback g_StrDataReadyCallback;

typedef struct CdStreamReadyDataPage {
    char reserved00[0x3494];
    int sector_data;
} CdStreamReadyDataPage;

typedef struct CdStreamRingReadPage {
    int slot;
    char reserved04[0x1618];
} CdStreamRingReadPage;

register void *g_CdStreamDataPage asm("$1");

void data_ready_callback(void) {
    unsigned char *base = g_CdRingBufPtr + (g_CdStreamRingReadSlot << 5);
    int sector_data;
    register int next_slot asm("$3");
    register Callback callback asm("$4");

    *(short *)base = 2;
    __builtin_memcpy(&D_800A3490, base + 0x1C, 4);
    sector_data = *(int *)(base + 8);
    next_slot = D_800BE998;
    callback = g_StrDataReadyCallback;
    g_CdStreamDataPage = (void *)0x800A0000;
    ((CdStreamReadyDataPage *)g_CdStreamDataPage)->sector_data = sector_data;
    g_CdStreamDataPage = (void *)0x800C0000;
    ((CdStreamRingReadPage *)g_CdStreamDataPage)[-1].slot = next_slot;
    if (callback != 0) {
        callback();
    }
    g_CdStreamDataReadyFlag = 0;
}
