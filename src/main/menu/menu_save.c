/* CC1_FLAGS: -G8 -g3 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeHeight;
int g_MenuSaveBgFadeLutLen;
int g_MenuSaveBgFadeStep;
int g_MenuSaveBgFadeTint;

void Menu_ComputeGammaLut(int arg0, int arg1);

int g_MenuSaveBgFadeTint;

typedef unsigned char u8;

int g_MenuSaveBgFadeLutLen;
extern u8 D_800A1878[];

int g_MenuSaveBgFadeState;

void Menu_SaveBgInitFade(void) {
    g_MenuSaveBgFadeState = 0;
    g_MenuSaveBgFadeLutLen = 0;
    g_MenuSaveBgFadeStep = 0;
    g_MenuSaveBgFadeHeight = 0x20;
    Menu_ComputeGammaLut(0x90, 0xFF);
    g_MenuSaveBgFadeTint = 0x48;
}

void Menu_SaveBgSetFadeTarget(int arg0) {
    g_MenuSaveBgFadeTint = arg0;
}

void Menu_ComputeGammaLut(int arg0, int arg1) {
    u8 *base;
    u8 *ptr;
    int scale;
    int limit;
    u8 *iter_end;
    u8 *check_end;

    base = D_800A1878;
    base[0] = 0;
    ptr = base;
    limit = 0x100;
    scale = limit - arg0;
    check_end = ptr + 0xF;
    arg0 <<= 8;
    if (ptr < check_end) {
        iter_end = check_end;
        do {
            if (*ptr >= arg1) {
                goto out;
            }
            ptr[1] = (arg0 + (scale * *ptr)) >> 8;
            ptr++;
        } while (ptr < iter_end);
    }

out:
    base = D_800A1878;
    limit = (ptr - base) + 1;
    g_MenuSaveBgFadeLutLen = limit;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu_save", Menu_SaveBgApplyFadeStep);

int Menu_SaveBgIsFadeActive(void) {
    return g_MenuSaveBgFadeState != 0;
}
