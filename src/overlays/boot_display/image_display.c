#include "pe1/boot.h"
#include "pe1/boot_display.h"
#include "pe1/psyq_gpu.h"

#include "image_display_externs.h"

void BootDisplay_InitDisplayBuffers(void) {
    s16 rect[4];
    u16 line;
    /* Keep the original buffer offset in v0 across both address calculations. */
    register s32 delta asm("$2");
    char *base;
    char *start;
    char *end;
    s32 offset;
    u16 *screenY;
    void *resource;

    ResetGraph(1);
    SetGraphDebug(0);
    SetDispMask(0);

    screenY = g_MainAssetLbaTbl;
    g_BootDisplayTextScrollX = 0xE000;
    g_BootDisplayTextScrollStepX = -0x100;
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x3FF;
    rect[3] = 0x1FF;
    g_BootDisplayBufferIndex = 0;
    g_BootDisplayState = 0;
    g_BootDisplayTransferProgress = 0;
    g_BootDisplayStateTimer = 0;
    g_BootDisplayStateTimer = 0;

    line = *screenY;
    base = g_CdLoadAreaBase;
    delta = (line - g_BootDisplayDataLbaTbl) << 11;
    start = base + delta;
    delta += 0x54000;
    end = g_CdLoadAreaBase + delta;
    g_BootDisplayImageBuffers[0].pixels = start;
    g_BootDisplayImageBuffers[0].decodeTarget = end;
    g_BootDisplayImageBuffers[1].pixels = end;
    g_BootDisplayImageBuffers[1].decodeTarget = start;
    ClearImage(rect, 0, 0, 0);
    DrawSync(0);
    DecDCTReset(0);
    offset = 0x43000;
    MdecDiag_ExpandDecoderTable(((*screenY - g_BootDisplayDataLbaTbl) << 11) + offset + g_CdLoadAreaBase);

    resource = g_BootDisplayFontTim;
    g_BootDisplayFontImagePixels = Asset_GetTimImagePixels(resource);
    g_BootDisplayFontClutPixels = Asset_GetTimClutPixels(resource);
    g_BootDisplayFontImageRect = Asset_GetTimImageRect(resource);
    g_BootDisplayFontClutRect = Asset_GetTimClutRect(resource);
    g_BootDisplayTPage = GetTPage(0, 0, g_BootDisplayFontImageRect[0], g_BootDisplayFontImageRect[1]);
    g_BootDisplayClut = GetClut(g_BootDisplayFontClutRect[0], g_BootDisplayFontClutRect[1]);
    BootDisplay_SetImageDisplay(0);
    BootDisplay_SetImageDisplay(1);
    PutDispEnv(&g_RenderDispEnvArray[g_ActiveDrawSlot]);
    SetDispMask(1);
}

void BootDisplay_PresentFrame(void) {
    s16 rect[4];
    u16 timer;
    if (g_ActiveDrawSlot != 0) {
        rect[2] = 480;
        rect[0] = 0;
        rect[1] = 0;
        rect[3] = 224;
    } else {
        rect[0] = 0;
        rect[1] = 224;
        rect[2] = 480;
        rect[3] = 224;
    }
    LoadImage(rect, g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].pixels);
    if (g_BootDisplayState == 6) {
        timer = g_BootDisplayStateTimer + 1;
        g_BootDisplayStateTimer = timer;
        g_BootDisplayBufferIndex ^= 1;
        if (timer < 31)
            g_BootDisplayState = 0;
        else
            g_BootDisplayState = 7;
    }
    DrawSync(0);
    VSync(2);
    PutDispEnv(&g_RenderDispEnvArray[g_ActiveDrawSlot]);
    PutDrawEnv(&g_RenderDrawEnvArray[g_ActiveDrawSlot]);
    g_ActiveDrawSlot ^= 1;
}

/* RGB24 pixels receive saturated brightness from four packed palette indices. */
#define BRIGHTEN_NIBBLE(mask, shift) { \
    u32 index = bits & (mask); \
    if (index) { \
        u32 brightness = (((u16 *)g_BootDisplayFontClutPixels)[index >> (shift)] & 31) << 3; \
        register u32 blue asm("$5") = greenPixel[1]; \
        register u32 green asm("$4") = greenPixel[0]; \
        u32 red = pixel->red; \
        blue += brightness; if (blue >= 256) blue = 255; \
        green += brightness; if (green >= 256) green = 255; \
        red += brightness; if (red >= 256) red = 255; \
        greenPixel[1] = blue; greenPixel[0] = green; pixel->red = red; \
    } \
    greenPixel += 3; \
    asm("" : "=r"(greenPixel) : "0"(greenPixel)); \
    ++pixel; \
}
void BootDisplay_BrightenGlyph(s32 x, s32 y, u8 character) {
    s32 savedX = x;
    register s32 row asm("$18") = y;
    char *screen = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].pixels;
    s32 glyph = BootDisplay_GetGlyphIndex(character);
    s32 low = glyph & 15;
    s32 high = glyph & 240;
    s32 imageOffset;
    u8 *image;
    register s32 endY asm("$16");
    register s32 computedEnd asm("$2");
    s32 nextRow;
    /* Empty barriers preserve retail mask, coordinate, and font-load ordering. */
    imageOffset = (low * 4 + (high << 6)) * 2;
    asm("" : : "r"(imageOffset));
    savedX = (s16)savedX; y = (s16)y;
    asm("" : : "r"(savedX));
    computedEnd = (s16)y * 15;
    image = (u8 *)g_BootDisplayFontImagePixels;
    asm("" : "=r"(image) : "0"(image));
    computedEnd = (computedEnd << 6);
    image += imageOffset;
    screen += (s16)savedX * 3 + computedEnd;
    computedEnd = (s16)y + 16;
    if ((s16)y < computedEnd) {
        endY = computedEnd;
        do {
            u16 *source = (u16 *)image;
            BootRgb24Pixel *pixel = (BootRgb24Pixel *)screen;
            s16 chunk;
            u8 *greenPixel;
            if ((s16)row >= 0) {
                if ((s16)row >= 224) break;
                chunk = 0;
                greenPixel = &pixel->green;
                for (; chunk < 4; ++chunk) {
                    u32 bits = *source;
                    if (bits) {
                        BRIGHTEN_NIBBLE(0xF, 0);
                        BRIGHTEN_NIBBLE(0xF0, 4);
                        BRIGHTEN_NIBBLE(0xF00, 8);
                        BRIGHTEN_NIBBLE(0xF000, 12);
                    } else {
                        greenPixel += 12;
                        pixel += 4;
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

#undef BRIGHTEN_NIBBLE

void BootDisplay_SetImageDisplay(u16 index) {
    s32 displayY, drawY;
    u16 savedWidth;
    DRAWENV *drawArg;
    s32 zeroArg;
    s32 yArg;
    s32 widthArg;
    if (index != 0) {
        displayY = 0;
        drawY = 224;
    } else {
        displayY = 224;
        drawY = 0;
    }
    SetDefDispEnv(&g_RenderDispEnvArray[index], 0, displayY, 320, 224);
    drawArg = &g_RenderDrawEnvArray[index];
    zeroArg = 0;
    yArg = drawY;
    widthArg = 480;
    savedWidth = g_RenderDispEnvArray[index].disp.w;
    g_RenderDispEnvArray[index].isrgb24 = 1;
    g_RenderDispEnvArray[index].screen.y = 8;
    g_RenderDispEnvArray[index].screen.h = 224;
    g_RenderDispEnvArray[index].disp.w = savedWidth;
    SetDefDrawEnv(drawArg, zeroArg, yArg, widthArg, 224);
    g_RenderDrawEnvArray[index].dtd = 0;
    g_RenderDrawEnvArray[index].isbg = 0;
    g_RenderDrawEnvArray[index].clip.w = g_RenderDrawEnvArray[index].clip.w * 2 / 3;
}

void BootDisplay_StepImageDisplay(void) {
    s16 rect[4];
    rect[0] = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].x;
    rect[1] = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].y;
    rect[2] = 480;
    rect[3] = 224;
    StoreImage(rect, g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].pixels);
    DrawSync(0);
    if ((int)g_FieldPadBits < 0) {
        if (g_FieldPadPressed & 0x1000000) {
            u16 timer = (u16)g_BootDisplayStateTimer;
            if (timer != 0) {
                g_BootDisplayStateTimer = timer - 1;
                g_BootDisplayState = 0;
            }
        } else if (g_FieldPadPressed & 0x2000000) {
            u16 timer = (u16)g_BootDisplayStateTimer;
            if (timer < 30) {
                g_BootDisplayStateTimer = timer + 1;
                g_BootDisplayState = 0;
            }
        }
    }
    switch (g_BootDisplayState) {
    case 0: {
        s32 offset;
        s32 startOffset;
        s32 endOffset;
        char *base;

        DecDCTvlcSize2(MdecDiag_ReadHalfword(g_BootDisplayCreditImages[(u16)g_BootDisplayStateTimer]) >> 4);
        startOffset = 0x38800;
        endOffset = 0x43000;
        offset = (g_MainAssetLbaTbl[0] - g_BootDisplayDataLbaTbl) * 2048;
        base = g_CdLoadAreaBase;
        startOffset += offset;
        DecDCTvlc2(g_BootDisplayCreditImages[(u16)g_BootDisplayStateTimer], base + startOffset, base + (offset + endOffset));
        g_BootDisplayState = 1;
        break;
    }
    case 1: {
        s32 offset;
        offset = (g_MainAssetLbaTbl[0] - g_BootDisplayDataLbaTbl) * 2048;
        if (DecDCTvlc2(0, 0, g_CdLoadAreaBase + (offset + 0x43000)) == 0) {
            g_BootDisplayState = 2;
            g_BootDisplayTransferProgress = 0;
        }
        break;
    }
    case 2: {
        s32 offset;
        g_BootDisplayTransferCursor = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].decodeTarget;
        DecDCTReset(0);
        offset = (g_MainAssetLbaTbl[0] - g_BootDisplayDataLbaTbl) * 2048;
        DecDCTin(g_CdLoadAreaBase + (offset + 0x38800), 1);
        DecDCTout(g_BootDisplayTransferCursor, 0x3C00);
        g_BootDisplayState = 3;
        break;
    }
    case 3:
        if (DecDCToutSync(1) == 0) {
            g_BootDisplayState = 4;
            g_BootDisplayTransferProgress = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferX + 120;
        }
        break;
    case 4:
        if ((u16)g_BootDisplayTransferProgress < g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferX + 360) {
            rect[0] = g_BootDisplayTransferProgress;
            rect[1] = g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].transferY + 52;
            rect[2] = 24;
            rect[3] = 120;
            LoadImage(rect, g_BootDisplayTransferCursor);
            g_BootDisplayTransferCursor += 0x1800;
            g_BootDisplayTransferProgress += 24;
        } else g_BootDisplayState = 5;
        break;
    case 5:
        if (g_BootDisplayFrameCounter % 450 == 0) g_BootDisplayState = 6;
        break;
    case 6:
    case 7:
        break;
    }
}

void BootDisplay_CallInitSystem(void) {
    Boot_InitSystem();
}

void BootDisplay_BrightenImageRegion(void)
{
    s16 line;
    register u8 *image asm("$14") = &((u8 *)g_BootDisplayFontImagePixels)[0x4000];
    register u8 *screen asm("$13") = &((u8 *)g_BootDisplayImageBuffers[g_BootDisplayBufferIndex].pixels)[0x19DAC];

    for (line = 110; line < 134; line++) {
        s16 row;
        u16 *source = (u16 *)image;
        BootRgb24Pixel *pixel = (BootRgb24Pixel *)screen;

        for (row = 0; row < 24; row++) {
            u32 bits = *source;
            if (bits) {
                s16 column;
                column = 0;
                for (; column < 4; column++) {
                    int index = bits & 15;
                    if (index) {
                        int brightness = (((u16 *)g_BootDisplayFontClutPixels)[index] & 31) << 3;
                        u32 blue = pixel->blue;
                        register u32 green asm("$5") = pixel->green;
                        u32 red = pixel->red;
                        blue += brightness;
                        if (blue >= 256)
                            blue = 255;
                        green += brightness;
                        if (green >= 256)
                            green = 255;
                        red += brightness;
                        if (red >= 256)
                            red = 255;
                        pixel->blue = blue;
                        pixel->green = green;
                        pixel->red = red;
                    }
                    bits >>= 4;
                    ++pixel;
                }
            } else {
                pixel += 4;
            }
            source++;
        }
        image += 0x80;
        screen += 0x3C0;
    }
}
