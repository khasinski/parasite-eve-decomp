#include "pe1/boot_display.h"
#include "pe1/psyq_gpu.h"
extern void func_800749D8(DISPENV *, s32, s32, s32, s32);
extern void func_80074924(DRAWENV *, s32, s32, s32, s32);
/* Pins and empty barriers retain the retail address calculations and call setup. */
void Boot_SetImageDisplay(u16 index) {
    s32 displayY, drawY;
    register s32 displayOffset asm("$17");
    register s32 drawOffset asm("$16");
    u16 savedWidth;
    register DRAWENV *drawArg asm("$4");
    register s32 zeroArg asm("$5");
    register s32 yArg asm("$6");
    register s32 widthArg asm("$7");
    if (index != 0) {
        displayY = 0;
        drawY = 224;
    } else {
        displayY = 224;
        drawY = 0;
    }
    displayOffset = index * sizeof(DISPENV);
    func_800749D8((DISPENV *)(displayOffset + g_RenderDisplayEnvArray), 0, displayY, 320, 224);
    drawOffset = index * sizeof(DRAWENV);
    drawArg = (DRAWENV *)(drawOffset + g_RenderDrawEnvArray);
    zeroArg = 0;
    yArg = drawY;
    widthArg = 480;
    __asm__("" : : "r"(drawArg), "r"(zeroArg), "r"(yArg), "r"(widthArg));
    savedWidth = ((DISPENV *)(g_RenderDisplayEnvArray + displayOffset))->disp.w;
    ((DISPENV *)(g_RenderDisplayEnvArray + displayOffset))->isrgb24 = 1;
    ((DISPENV *)(g_RenderDisplayEnvArray + displayOffset))->screen.y = 8;
    __asm__("" : "=r"(displayOffset) : "0"(displayOffset));
    ((DISPENV *)(g_RenderDisplayEnvArray + displayOffset))->screen.h = 224;
    __asm__("" : "=r"(displayOffset) : "0"(displayOffset));
    ((DISPENV *)(g_RenderDisplayEnvArray + displayOffset))->disp.w = savedWidth;
    __asm__("" : "=r"(displayOffset) : "0"(displayOffset));
    func_80074924(drawArg, zeroArg, yArg, widthArg, 224);
    ((DRAWENV *)(g_RenderDrawEnvArray + drawOffset))->dtd = 0;
    ((DRAWENV *)(g_RenderDrawEnvArray + drawOffset))->isbg = 0;
    ((DRAWENV *)(g_RenderDrawEnvArray + drawOffset))->clip.w = ((DRAWENV *)(g_RenderDrawEnvArray + drawOffset))->clip.w * 2 / 3;
}
