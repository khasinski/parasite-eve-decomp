/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "include_asm.h"

extern int g_DrawDigitFontTpageClut;
extern int g_DrawDigitFontBaseTexU;
extern int g_DrawDigitFontBaseTexV;

extern volatile int g_TextCursorX;
extern volatile int g_TextCursorY;

void Draw_PrintNumberFixedWidth(int arg0, int arg1);
void Draw_AllocSprite(int arg0);

void Draw_SetFontVariant(int arg0) {
    g_DrawDigitFontTpageClut = arg0 != 0 ? 0x3A1C : 0x395D;
    g_DrawDigitFontBaseTexU = arg0 != 0 ? 0xCC : 0x84;
    g_DrawDigitFontBaseTexV = 0xA4;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/draw_print", Draw_EmitDigitSprite);

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
