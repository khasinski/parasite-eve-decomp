/* Scene display clearing and CD read flow share the scene read flags. */
#include "pe1/game_state_types.h"
#include "common.h"
#include "pe1/render_camera.h"
#include "pe1/psyq_gpu.h"
#include "pe1/cdrom.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/psyq_cd.h"
/* Scalar symbol view still preserves the boot loader scheduling.
 * Sector read and polling routines below use the shared structure fields. */
extern int g_GameState;

extern unsigned char g_DiscChangeFlags;

void ClearImage(RECT *rect, int r, int g, int b);
int DrawSync(int arg0);

void Gpu_ClearOnFlag(void) {
    u32 *state = (u32 *)&g_GameState;

    if (*state & 0x08000000) {
        RECT rect;
        unsigned int value;
        unsigned char byte;

        rect.x = 0;
        rect.y = 0;
        rect.w = 0x140;
        rect.h = 0x1C0;
        ClearImage(&rect, 0, 0, 1);
        DrawSync(0);
        Render_PrepareFrame();
        byte = g_DiscChangeFlags;
        value = *state;
        g_DiscChangeFlags = byte | 2;
        *state = value & 0xF7FFFDFF;
    }
}


int CdRom_ReadSectorsFromLba(u32 lba, void *destination, u32 size) {
    return CdRom_ReadSectors(lba, 0, destination, size);
}

void exit(int code);
CdlLOC *DsIntToPos(int i, CdlLOC *p);
int printf(char *fmt, ...);

extern u_short g_CdDiskType;
extern char D_8001136C[];

int CdRom_ReadSectors(u32 lba, u32 offset, void *destination, u32 size) {
    register int base;
    register int rel;
    register int dst_reg;
    register int size_reg;
    Pe1GameState *state;
    CdlLOC loc;
    int ret;

    base = lba;
    rel = offset;
    dst_reg = (int)destination;
    size_reg = size;
    state = (Pe1GameState *)&g_GameState;

    if ((state->flags & 0x1000000) != 0) {
        return -1;
    }
    if (DsSystemStatus() != 1) {
        return -1;
    }
    if (DsQueueLen() != 0) {
        return -1;
    }
    if (DsShellOpen() != g_CdDiskType) {
        exit(1);
    }

    state->flags |= 0x1004000;
    base += rel;
    DsIntToPos(base, &loc);
    ret = DsRead(&loc, size_reg, (void *)dst_reg, 0x80);
    if (ret != 0) {
        return ret;
    }

    state->flags &= 0xFEFFBFFF;
    printf(D_8001136C, base, size_reg);
    return -1;
}
int CdRom_PollReady(void) {
    int scratch;
    int status;
    Pe1GameState *state;

    status = DsReadSync(&scratch);
    if ((unsigned int)(status + 1) < 2U) {
        state = (Pe1GameState *)&g_GameState;
        state->flags &= 0xFEFFBFFF;
    }
    return status;
}

#define NULL ((void *)0)


void Akao_Cmd_F0(void);
int VSync(int mode);
s32 EnterCriticalSection();
s32 ExitCriticalSection();
void FlushCache(void);

DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h);

extern s8 D_800B0DB2;
extern s8 D_800B0DB3;
extern s8 D_800B0DB4;
extern s8 D_800B0DB5;
extern s8 D_800B0DB6;
extern s8 D_800B0DB7;
extern s32 g_PeImageBaseLba;
extern u16 g_StrFileDirLba[];

void SetDispMask(int mask);

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
        v1 = DsReadSync(&sp30);
        v0 = v1;
        v0 = (v1 + 1);
        if (v0 < 2U) {
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
