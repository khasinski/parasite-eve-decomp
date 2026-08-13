#include "common.h"

extern volatile u8 *D_8009B27C;
extern u8 D_8009B295;
extern u8 D_800A3460[];
extern u8 D_800A3468[];
extern void (*D_8009AFB4)(u8, u8 *);
extern void (*D_8009AFB8)(u8, u8 *);

typedef struct CdIntrDataPage {
    u8 pad_0000[0x3460];
    u8 syncResult[8];
    u8 readyResult[8];
} CdIntrDataPage;

register CdIntrDataPage *g_CdIntrDataPage asm("$5");

int getintr(void);

void Cd_SetIntrMask(void) {
    u8 savedMask;
    u8 *readyStatus;
    u8 *syncStatus;
    u8 status;
    int intr;

    readyStatus = &D_8009B295;
    syncStatus = readyStatus - 1;
    savedMask = *D_8009B27C & 3;

    while ((intr = getintr()) != 0) {
        if (intr & 4) {
            if (D_8009AFB8 != 0) {
                g_CdIntrDataPage = (CdIntrDataPage *)0x800A0000;
                status = *readyStatus;
                D_8009AFB8(status, g_CdIntrDataPage->readyResult);
            }
        }

        if (intr & 2) {
            if (D_8009AFB4 != 0) {
                g_CdIntrDataPage = (CdIntrDataPage *)0x800A0000;
                status = *syncStatus;
                D_8009AFB4(status, g_CdIntrDataPage->syncResult);
            }
        }
    }

    *D_8009B27C = savedMask;
}
