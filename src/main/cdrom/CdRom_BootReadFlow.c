#include "pe1/cdrom.h"
#include "pe1/cdrom_buffers.h"

int CdRom_ReadSectorsFromLba(u32 lba, void *destination, u32 size) {
    return CdRom_ReadSectors(lba, 0, destination, size);
}
#include "pe1/psyq_cd.h"

void exit(int code);
CdlLOC *CdIntToPos(int i, CdlLOC *p);
int printf(char *fmt, ...);

extern int g_GameState;
extern u_short g_CdDiskType;
extern char D_8001136C[];

int CdRom_ReadSectors(u32 lba, u32 offset, void *destination, u32 size) {
    register int base;
    register int rel;
    register int dst_reg;
    register int size_reg;
    int *state;
    CdlLOC loc;
    int ret;

    base = lba;
    rel = offset;
    dst_reg = (int)destination;
    size_reg = size;
    state = &g_GameState;

    if ((*state & 0x1000000) != 0) {
        return -1;
    }
    if (Cd_GetReadyStatus() != 1) {
        return -1;
    }
    if (CdRom_GetPendingReadCount() != 0) {
        return -1;
    }
    if (CdRom_GetDiskType() != g_CdDiskType) {
        exit(1);
    }

    *state |= 0x1004000;
    base += rel;
    CdIntToPos(base, &loc);
    ret = CdRom_StartRead(&loc, size_reg, (void *)dst_reg, 0x80);
    if (ret != 0) {
        return ret;
    }

    *state &= 0xFEFFBFFF;
    printf(D_8001136C, base, size_reg);
    return -1;
}
int CdRom_PollReady(void) {
    int scratch;
    int status;
    int *state;

    status = Sys_VSyncTimeout(&scratch);
    if ((unsigned int)(status + 1) < 2U) {
        state = &g_GameState;
        *state &= 0xFEFFBFFF;
    }
    return status;
}
#include "common.h"
#include "include_asm.h"

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/psyq_gpu.h"

s32 Akao_Cmd_F0();
s32 VSync();
s32 EnterCriticalSection();
s32 ExitCriticalSection();
s32 FlushCache();

void SetDefDispEnv(void *env, int x, int y, int w, int h);

extern s8 D_800B0DB2;
extern s8 D_800B0DB3;
extern s8 D_800B0DB4;
extern s8 D_800B0DB5;
extern s8 D_800B0DB6;
extern s8 D_800B0DB7;
extern s32 g_PeImageBaseLba;
extern u16 g_StrFileDirLba[];

s32 SetDispMask(s32 arg0);

s32 Overlay_LoadInitialImage(void) {
    register s32 v0 asm("$2");
    s32 v1;
    u16 *range;
    s32 sp30;
    DISPENV sp18;

    D_800B0DB5 = -1;
    D_800B0DB4 = -1;
    D_800B0DB7 = -1;
    D_800B0DB6 = -1;
    D_800B0DB3 = -1;
    D_800B0DB2 = -1;
    g_GameState &= 0xFFFFFF0F;
    Akao_Cmd_F0();

restart:
    range = g_StrFileDirLba;
    do {
        v1 = CdRom_ReadSectors(g_PeImageBaseLba + range[0], 0, g_StrFileDirBuffer, range[1] - range[0]);
    } while (v1 == -1);

    while (1) {
        v1 = Sys_VSyncTimeout(&sp30);
        v0 = v1;
        v0 = (v1 + 1);
        if ((u32)v0 < 2U) {
            g_GameState &= 0xFEFFBFFF;
        }
        v0 = v1;
        if (v0 == 0) {
            break;
        }
        if (v0 == -1) {
            goto restart;
        }
    }

    EnterCriticalSection();
    FlushCache();
    ExitCriticalSection();
    VSync(0);
    SetDispMask(0);
    SetDefDispEnv(&sp18, 0, 0, 0x140, 0xF0);
    sp18.isrgb24 = 1;
    PutDispEnv(&sp18);
    return 0;
}
