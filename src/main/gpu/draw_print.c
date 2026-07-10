/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "include_asm.h"

extern int g_DrawDigitFontTpageClut;
extern int g_DrawDigitFontBaseTexU;
extern int g_DrawDigitFontBaseTexV;

extern volatile int g_TextCursorX;
extern volatile int g_TextCursorY;

extern unsigned char *D_8009D100;
extern unsigned char *D_8009D104;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern unsigned int *D_8009D11C;
extern unsigned short D_8009D124;
extern unsigned short D_8009D128;
extern int D_8009D13C;
extern int D_8009D140;
extern int D_8009D144;

void Draw_PrintNumberFixedWidth(int arg0, int arg1);
void Draw_AllocSprite(int arg0);
void BoundsCheck_AssertStub(int arg0);

typedef struct DrawDigitPrim {
    unsigned int tag;
    unsigned char r0, g0, b0, code;
    unsigned short x0, y0;
    unsigned char u0, v0;
    unsigned short clut;
    unsigned short x1, y1;
    unsigned char u1, v1;
    unsigned short tpage;
    unsigned short x2, y2;
    unsigned char u2, v2;
    unsigned short pad2;
    unsigned short x3, y3;
    unsigned char u3, v3;
    unsigned short pad3;
} DrawDigitPrim;

void Draw_SetFontVariant(int arg0) {
    g_DrawDigitFontTpageClut = arg0 != 0 ? 0x3A1C : 0x395D;
    g_DrawDigitFontBaseTexU = arg0 != 0 ? 0xCC : 0x84;
    g_DrawDigitFontBaseTexV = 0xA4;
}

void Draw_EmitDigitSprite(int digit) {
    register int digit_reg asm("$17") = digit;
    register DrawDigitPrim *prim asm("$16");
    register DrawDigitPrim *packet asm("$7");
    unsigned char *old;
    unsigned char *next;
    int u;
    int v;
    unsigned int x;
    unsigned int y;
    unsigned int ot;

    old = D_8009D100;
    prim = 0;
    next = old + sizeof(DrawDigitPrim);
    if (next < D_8009D104 + 0x4000) {
        D_8009D100 = next;
        prim = (DrawDigitPrim *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }

    if (prim != 0) {
        if (D_8009D10C != 0) {
            *(unsigned int *)&prim->r0 = D_8009D114;
        } else {
            *(unsigned int *)&prim->r0 = D_8009D110;
        }
        ((unsigned char *)prim)[3] = 9;
        prim->code = 0x2C;
    }

    packet = prim;
    packet->x2 = D_8009D124;
    packet->x0 = D_8009D124;
    x = packet->x0;
    packet->y1 = D_8009D128;
    packet->y0 = D_8009D128;
    y = packet->y0;
    x += 5;
    y += 7;
    packet->x3 = x;
    packet->x1 = x;
    packet->y3 = y;
    packet->y2 = y;

    if (digit_reg >= 0) {
        u = D_8009D140 + (digit_reg % 10) * 5;
    } else {
        u = 0x58;
    }
    packet->u2 = u;
    u = packet->u2;
    packet->u2 = u;
    packet->u0 = u;

    if (digit_reg >= 0) {
        packet->v1 = D_8009D144;
    } else {
        packet->v1 = 0xA4;
    }
    v = packet->v1;
    u = packet->u0;
    packet->v0 = v;
    v = packet->v0;
    u += 5;
    packet->v1 = v;
    packet->u3 = u;
    packet->u1 = u;
    v += 7;
    packet->v3 = v;
    packet->v2 = v;
    packet->clut = D_8009D13C;
    packet->tpage = 7;

    ot = *D_8009D11C;
    prim->tag = (prim->tag & 0xFF000000) | (ot & 0x00FFFFFF);
    *D_8009D11C = (*D_8009D11C & 0xFF000000) | ((unsigned int)prim & 0x00FFFFFF);
}

void Draw_PrintNumberWidth2Unk(int value) {
    register volatile int digit_base asm("$17");
    register int width asm("$18");
    register int number asm("$19");
    register int magic asm("$20");
    register int last_leading asm("$21");
    register int i asm("$16");
    int pos;
    int other;
    int digit;
    register int leading asm("$2");

    number = value;
    width = 2;
    digit_base = 1;

    if (number < 0) {
        number = -number;
        Draw_AllocSprite(0x52);
        width = 1;
        pos = g_TextCursorX;
        other = g_TextCursorY;
        g_TextCursorX = pos + 5;
        g_TextCursorY = other;
    }

    for (i = 1; i < width; i++) {
        digit_base *= 10;
    }

    asm volatile("" : "=r"(width), "=r"(digit_base) : "0"(width), "1"(digit_base));
    i = 0;
    if (width != 0) {
        last_leading = width - 1;
        magic = 0x66666667;
        do {
            leading = i < last_leading;
            digit = number / digit_base;
            if (leading && digit == 0) {
                digit = -1;
            }
            i++;
            Draw_EmitDigitSprite(digit);
            asm volatile("mult %0,%1" : : "r"(digit_base), "r"(magic));
            pos = g_TextCursorX;
            other = g_TextCursorY;
            g_TextCursorX = pos + 5;
            asm volatile("sra $2,%0,31" : : "r"(digit_base) : "$2");
            g_TextCursorY = other;
            asm volatile(
                "mfhi $5\n"
                "sra $3,$5,2\n"
                "subu %0,$3,$2"
                : "=r"(digit_base)
                :
                : "$3", "$5");
        } while (i < width);
    }
}

void Draw_PrintNumberWidth3Unk(int value) {
    register volatile int digit_base asm("$17");
    register int width asm("$18");
    register int number asm("$19");
    register int magic asm("$20");
    register int last_leading asm("$21");
    register int i asm("$16");
    int pos;
    int other;
    int digit;
    register int leading asm("$2");

    number = value;
    width = 3;
    digit_base = 1;

    if (number < 0) {
        number = -number;
        Draw_AllocSprite(0x52);
        width = 2;
        pos = g_TextCursorX;
        other = g_TextCursorY;
        g_TextCursorX = pos + 5;
        g_TextCursorY = other;
    }

    for (i = 1; i < width; i++) {
        digit_base *= 10;
    }

    asm volatile("" : "=r"(width), "=r"(digit_base) : "0"(width), "1"(digit_base));
    i = 0;
    if (width != 0) {
        last_leading = width - 1;
        magic = 0x66666667;
        do {
            leading = i < last_leading;
            digit = number / digit_base;
            if (leading && digit == 0) {
                digit = -1;
            }
            i++;
            Draw_EmitDigitSprite(digit);
            asm volatile("mult %0,%1" : : "r"(digit_base), "r"(magic));
            pos = g_TextCursorX;
            other = g_TextCursorY;
            g_TextCursorX = pos + 5;
            asm volatile("sra $2,%0,31" : : "r"(digit_base) : "$2");
            g_TextCursorY = other;
            asm volatile(
                "mfhi $5\n"
                "sra $3,$5,2\n"
                "subu %0,$3,$2"
                : "=r"(digit_base)
                :
                : "$3", "$5");
        } while (i < width);
    }
}

void Draw_PrintSignedNumberWidth3(int value) {
    register volatile int digit_base asm("$17");
    register int width asm("$18");
    register int number asm("$19");
    register int magic asm("$20");
    register int last_leading asm("$21");
    register int i asm("$16");
    int pos;
    int other;
    int digit;
    register int leading asm("$2");

    number = value;
    width = 3;
    digit_base = 1;

    if (number < 0) {
        number = -number;
        Draw_AllocSprite(0x52);
        width = 2;
        pos = g_TextCursorX;
        other = g_TextCursorY;
        g_TextCursorX = pos + 5;
        g_TextCursorY = other;
    } else if (number > 0) {
        Draw_AllocSprite(0x89);
        width = 2;
        pos = g_TextCursorX;
        other = g_TextCursorY;
        g_TextCursorX = pos + 5;
        g_TextCursorY = other;
    }

    for (i = 1; i < width; i++) {
        digit_base *= 10;
    }

    asm volatile("" : "=r"(width), "=r"(digit_base) : "0"(width), "1"(digit_base));
    i = 0;
    if (width != 0) {
        last_leading = width - 1;
        magic = 0x66666667;
        do {
            leading = i < last_leading;
            digit = number / digit_base;
            if (leading && digit == 0) {
                digit = -1;
            }
            i++;
            Draw_EmitDigitSprite(digit);
            asm volatile("mult %0,%1" : : "r"(digit_base), "r"(magic));
            pos = g_TextCursorX;
            other = g_TextCursorY;
            g_TextCursorX = pos + 5;
            asm volatile("sra $2,%0,31" : : "r"(digit_base) : "$2");
            g_TextCursorY = other;
            asm volatile(
                "mfhi $5\n"
                "sra $3,$5,2\n"
                "subu %0,$3,$2"
                : "=r"(digit_base)
                :
                : "$3", "$5");
        } while (i < width);
    }
}

void Draw_PrintNumberWidth4Unk(int value) {
    register volatile int digit_base asm("$17");
    register int width asm("$18");
    register int number asm("$19");
    register int magic asm("$20");
    register int last_leading asm("$21");
    register int i asm("$16");
    int pos;
    int other;
    int digit;
    register int leading asm("$2");

    number = value;
    width = 4;
    digit_base = 1;

    if (number < 0) {
        number = -number;
        Draw_AllocSprite(0x52);
        width = 3;
        pos = g_TextCursorX;
        other = g_TextCursorY;
        g_TextCursorX = pos + 5;
        g_TextCursorY = other;
    }

    for (i = 1; i < width; i++) {
        digit_base *= 10;
    }

    asm volatile("" : "=r"(width), "=r"(digit_base) : "0"(width), "1"(digit_base));
    i = 0;
    if (width != 0) {
        last_leading = width - 1;
        magic = 0x66666667;
        do {
            leading = i < last_leading;
            digit = number / digit_base;
            if (leading && digit == 0) {
                digit = -1;
            }
            i++;
            Draw_EmitDigitSprite(digit);
            asm volatile("mult %0,%1" : : "r"(digit_base), "r"(magic));
            pos = g_TextCursorX;
            other = g_TextCursorY;
            g_TextCursorX = pos + 5;
            asm volatile("sra $2,%0,31" : : "r"(digit_base) : "$2");
            g_TextCursorY = other;
            asm volatile(
                "mfhi $5\n"
                "sra $3,$5,2\n"
                "subu %0,$3,$2"
                : "=r"(digit_base)
                :
                : "$3", "$5");
        } while (i < width);
    }
}

void Draw_PrintSignedNumberWidth4(int value) {
    register volatile int digit_base asm("$17");
    register int width asm("$18");
    register int number asm("$19");
    register int magic asm("$20");
    register int last_leading asm("$21");
    register int i asm("$16");
    int pos;
    int other;
    int digit;
    register int leading asm("$2");

    number = value;
    width = 4;
    digit_base = 1;

    if (number < 0) {
        number = -number;
        Draw_AllocSprite(0x52);
        width = 3;
        pos = g_TextCursorX;
        other = g_TextCursorY;
        g_TextCursorX = pos + 5;
        g_TextCursorY = other;
    } else if (number > 0) {
        Draw_AllocSprite(0x89);
        width = 3;
        pos = g_TextCursorX;
        other = g_TextCursorY;
        g_TextCursorX = pos + 5;
        g_TextCursorY = other;
    }

    for (i = 1; i < width; i++) {
        digit_base *= 10;
    }

    asm volatile("" : "=r"(width), "=r"(digit_base) : "0"(width), "1"(digit_base));
    i = 0;
    if (width != 0) {
        last_leading = width - 1;
        magic = 0x66666667;
        do {
            leading = i < last_leading;
            digit = number / digit_base;
            if (leading && digit == 0) {
                digit = -1;
            }
            i++;
            Draw_EmitDigitSprite(digit);
            asm volatile("mult %0,%1" : : "r"(digit_base), "r"(magic));
            pos = g_TextCursorX;
            other = g_TextCursorY;
            g_TextCursorX = pos + 5;
            asm volatile("sra $2,%0,31" : : "r"(digit_base) : "$2");
            g_TextCursorY = other;
            asm volatile(
                "mfhi $5\n"
                "sra $3,$5,2\n"
                "subu %0,$3,$2"
                : "=r"(digit_base)
                :
                : "$3", "$5");
        } while (i < width);
    }
}

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/draw_print", Draw_PrintTimeValue);

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/draw_print", Draw_AllocTexturedRect);

void Draw_PrintNumberWidth6(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 6);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 6;
    g_TextCursorY = other;
}

void Draw_PrintNumberWidth5(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 5);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 5;
    g_TextCursorY = other;
}

void Draw_PrintNumberWidth4(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 4);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 4;
    g_TextCursorY = other;
}

void Draw_PrintNumberWidth3(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 3);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 3;
    g_TextCursorY = other;
}

void Draw_PrintNumberWidth2(int arg0) {
    int pos;
    int other;

    Draw_PrintNumberFixedWidth(arg0, 2);
    pos = g_TextCursorX;
    other = g_TextCursorY;
    g_TextCursorX = pos + 2;
    g_TextCursorY = other;
}
