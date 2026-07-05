/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "include_asm.h"

extern int g_DrawDigitFontTpageClut;
extern int g_DrawDigitFontBaseTexU;
extern int g_DrawDigitFontBaseTexV;

extern volatile int g_TextCursorX;
extern volatile int g_TextCursorY;

void Draw_PrintNumberFixedWidth(int arg0, int arg1);

void Draw_SetFontVariant(int arg0) {
    g_DrawDigitFontTpageClut = arg0 != 0 ? 0x3A1C : 0x395D;
    g_DrawDigitFontBaseTexU = arg0 != 0 ? 0xCC : 0x84;
    g_DrawDigitFontBaseTexV = 0xA4;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_050044", Draw_EmitDigitSprite);

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_050044", Draw_PrintNumberWidth2Unk);

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_050044", Draw_PrintNumberWidth3Unk);

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_050044", Draw_PrintSignedNumberWidth3);

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_050044", Draw_PrintNumberWidth4Unk);

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_050044", Draw_PrintSignedNumberWidth4);

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_050044", Draw_PrintTimeValue);

INCLUDE_ASM("asm/USA/main/nonmatchings/gpu/tu_050044", Draw_AllocTexturedRect);

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
