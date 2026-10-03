#include "common.h"
#include "pe1/render_prim.h"
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

typedef RenderDrawModePacket DrawModePrim;
typedef RenderColorTilePacket ColorTilePrim;

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
    int w = width;
    int h = height;
    register ColorQuadPrim *first asm("$17");
    register ColorQuadPrim *packet asm("$10");
    ColorQuadPrim *second;
    register ColorTilePrim *tile asm("$16");
    register DrawModePrim *drawMode asm("$17");
    volatile int stackPad[2];
    u8 *old, *next;
    int shadeOffset;
    int ratio;
    int minimum;
    int widthSmaller;
    register u32 mask24 asm("$6");
    u32 maskTop;
    register u32 *ot asm("$3");
    u32 firstTag;
    int firstY;
    register int firstRight asm("$4");
    int firstBottom;
    int firstLeft;
    int secondRight;
    register int secondTop asm("$5");
    int secondBottom;
    int secondBound;
    int tileY;
    register u32 tileMask24 asm("$5");
    u32 tileMaskTop;
    int tileWidth;
    int tileHeight;
    int tileCode;
    register u32 modeMask24 asm("$4");
    u32 modeMaskTop;
    register u32 modeColor1 asm("$8");
    register u32 modeColor2 asm("$7");
    u32 *modeOt;
    u32 modeTag;
    u32 secondTag;
    register u32 linkHigh asm("$2");
    register u32 secondMaskTop asm("$5");
    int darkColor;
    int darkerColor;

    first = 0;
    shadeOffset = Battle_GetStateFlag1() == 0 ? 7 : 0;
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
        int secondMinimum = h;
        int scaled;
        if (widthSmaller) secondMinimum = w;
        scaled = secondMinimum * 56;
        ratio = scaled / h;
    }
    goto ratio_done;
ratio_zero:
    ratio = 0;
ratio_done:
    firstY = shadeOffset + 0xAA;
    firstRight = ratio + 0xE5;
    asm volatile("" : "=r"(firstRight) : "0"(firstRight));
    firstBottom = shadeOffset + 0xAD;
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
    darkColor = 0x303030;
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
        *(u32 *)&tile->r0 = D_8009D114;
        goto tile_color_done;
    tile_primary_color:
        *(u32 *)&tile->r0 = D_8009D110;
    tile_color_done:
        ((u8 *)tile)[3] = 3;
        tile->code = 0x60;
    }
    tileMask24 = 0xFFFFFF;
    tile->x0 = 0xDD;
    tileY = shadeOffset + 0xA6;
    tile->y0 = tileY;
    tileWidth = 0x54;
    tileHeight = 0x0B;
    tileMaskTop = 0xFF000000;
    tile->w = tileWidth;
    tileCode = tile->code;
    tile->h = tileHeight;
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
