#include "include_asm.h"

#include <stdio.h>

extern char D_80010EB0[];
extern char D_80010EC8[];
extern char *D_80091A2C[];

typedef unsigned char u8;

extern u8 *g_FontGlyphTable;

extern unsigned char D_80091A1C;

extern unsigned char g_FontGlyphLoadFailed;

extern int D_80091A24;

void SysErrReport(char *arg0, char *arg1, int arg2, unsigned char arg3) {
    int index = arg3 & 0xFF;

    printf(D_80010EB0, D_80091A2C[index], arg0);
    printf(D_80010EC8, arg1, arg2);

    if (index == 1) {
    loop:
        goto loop;
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/main/menu2", Render_LoadFontGlyph);

INCLUDE_ASM("asm/USA/main/nonmatchings/main/menu2", Render_DrawTextDigit);

int Menu_GetEquipSlotProperty(int arg0) {
    u8 *base;
    u8 *cursor;
    u8 index;

    base = g_FontGlyphTable;
    cursor = base + (u8)arg0;
    index = cursor[0x1D];
    base += index;
    return base[4];
}

int Menu_IsEquipSlotActive(void) {
    return D_80091A1C != 0;
}

int Menu_ConsumeEquipSlotFlag(void) {
    unsigned char *ptr;

    ptr = &g_FontGlyphLoadFailed;
    if (*ptr != 0) {
        *ptr = 0;
        return 0;
    }

    return 0xFF;
}

int Menu_ConsumeEquipSlotInt(void) {
    int *ptr;

    ptr = &D_80091A24;
    if (*ptr != 0) {
        *ptr = 0;
        return 0;
    }

    return 0xFF;
}
