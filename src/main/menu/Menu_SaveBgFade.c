/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/menu_background.h"
#include "pe1/menu_inventory.h"

/* Save-screen background fade: gamma curve, per-frame darkening of the
 * captured frame and the fade state machine setters. Contiguous at
 * 0x80042C78 and joined by the fade state words. MemCard_InitSlotState
 * (historical name) arms the fade-in. */

/* g_GeomOtZ lives at 0x800BD024, outside the gp window; the incomplete
 * array type keeps -G8 from treating the 1-byte extern as small data. */
extern unsigned char g_GeomOtZ[];

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeStep;

void Menu_SaveBgInitFade(void) {
    g_MenuSaveBgFadeState = 0;
    g_MenuSaveBgFadeLutLen = 0;
    g_MenuSaveBgFadeStep = 0;
    g_MenuSaveBgFadeHeight = 0x20;
    Menu_ComputeGammaLut(0x90, 0xFF);
    g_MenuSaveBgFadeTint = 0x48;
}

void Menu_SaveBgSetFadeTarget(int value) {
    g_MenuSaveBgFadeTint = value;
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
    int pos;
    int level;
    int bias;
    int attenuation;
    u16 *src;
    register u16 *dst asm("$8");
    int i;

    pos = g_MenuSaveBgFadeIndex + g_MenuSaveBgFadeStep;
    g_MenuSaveBgFadeIndex = pos;
    if (pos < 0) {
        g_MenuSaveBgFadeIndex = 0;
        g_MenuSaveBgFadeStep = 0;
        g_MenuSaveBgFadeState = 0;
    } else if (pos >= g_MenuSaveBgFadeLutLen) {
        g_MenuSaveBgFadeIndex = g_MenuSaveBgFadeLutLen - 1;
        g_MenuSaveBgFadeStep = 0;
        g_MenuSaveBgFadeState = 7;
    }

    level = D_800A1878[g_MenuSaveBgFadeIndex];
    {
        int product = level * g_MenuSaveBgFadeTint;
        bias = product << 5;
    }
    attenuation = level * (g_MenuSaveBgFadeTint + 0x100);
    src = g_GameState.save_background_source;
    dst = g_GameState.save_background_destination;

    for (i = 0; i < (g_MenuSaveBgFadeHeight << 8); i++) {
        register int color asm("$3") = *src++;

        if (color != 0) {
            register int r asm("$7") = color & 0x1F;
            int g = (color >> 5) & 0x1F;
            int b = (color >> 10) & 0x1F;
            color &= 0x8000;

            {
                int mixed = r << 16;
                mixed = bias + mixed - attenuation * r;
                r = mixed >> 16;
            }
            g = (bias + (g << 16) - (attenuation * g)) >> 16;
            color |= r;
            b = (bias + (b << 16) - (attenuation * b)) >> 16;
            *dst = color | (g << 5) | (b << 10);
        } else {
            *dst = 0;
        }
        dst++;
    }

    {
        unsigned shade = (unsigned)bias >> 13;
        D_800BCDC8[1].b0 = shade;
        D_800BCDC8[1].g0 = shade;
        D_800BCDC8[1].r0 = shade;
        D_800BCDC8[0].b0 = shade;
        D_800BCDC8[0].g0 = shade;
        D_800BCDC8[0].r0 = shade;
    }
}

int Menu_SaveBgIsFadeActive(void) {
    return g_MenuSaveBgFadeState != 0;
}

void MemCard_InitSlotState(void) {
    int value = g_GeomOtZ[0];

    g_MenuSaveBgFadeState = 1;
    g_MenuSaveBgFadeStep = 1;
    g_MenuSaveBgFadeIndex = 0;
    g_MenuSaveBgFadeHeight = value;

    /* value is a zero-extended byte, so the < 0 arm is dead at runtime,
     * but it is what retail compiled (bgez). */
    if (value < 0) {
        g_MenuSaveBgFadeHeight = 1;
    } else if (value >= 0x21) {
        g_MenuSaveBgFadeHeight = 0x20;
    }
}

void Menu_SaveBgStartFadeOut(void) {
    g_MenuSaveBgFadeState = 5;
    g_MenuSaveBgFadeStep = -1;
}

void Menu_SaveBgClearFade(void) {
    g_MenuSaveBgFadeState = 0;
}
