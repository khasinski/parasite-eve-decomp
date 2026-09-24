#include "common.h"
/* CC1_FLAGS: -G8 -fno-schedule-insns */
/* MASPSX_FLAGS: -G8 --expand-div */

typedef struct ColorQuadPrim {
    u32 tag;
    u8 r0, g0, b0, code;
    u16 x0, y0;
    u8 r1, g1, b1, pad1;
    u16 x1, y1;
    u8 r2, g2, b2, pad2;
    u16 x2, y2;
    u8 r3, g3, b3, pad3;
    u16 x3, y3;
} ColorQuadPrim;

typedef struct DrawModePrim {
    u32 tag;
    u32 command;
} DrawModePrim;

typedef struct ColorTilePrim {
    u32 tag;
    u8 r, g, b, code;
    u16 x, y, width, height;
} ColorTilePrim;

extern u8 *D_8009D100;
extern u8 *D_8009D104;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern u32 *D_8009D11C;

int Battle_GetStateFlag1(void);
void BoundsCheck_AssertStub(int arg0);
void SetDrawMode(char *packet, int drawTexture, int dither, int tpage);

void Draw_AllocColorQuad(int width, int height) {
    register int w asm("$18") = width;
    register int h asm("$16") = height;
    register ColorQuadPrim *first asm("$17");
    register ColorQuadPrim *packet asm("$10");
    register ColorQuadPrim *second asm("$16");
    register ColorTilePrim *tile asm("$16");
    register DrawModePrim *drawMode asm("$17");
    volatile int stackPad[2];
    u8 *old, *next;
    register int shadeOffset asm("$19");
    register int ratio asm("$17");
    int minimum;
    register int widthSmaller asm("$4");
    register u32 mask24 asm("$6");
    register u32 maskTop asm("$4");
    register u32 *ot asm("$3");
    register u32 firstTag asm("$5");
    register int firstY asm("$3");
    register int firstRight asm("$4");
    register int firstBottom asm("$7");
    register int firstLeft asm("$2");
    register int secondRight asm("$3");
    register int secondTop asm("$5");
    register int secondBottom asm("$7");
    register int secondBound asm("$2");
    register int tileY asm("$2");
    register u32 tileMask24 asm("$5");
    register u32 tileMaskTop asm("$6");
    register int tileWidth asm("$2");
    register int tileHeight asm("$3");
    register int tileCode asm("$2");
    register u32 modeMask24 asm("$4");
    register u32 modeMaskTop asm("$6");
    register u32 modeColor1 asm("$8");
    register u32 modeColor2 asm("$7");
    register u32 *modeOt asm("$5");
    register u32 modeTag asm("$3");
    register u32 secondTag asm("$4");
    register u32 linkHigh asm("$2");
    register u32 secondMaskTop asm("$5");
    register int darkColor asm("$9");
    register int darkerColor asm("$8");

    first = 0;
    shadeOffset = Battle_GetStateFlag1() == 0 ? 7 : 0;
    asm volatile("" : : "r"(shadeOffset));
    old = D_8009D100;
    next = old + sizeof(ColorQuadPrim);
    if (next >= D_8009D104 + 0x4000) goto first_fail;
    D_8009D100 = next;
    first = (ColorQuadPrim *)old;
    goto first_done;
first_fail:
    BoundsCheck_AssertStub(1);
first_done:
    if (first != 0) {
        *(u32 *)&first->r0 = 0x808080;
        ((u8 *)first)[3] = 8;
        first->code = 0x38;
    }

    packet = first;
    if (h <= 0) goto ratio_zero;
    widthSmaller = w < h;
    minimum = h;
    if (widthSmaller) minimum = w;
    if (minimum < 0) goto ratio_zero;
    {
        register int secondMinimum asm("$3") = h;
        register int scaled asm("$2");
        if (widthSmaller) secondMinimum = w;
        scaled = secondMinimum * 56;
        ratio = scaled / h;
    }
    goto ratio_done;
ratio_zero:
    ratio = 0;
ratio_done:
    firstY = shadeOffset + 0xAA;
    asm volatile("" : "=r"(firstY) : "0"(firstY));
    firstRight = ratio + 0xE5;
    asm volatile("" : "=r"(firstRight) : "0"(firstRight));
    firstBottom = shadeOffset + 0xAD;
    asm volatile("" : "=r"(firstBottom) : "0"(firstBottom));
    mask24 = 0xFFFFFF;
    firstTag = packet->tag;

    packet->g2 = 0x82;
    packet->g0 = 0x82;
    packet->b2 = 0x36;
    packet->b0 = 0x36;
    packet->r3 = 0x4A;
    packet->r1 = 0x4A;
    packet->g3 = 0xFF;
    packet->g1 = 0xFF;
    packet->b3 = 0x3B;
    packet->b1 = 0x3B;
    asm volatile("" ::: "memory");
    firstLeft = 0xE5;
    packet->x3 = firstRight;
    packet->x1 = packet->x3;
    packet->r2 = 0;
    packet->r0 = 0;
    packet->y1 = firstY;
    packet->y0 = packet->y1;
    packet->x2 = firstLeft;
    packet->x0 = firstLeft;
    packet->y3 = firstBottom;
    packet->y2 = packet->y3;

    maskTop = 0xFF000000;
    ot = D_8009D11C;
    packet->tag = (firstTag & maskTop) | (*ot & mask24);
    linkHigh = *ot & maskTop;
    old = D_8009D100;
    *ot = linkHigh | ((u32)packet & mask24);

    second = 0;
    next = old + sizeof(ColorQuadPrim);
    if (next >= D_8009D104 + 0x4000) goto second_fail;
    D_8009D100 = next;
    second = (ColorQuadPrim *)old;
    goto second_done;
second_fail:
    BoundsCheck_AssertStub(1);
second_done:
    if (second != 0) {
        *(u32 *)&second->r0 = 0x808080;
        ((u8 *)second)[3] = 8;
        second->code = 0x38;
    }

    packet = second;
    secondRight = ratio + 0xE5;
    asm volatile("" : "=r"(secondRight) : "0"(secondRight));
    secondTop = shadeOffset + 0xAA;
    asm volatile("" : "=r"(secondTop) : "0"(secondTop));
    secondBottom = shadeOffset + 0xAD;
    asm volatile("" : "=r"(secondBottom) : "0"(secondBottom));
    mask24 = 0xFFFFFF;
    asm volatile("" : "=r"(mask24) : "0"(mask24));
    darkColor = 0x303030;
    asm volatile("" : "=r"(darkColor) : "0"(darkColor));
    darkerColor = 0x181818;
    asm volatile("" : "=r"(darkerColor) : "0"(darkerColor));
    secondTag = packet->tag;
    packet->r2 = 0xFF;
    packet->r0 = 0xFF;
    packet->g2 = 0x3D;
    packet->g0 = 0x3D;
    packet->b2 = 0x81;
    packet->b0 = 0x81;
    packet->r3 = 0x83;
    packet->r1 = 0x83;
    packet->g3 = 0x13;
    packet->g1 = 0x13;
    packet->b3 = 1;
    packet->b1 = 1;
    asm volatile("" ::: "memory");
    secondBound = 0x11D;
    packet->y1 = secondTop;
    packet->y0 = packet->y1;
    packet->x2 = secondRight;
    packet->x0 = packet->x2;
    packet->x3 = secondBound;
    packet->x1 = secondBound;
    packet->y3 = secondBottom;
    packet->y2 = packet->y3;

    D_8009D110 = darkColor;
    D_8009D114 = darkerColor;
    D_8009D10C = 0;
    secondMaskTop = 0xFF000000;
    ot = D_8009D11C;
    packet->tag = (secondTag & secondMaskTop) | (*ot & mask24);
    linkHigh = *ot & secondMaskTop;
    old = D_8009D100;
    *ot = linkHigh | ((u32)packet & mask24);

    tile = 0;
    next = old + sizeof(ColorTilePrim);
    if (next >= D_8009D104 + 0x4000) goto tile_fail;
    D_8009D100 = next;
    tile = (ColorTilePrim *)old;
    goto tile_done;
tile_fail:
    BoundsCheck_AssertStub(1);
tile_done:
    if (tile != 0) {
        if (D_8009D10C == 0) goto tile_primary_color;
        *(u32 *)&tile->r = D_8009D114;
        goto tile_color_done;
    tile_primary_color:
        *(u32 *)&tile->r = D_8009D110;
    tile_color_done:
        ((u8 *)tile)[3] = 3;
        tile->code = 0x60;
    }
    tileMask24 = 0xFFFFFF;
    asm volatile("" : "=r"(tileMask24) : "0"(tileMask24));
    tile->x = 0xDD;
    asm volatile("" ::: "memory");
    tileY = shadeOffset + 0xA6;
    asm volatile("" : "=r"(tileY) : "0"(tileY));
    tile->y = tileY;
    tileWidth = 0x54;
    tileHeight = 0x0B;
    asm volatile("" : "=r"(tileWidth) : "0"(tileWidth));
    asm volatile("" : "=r"(tileHeight) : "0"(tileHeight));
    tileMaskTop = 0xFF000000;
    asm volatile("" : "=r"(tileMaskTop) : "0"(tileMaskTop));
    tile->width = tileWidth;
    asm volatile("" ::: "memory");
    tileCode = tile->code;
    tile->height = tileHeight;
    tile->code = tileCode | 2;
    ot = D_8009D11C;
    tile->tag = (tile->tag & tileMaskTop) | (*ot & tileMask24);
    linkHigh = *ot & tileMaskTop;
    old = D_8009D100;
    *ot = linkHigh | ((u32)tile & tileMask24);

    drawMode = 0;
    next = old + sizeof(DrawModePrim);
    if (next >= D_8009D104 + 0x4000) goto mode_fail;
    D_8009D100 = next;
    drawMode = (DrawModePrim *)old;
    goto mode_done;
mode_fail:
    BoundsCheck_AssertStub(1);
mode_done:
    if (drawMode != 0) {
        SetDrawMode((char *)drawMode, 0, 0, 0);
    }
    modeMask24 = 0xFFFFFF;
    modeColor1 = 0x808080;
    modeColor2 = 0x400000;
    asm volatile("" : "=r"(modeColor2) : "0"(modeColor2));
    modeMaskTop = 0xFF000000;
    modeTag = drawMode->tag;
    modeOt = D_8009D11C;
    modeColor2 |= 0x4040;
    D_8009D110 = modeColor1;
    D_8009D114 = modeColor2;
    drawMode->tag = (modeTag & modeMaskTop) | (*modeOt & modeMask24);
    *modeOt = (*modeOt & modeMaskTop) | ((u32)drawMode & modeMask24);
}
