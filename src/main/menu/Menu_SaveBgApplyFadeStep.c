/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_background.h"

void Menu_SaveBgApplyFadeStep(void) {
    int pos;
    int level;
    int bias;
    int attenuation;
    u16 *src;
    register u16 *dst asm("$8");
    int i;

    pos = D_8009CEE8 + D_8009CEE4;
    D_8009CEE8 = pos;
    if (pos < 0) {
        D_8009CEE8 = 0;
        D_8009CEE4 = 0;
        D_8009CED8 = 0;
    } else if (pos >= D_8009CEE0) {
        D_8009CEE8 = D_8009CEE0 - 1;
        D_8009CEE4 = 0;
        D_8009CED8 = 7;
    }

    level = D_800A1878[D_8009CEE8];
    {
        register int product asm("$2") = level * D_8009CEEC;
        bias = product << 5;
    }
    attenuation = level * (D_8009CEEC + 0x100);
    src = g_GameState.save_background_source;
    dst = g_GameState.save_background_destination;

    for (i = 0; i < (D_8009CEDC << 8); i++) {
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
