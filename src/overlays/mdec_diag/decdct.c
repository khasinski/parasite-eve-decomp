/* ASSEMBLER: GNU */

#include "common.h"
#include "pe1/mdec_diag.h"
#include "pe1/psyq_callbacks.h"

#include "decdct_externs.h"

/*
 * The LIBPRESS DCT object as linked into the overlay: the DecDCT entry
 * points (DecDCTReset .. DecDCToutCallback), then the MDEC register layer
 * they sit on (reset, input and output DMA). Every function compiles with
 * GCC 2.7.2 and plain GNU as. The rest of the object follows in its own
 * units: the two busy waits (mdec_wait.c, whose global register variable
 * must precede every function of its unit) and the status read with the
 * timeout handler (mdec_status.c, which needs $a0 back).
 */

typedef unsigned int size_t;

typedef struct DecDCTEnv {
    unsigned char iqY[64];
    unsigned char iqC[64];
    short dct[64];
} DecDCTEnv;

void DecDCTReset(int mode) {
    if (mode == 0)
        ResetCallback();
    MdecDiag_Reset(mode);
}

static inline void CopyWords(long *dst, long *src, size_t count) {
    while (count--) {
        *dst++ = *src++;
    }
}

s32 *DecDCTGetEnv(DecDCTEnv *env) {
    CopyWords((long *)&env->iqY, (long *)&g_MdecQuantTableCommand[1], 16);
    CopyWords((long *)&env->iqC, (long *)&g_MdecQuantTableCommand[17], 16);
    CopyWords((long *)&env->dct, (long *)&g_MdecScaleTableCommand[1], 32);
    return (s32 *)env;
}

s32 *DecDCTPutEnv(s32 *source)
{
    register s32 *read = source;
    register s32 *write = g_MdecQuantTableLuma;
    int index;

    for (index = 15; index != -1; index--)
        *write++ = *read++;

    write = g_MdecQuantTableChroma;
    read = source + 16;
    for (index = 15; index != -1; index--)
        *write++ = *read++;

    MdecDiag_SendInput(g_MdecQuantTableCommand, 0x20);
    MdecDiag_SendInput(g_MdecScaleTableCommand, 0x20);
    return source;
}

unsigned short MdecDiag_ReadHalfword(unsigned short *value) {
    return *value;
}

void DecDCTin(s32 *arg0, s32 arg1) {
    if (arg1 & 1) {
        *arg0 &= ~0x08000000;
    } else {
        *arg0 |= 0x08000000;
    }

    if (arg1 & 2) {
        *arg0 |= 0x02000000;
    } else {
        *arg0 &= ~0x02000000;
    }

    MdecDiag_SendInput(arg0, *(u16 *)arg0);
}

void DecDCTout(s32 *buffer, u32 size) {
    MdecDiag_ReceiveOutput(buffer, size);
}

s32 DecDCTinSync(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = MdecDiag_WaitInputSync();
    } else {
        result = ((unsigned)MdecDiag_ReadStatus() >> 29) & 1;
    }
    return result;
}

s32 DecDCToutSync(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = MdecDiag_WaitOutputSync();
    } else {
        result = ((unsigned)MdecDiag_ReadStatus() >> 24) & 1;
    }
    return result;
}

void DecDCTinCallback(void *value) {
    DMACallback(0, value);
}

void DecDCToutCallback(void *value) {
    DMACallback(1, value);
}

void MdecDiag_Reset(int mode)
{
    switch (mode) {
    case 0:
        *g_MdecStatusRegPtr = 0x80000000;
        *g_MdecDma0ChcrPtr = 0;
        *g_MdecDma1ChcrPtr = 0;
        *g_MdecStatusRegPtr = 0x60000000;
        MdecDiag_SendInput(g_MdecQuantTableCommand, 0x20);
        MdecDiag_SendInput(g_MdecScaleTableCommand, 0x20);
        break;
    case 1:
        *g_MdecStatusRegPtr = 0x80000000;
        *g_MdecDma0ChcrPtr = 0;
        *g_MdecDma1ChcrPtr = 0;
        (void)*g_MdecDma1ChcrPtr;
        *g_MdecStatusRegPtr = 0x60000000;
        break;
    default:
        printf(g_MdecResetBadOptionFormat, mode);
    }
}

void MdecDiag_SendInput(s32 *data, u32 size)
{
    MdecDiag_WaitInputSync();
    *g_MdecDmaControlRegPtr |= 0x88;
    *g_MdecDma0MadrPtr = data + 1;
    *g_MdecDma0BcrPtr = ((size >> 5) << 16) | 0x20;
    *g_MdecCommandRegPtr = *data;
    *g_MdecDma0ChcrPtr = 0x1000201;
}

void MdecDiag_ReceiveOutput(s32 *data, u32 size)
{
    MdecDiag_WaitOutputSync();
    *g_MdecDmaControlRegPtr |= 0x88;
    *g_MdecDma1ChcrPtr = 0;
    *g_MdecDma1MadrPtr = data;
    *g_MdecDma1BcrPtr = ((size >> 5) << 16) | 0x20;
    *g_MdecDma1ChcrPtr = 0x1000200;
}

