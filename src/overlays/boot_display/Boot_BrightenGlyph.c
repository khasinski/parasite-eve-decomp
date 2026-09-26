#include "pe1/boot_display.h"
/* RGB24 pixels receive saturated brightness from four packed palette indices. */
#define BRIGHTEN_NIBBLE(mask, shift) do { \
    u32 index = bits & (mask); \
    if (index) { \
        u32 brightness = (((u16 *)D_80172C9C)[index >> (shift)] & 31) << 3; \
        register u32 blue asm("$5") = greenPixel[1]; \
        register u32 green asm("$4") = greenPixel[0]; \
        register u32 red asm("$3") = pixel[0]; \
        blue += brightness; if (blue >= 256) blue = 255; \
        green += brightness; if (green >= 256) green = 255; \
        red += brightness; if (red >= 256) red = 255; \
        greenPixel[1] = blue; greenPixel[0] = green; pixel[0] = red; \
    } \
    greenPixel += 3; \
    asm("" : "=r"(greenPixel) : "0"(greenPixel)); \
    pixel += 3; \
} while (0)
void Boot_BrightenGlyph(s32 x, s32 y, u8 character) {
    s32 savedX = x;
    register s32 row asm("$18") = y;
    char *screen = D_80125B88[g_BootDisplayBufferIndex * 4];
    s32 glyph = func_80123B1C(character);
    s32 low = glyph & 15;
    s32 high = glyph & 240;
    s32 imageOffset;
    register u8 *image asm("$11");
    register s32 endY asm("$16");
    register s32 computedEnd asm("$2");
    s32 nextRow;
    /* Empty barriers preserve retail mask, coordinate, and font-load ordering. */
    asm("" : : "r"(low), "r"(high));
    imageOffset = (low * 4 + (high << 6)) * 2;
    asm("" : : "r"(imageOffset));
    savedX = (s16)savedX; y = (s16)y;
    asm("" : : "r"(savedX) : "memory");
    computedEnd = (s16)y * 15;
    image = (u8 *)D_80172C98;
    asm("" : "=r"(image) : "0"(image));
    computedEnd = (s32)((u32)computedEnd << 6);
    image += imageOffset;
    screen += (s16)savedX * 3 + computedEnd;
    computedEnd = (s16)y + 16;
    if ((s16)y < computedEnd) {
        endY = computedEnd;
        do {
            u16 *source = (u16 *)image;
            u8 *pixel = (u8 *)screen;
            s16 chunk;
            u8 *greenPixel;
            if ((s16)row >= 0) {
                if ((s16)row >= 224) break;
                chunk = 0;
                asm("" : : "r"(chunk));
                greenPixel = pixel + 1;
                for (; chunk < 4; ++chunk) {
                    u32 bits = *source;
                    if (bits) {
                        BRIGHTEN_NIBBLE(0xF, 0);
                        BRIGHTEN_NIBBLE(0xF0, 4);
                        BRIGHTEN_NIBBLE(0xF00, 8);
                        BRIGHTEN_NIBBLE(0xF000, 12);
                    } else {
                        greenPixel += 12;
                        pixel += 12;
                    }
                    ++source;
                }
            }
            image += 128;
            nextRow = row + 1;
            row = nextRow;
            asm("" : "=r"(nextRow) : "0"(nextRow));
            screen += 960;
        } while ((s16)nextRow < endY);
    }
}
