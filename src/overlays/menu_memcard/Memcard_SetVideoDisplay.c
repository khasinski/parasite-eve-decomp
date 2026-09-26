#include "menu_memcard_video.h"

void Memcard_SetVideoDisplay(s8 index,s8 wide) {
    s16 displayY, drawY;
    s32 displayOffset;
    s32 drawOffset;
    s32 scaled;
    if (!index) {
        displayY = 240;
        drawY = 0;
    } else {
        displayY = 0;
        drawY = 240;
    }
    /* Keep both screen origins in halfword stack slots. */
    __asm__("" : "=m"(displayY), "=m"(drawY) : "m"(displayY), "m"(drawY));
    if (wide) {
        D_801D0DBE = 3;
        func_800749D8(&D_800BCE80[index], 0, displayY, 480, 240);
        displayOffset = index * 20;
        scaled = *(s16 *)((u8 *)D_800BCE84 + displayOffset) * 2 / 3;
        *((u8 *)D_800BCE91 + displayOffset) = 1;
        *(s16 *)((u8 *)D_800BCE84 + displayOffset) = scaled;
        func_80074924(&D_800BCDC8[index], 0, drawY, 480, 240);
        drawOffset = index * 92;
        scaled = *(s16 *)((u8 *)D_800BCDCC + drawOffset) * 2 / 3;
        *((u8 *)D_800BCDE0 + drawOffset) = 1;
        *((u8 *)D_800BCDDE + drawOffset) = 1;
        *((u8 *)D_800BCDDF + drawOffset) = 0;
        *((u8 *)D_800BCDE1 + drawOffset) = 0;
        *((u8 *)D_800BCDE2 + drawOffset) = 0;
        *((u8 *)D_800BCDE3 + drawOffset) = 0;
        *(s16 *)((u8 *)D_800BCDCC + drawOffset) = scaled;
    } else {
        D_801D0DBE = 2;
        func_800749D8(&D_800BCE80[index], 0, displayY, 320, 240);
        func_80074924(&D_800BCDC8[index], 0, drawY, 320, 240);
        drawOffset = index * 92;
        *((u8 *)D_800BCDE0 + drawOffset) = 1;
        *((u8 *)D_800BCDDE + drawOffset) = 1;
        *((u8 *)D_800BCDDF + drawOffset) = 0;
        *((u8 *)D_800BCDE1 + drawOffset) = 0;
        *((u8 *)D_800BCDE2 + drawOffset) = 0;
        *((u8 *)D_800BCDE3 + drawOffset) = 0;
    }
}
