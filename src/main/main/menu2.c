#include "include_asm.h"

extern char D_80010EB0[];
extern char D_80010EC8[];
extern char *D_80091A2C[];
int printf(char *fmt, ...);

typedef unsigned char u8;

extern u8 *g_FontGlyphTable;
extern struct { char _[16]; } D_80091A28_o __asm__("D_80091A28");
#define D_80091A28 (*(u8 **)&D_80091A28_o)
extern struct { char _[16]; } D_80091A1D_o __asm__("D_80091A1D");
#define D_80091A1D (*(u8 *)&D_80091A1D_o)

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

int Render_DrawTextDigit(u8 *table, int mode) {
    register u8 *table_reg asm("$7");
    register int digit asm("$3");
    register int selected asm("$6");
    register int i asm("$5");
    int count;
    register int count_load asm("$2");
    register u8 *base asm("$4");
    u8 *ptr;
    int stack_pad[3];

    table_reg = table;
    if (table_reg == 0) {
        table_reg = D_80091A28;
    }

    mode &= 0xFF;
    if (mode == 1) {
        goto mode_one;
    }
    if (mode < 2) {
        digit = 3;
        if (mode != 0) {
            digit = 4;
        }
        goto common;
    }
    digit = 2;
    if (mode != 2) {
        digit = 4;
    }
    goto common;

mode_one:
    digit = D_80091A1D % 10;
    digit = digit != 0;

common:
    selected = 0;
    i = 0;
    count_load = table_reg[3];
    base = table_reg + 1;
    if (count_load > 0) {
        digit &= 0xFF;
        count = count_load;
        do {
            ptr = base + i;
            if (ptr[3] == digit) {
                selected = i;
                i = base[2];
            }
            i++;
        } while (i < count);
        i = 0;
    }

    count_load = base[0x1B];
    base += 0x1B;
    if (count_load > 0) {
        register int count2 asm("$3");

        selected &= 0xFF;
        count2 = count_load;
        do {
            ptr = base + i;
            if (ptr[1] == selected) {
                return i & 0xFF;
            }
            i++;
        } while (i < count2);
    }

    asm volatile("" : "=m"(stack_pad));
    return 0xFF;
}

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
