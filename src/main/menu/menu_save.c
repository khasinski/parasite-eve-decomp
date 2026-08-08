#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeHeight;
int g_MenuSaveBgFadeLutLen;
int g_MenuSaveBgFadeStep;
int g_MenuSaveBgFadeTint;
extern int g_MenuSaveBgFadeIndex;

void Menu_ComputeGammaLut(int arg0, int arg1);

int g_MenuSaveBgFadeTint;

int g_MenuSaveBgFadeLutLen;
extern u8 D_800A1878[];
extern u16 *D_800B0E50[3];
extern u16 *D_800B0E54[3];
extern unsigned char D_800BCDE1[16];
extern unsigned char D_800BCDE2[16];
extern unsigned char D_800BCDE3[16];
extern unsigned char D_800BCE3D[16];
extern unsigned char D_800BCE3E[16];
extern unsigned char D_800BCE3F[16];
extern int D_8009CED8;
extern int D_8009CEDC;
extern int D_8009CEE0;
extern int D_8009CEE4;
extern int D_8009CEE8;
extern int D_8009CEEC;

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

void Menu_SaveBgApplyFadeStep(void) {
    int idx;
    int lut;
    register int tint asm("$3");
    register int fade_base asm("$2");
    register int inv_factor asm("$11");
    int factor;
    int step;
    int i;
    u16 *src;
    u16 *dst;
    unsigned char shade;

    {
        register int idx_reg asm("$3");

        idx_reg = D_8009CEE8;
        step = D_8009CEE4;
        idx_reg = idx_reg + step;
        step = idx_reg;
        D_8009CEE8 = step;
        if (step < 0) {
            D_8009CEE8 = 0;
            D_8009CEE4 = 0;
            D_8009CED8 = 0;
        } else if (step >= D_8009CEE0) {
            D_8009CEE8 = D_8009CEE0 - 1;
            D_8009CEE4 = 0;
            D_8009CED8 = 7;
        }
    }

    idx = D_8009CEE8;
    tint = D_8009CEEC;
    lut = D_800A1878[idx];
    fade_base = lut * tint;
    tint += 0x100;
    factor = lut * tint;
    src = D_800B0E50[0];
    inv_factor = fade_base << 5;
    asm volatile("" : "=r"(inv_factor) : "0"(inv_factor));
    dst = D_800B0E54[0];
    for (i = 0; i < (D_8009CEDC << 8); i++) {
        int color;
        color = *src++;
        if (color != 0) {
            int r;
            register int g asm("$6");
            register int b asm("$5");
            int r_prod;
            register int g_prod asm("$9");
            int b_prod;
            register int tmp asm("$2");

            r = color & 0x1F;
            r_prod = factor * r;
            asm("sra %0, %1, 5" : "=r"(tmp) : "r"(color));
            g = tmp & 0x1F;
            g_prod = factor * g;
            asm("sra %0, %1, 10" : "=r"(tmp) : "r"(color));
            b = tmp & 0x1F;
            color &= 0x8000;
            b_prod = factor * b;
            tmp = r << 16;
            tmp = inv_factor + tmp;
            tmp -= r_prod;
            r = tmp >> 16;
            tmp = g << 16;
            tmp = inv_factor + tmp;
            tmp -= g_prod;
            g = tmp >> 16;
            tmp = b << 16;
            tmp = inv_factor + tmp;
            tmp -= b_prod;
            b = tmp >> 16;
            color |= r;
            color |= g << 5;
            color |= b << 10;
            *dst = color;
        } else {
            *dst = 0;
        }
        dst++;
    }

    shade = inv_factor >> 13;
    D_800BCE3F[0] = shade;
    D_800BCE3E[0] = shade;
    D_800BCE3D[0] = shade;
    D_800BCDE3[0] = shade;
    D_800BCDE2[0] = shade;
    D_800BCDE1[0] = shade;
}

int Menu_SaveBgIsFadeActive(void) {
    return g_MenuSaveBgFadeState != 0;
}
