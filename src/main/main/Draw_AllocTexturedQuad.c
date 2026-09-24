#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct TexturedQuadPrim {
    u32 tag;
    u8 r0, g0, b0, code;
    u16 x0, y0;
    u8 u0, v0;
    u16 clut;
    u16 x1, y1;
    u8 u1, v1;
    u16 tpage;
    u16 x2, y2;
    u8 u2, v2;
    u16 pad2;
    u16 x3, y3;
    u8 u3, v3;
    u16 pad3;
} TexturedQuadPrim;

extern u8 *D_8009D100;
extern u8 *D_8009D104;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern u32 *D_8009D11C;
extern int D_8009D124;
extern int D_8009D128;
extern int D_8009CDB0;
extern int D_8009D0D8;

int Draw_LookupGlyphMetrics(int glyph);
void Draw_AllocSprite(int glyph);
u16 GetTPage(int tp, int abr, int x, int y);
void BoundsCheck_AssertStub(int arg0);

void Draw_AllocTexturedQuad(int code) {
    int low;
    int glyph;
    register int raw asm("$3") = code;
    register int current asm("$4");
    int metrics;
    int width;
    register int nibble asm("$3");
    int u;
    int v;
    register int spacing asm("$5");
    int column;
    int row;
    register int arg0 asm("$4");
    TexturedQuadPrim *prim;
    u8 *old, *next;

    current = raw & 0xFF;
    if (D_8009D0D8 != 0) {
        asm volatile("" : "=r"(current) : "0"(current));
        current += D_8009D0D8 << 8;
        D_8009D0D8 = 0;
    }

    asm volatile("" : "=r"(raw) : "0"(raw));
    low = raw & 0xFF;
    if ((u32)low >= 0xFA) {
        D_8009D0D8 = low - 0xFA;
        current = -1;
    }

    glyph = current;
    if (glyph == 0x100) {
        int x, y;
        Draw_AllocSprite(0x77);
        x = D_8009D124;
        y = *(volatile int *)&D_8009D128;
        D_8009D124 = x + 0xC;
        *(volatile int *)&D_8009D128 = y;
        return;
    }

    if (glyph < 0) {
        return;
    }

    if (glyph >= 0x101) {
        glyph -= 0x13;
    }

    glyph %= 0x1B9;
    row = glyph / 0x15;
    column = glyph - row * 0x15;

    prim = 0;
    old = D_8009D100;
    next = old + sizeof(TexturedQuadPrim);
    if (next >= D_8009D104 + 0x4000) goto alloc_fail;
    D_8009D100 = next;
    prim = (TexturedQuadPrim *)old;
    goto alloc_done;
alloc_fail:
    BoundsCheck_AssertStub(1);
alloc_done:
    if (prim != 0) {
        if (D_8009D10C == 0) goto primary_color;
        *(u32 *)&prim->r0 = D_8009D114;
        goto color_done;
    primary_color:
        *(u32 *)&prim->r0 = D_8009D110;
    color_done:
        ((u8 *)prim)[3] = 9;
        prim->code = 0x2C;
    }

    metrics = Draw_LookupGlyphMetrics(glyph);
    nibble = metrics & 0xF;
    asm volatile("" : : "r"(nibble));
    width = (metrics >> 4) & 0xF;
    spacing = 0;
    if (glyph < 10 || glyph == 15) {
        spacing = 1;
    }
    u = column * 0xC + nibble;
    v = row * 0xC;
    asm volatile("" : : "r"(u), "r"(v));
    {
        int uBase;
        int vBase;
        register int half asm("$5");
        register int cursorX asm("$6");
        register int cursorY asm("$7");
        arg0 = 0;
        asm volatile("" : : "r"(arg0));
        cursorX = D_8009D124;
        cursorY = D_8009D128;
        spacing++;
        D_8009CDB0 = spacing;

        prim->u2 = u;
        prim->u0 = u;
        prim->clut = 0x89C;
        asm volatile("" : "=r"(spacing) : "0"(spacing));
        half = spacing >> 1;
        prim->v1 = v;
        prim->v0 = v;
        /* Use the bytes stored in the packet after truncation. */
        uBase = *(volatile u8 *)&prim->u0;
        vBase = *(volatile u8 *)&prim->v0;
        uBase += width;
        vBase += 0xC;
        cursorX += half;
        cursorY++;
        asm volatile("" : "=r"(cursorX), "=r"(cursorY) : "0"(cursorX), "1"(cursorY));
        prim->u3 = uBase;
        prim->u1 = prim->u3;
        prim->x0 = cursorX;
        prim->x2 = prim->x0;
        prim->y1 = cursorY;
        prim->y0 = prim->y1;
        prim->v3 = vBase;
        prim->v2 = prim->v3;
        prim->x3 = prim->x0 + width;
        prim->x1 = prim->x3;
        prim->y3 = prim->y0 + 0xC;
        prim->y2 = prim->y3;
    }
    {
        u16 tpage = GetTPage(arg0, 0, 0x140, 0);
        register u32 mask24 asm("$6") = 0xFFFFFF;
        register u32 maskTop asm("$8") = 0xFF000000;
        register u32 tag asm("$3") = prim->tag;
        register u32 *ot asm("$7") = D_8009D11C;
        register int y asm("$5") = *(volatile int *)&D_8009D128;
        u32 otValue, linkedTag;
        register u32 otNew asm("$4");
        int newX;
        register int extra asm("$3");

        prim->tpage = tpage;
        otValue = *ot;
        *(volatile int *)&D_8009D128 = y;
        linkedTag = (tag & maskTop) | (otValue & mask24);
        asm volatile("" : : "r"(linkedTag));
        mask24 &= (u32)prim;
        prim->tag = linkedTag;
        asm volatile("" ::: "memory");
        extra = D_8009CDB0;
        otNew = *ot;
        extra = width + extra;
        newX = D_8009D124 + extra;
        asm volatile("" : "=r"(newX) : "0"(newX));
        otNew = (otNew & maskTop) | mask24;
        D_8009D124 = newX;
        *ot = otNew;
    }
}
