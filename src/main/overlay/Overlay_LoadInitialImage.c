
#include "include_asm.h"
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/psyq_gpu.h"

s32 Akao_Cmd_F0();
s32 VSync();
s32 PutDispEnv() __asm__("Render_StepEntityPool");
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
extern s32 g_GameState;
extern s32 g_PeImageBaseLba;
extern s32 g_StrFileDirBuffer;
extern u16 g_StrFileDirLba[];

s32 CdRom_ReadSectors(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
s32 Sys_VSyncTimeout(s32 *status);
s32 SetDispMask(s32 arg0);

asm(".globl func_8006E834");
asm("func_8006E834 = Overlay_LoadInitialImage");

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
