#include "common.h"

extern volatile u8 *g_CdRegIndexBase;
extern volatile u8 *g_CdRegResponse;
extern volatile u32 *g_CdRegRequest;
extern volatile u32 *D_8009B2B0;
extern volatile u32 *D_8009B2B4;
extern volatile u32 *D_8009B2B8;
extern volatile u32 *D_8009B2BC;
extern volatile u32 *g_CdRegDmaControl;

extern int D_8009B260;

typedef struct CdGetSectorFrame {
    volatile u32 readback;
    u32 pad04;
} CdGetSectorFrame;

typedef union CdRegisterValue {
    u32 word;
    volatile u8 *bytePtr;
    volatile u32 *wordPtr;
} CdRegisterValue;

register CdGetSectorFrame *g_CdGetSectorFrame asm("$29");
register u32 g_CdGetSectorResult asm("$2");
register u8 g_CdGetSectorStatus asm("$2");

int CD_getsector(u32 arg0, u32 arg1) {
    register u32 constant asm("$6");
    CdRegisterValue io;
    CdRegisterValue address;

    g_CdGetSectorFrame--;

    address.bytePtr = g_CdRegIndexBase;
    g_CdGetSectorResult = address.word;
    *(volatile u8 *)g_CdGetSectorResult = 0;
    g_CdGetSectorResult = 0x80;
    io.bytePtr = g_CdRegResponse;
    *io.bytePtr = g_CdGetSectorResult;
    address.wordPtr = D_8009B2B0;
    g_CdGetSectorResult = address.word;
    constant = 0x21020843;
    *(volatile u32 *)g_CdGetSectorResult = constant;
    g_CdGetSectorResult = 0x1325;
    io.wordPtr = g_CdRegRequest;
    *io.wordPtr = g_CdGetSectorResult;
    io.wordPtr = D_8009B2B4;
    g_CdGetSectorResult = *io.wordPtr;
    g_CdGetSectorResult |= 0x8000;
    *io.wordPtr = g_CdGetSectorResult;
    address.wordPtr = D_8009B2B8;
    g_CdGetSectorResult = address.word;
    *(volatile u32 *)g_CdGetSectorResult = arg0;
    g_CdGetSectorResult = 0x10000;
    io.wordPtr = D_8009B2BC;
    *io.wordPtr = arg1 | g_CdGetSectorResult;

    io.bytePtr = g_CdRegIndexBase;
    g_CdGetSectorStatus = *io.bytePtr;
    g_CdGetSectorResult &= 0x40;
    if (g_CdGetSectorResult == 0) {
        do {
            g_CdGetSectorStatus = *io.bytePtr;
            g_CdGetSectorResult &= 0x40;
        } while (g_CdGetSectorResult == 0);
    }

    io.word = 0x11400100;
    address.wordPtr = g_CdRegDmaControl;
    g_CdGetSectorResult = address.word;
    *(volatile u32 *)g_CdGetSectorResult = io.word;
    address.wordPtr = g_CdRegDmaControl;
    g_CdGetSectorResult = address.word;
    g_CdGetSectorResult = *(volatile u32 *)g_CdGetSectorResult;
    g_CdGetSectorFrame->readback = g_CdGetSectorResult;

    g_CdGetSectorResult = 0;
    g_CdGetSectorFrame++;
    return g_CdGetSectorResult;
}
