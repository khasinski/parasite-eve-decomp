#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef struct TilePrim {
    u32 tag;
    u8 r0, g0, b0, code;
    u16 x0, y0;
    u16 w, h;
} TilePrim;

typedef struct DrawModePrim {
    u32 tag;
    u32 code;
} DrawModePrim;

extern u8 *D_8009D100;
extern u8 *D_8009D104;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern u32 *D_8009D11C;
extern int D_8009D124;
extern int D_8009D128;

int VSync(int mode);
void SetDrawMode(void *packet, int dfe, int dtd, int tpage);
void BoundsCheck_AssertStub(int arg0);

void Draw_AllocColorTri(int width, int height, int pulse) {
    register TilePrim *tile asm("$16");
    register DrawModePrim *drawMode asm("$17");
    int shade;
    int sync;
    u8 *old, *next;

    tile = 0;
    old = D_8009D100;
    next = old + sizeof(TilePrim);
    if (next >= D_8009D104 + 0x4000) goto tile_fail;
    D_8009D100 = next;
    tile = (TilePrim *)old;
    goto tile_done;
tile_fail:
    BoundsCheck_AssertStub(1);
tile_done:
    if (tile != 0) {
        if (D_8009D10C == 0) goto primary_color;
        *(u32 *)&tile->r0 = D_8009D114;
        goto color_done;
    primary_color:
        *(u32 *)&tile->r0 = D_8009D110;
    color_done:
        ((u8 *)tile)[3] = 3;
        tile->code = 0x60;
    }

    if (pulse != 0) {
        u8 channelShade;
        sync = VSync(-1);
        if (sync & 0x20) {
            shade = (sync & 0x1F) << 1;
        } else {
            sync &= 0x1F;
            shade = 0x40 - (sync << 1);
        }
        tile->b0 = shade;
        channelShade = *(volatile u8 *)&tile->b0;
        tile->b0 = channelShade;
        tile->g0 = channelShade;
        tile->r0 = channelShade;
    } else {
        register u32 noPulseMask asm("$4") = 0xFFFFFF;
        u32 colorTop = *(u32 *)&tile->r0 & 0xFF000000;
        *(volatile u32 *)&tile->r0 = colorTop;
        *(u32 *)&tile->r0 = colorTop | (D_8009D114 & noPulseMask);
    }

    {
        register u32 tag asm("$4");
        register u32 mask24 asm("$5") = 0xFFFFFF;
        register u32 maskTop asm("$6");
        register int w asm("$2") = width - 4;
        register int h asm("$3") = height - 4;
        register int y asm("$3");
        register int x asm("$2");
        u8 code;
        u32 linkedTag, lowTile, otValue;
        u32 *ot;
        asm volatile("" : : "r"(w), "r"(h));
        tag = tile->tag;
        asm volatile("" : : "r"(tag));
        maskTop = 0xFF000000;
        tile->w = w;
        asm volatile("" ::: "memory");
        code = tile->code;
        tile->h = h;
        code |= 2;
        tile->code = code;
        y = D_8009D128;
        x = D_8009D124;
        tile->y0 = y + 2;
        ot = D_8009D11C;
        tile->x0 = x + 2;
        otValue = *ot;
        asm volatile("" : "=r"(tag) : "0"(tag), "r"(otValue));
        linkedTag = (tag & maskTop) | (otValue & mask24);
        lowTile = (u32)tile & mask24;
        asm volatile("" : : "r"(lowTile) : "memory");
        tile->tag = linkedTag;
        old = D_8009D100;
        *ot = (*ot & maskTop) | lowTile;
    }

    drawMode = 0;
    next = old + sizeof(DrawModePrim);
    if (next >= D_8009D104 + 0x4000) goto draw_mode_fail;
    D_8009D100 = next;
    drawMode = (DrawModePrim *)old;
    goto draw_mode_done;
draw_mode_fail:
    BoundsCheck_AssertStub(1);
draw_mode_done:
    if (drawMode != 0) {
        SetDrawMode(drawMode, 0, 0, 0x20);
    }
    {
        register u32 mask24 asm("$4") = 0xFFFFFF;
        register u32 maskTop asm("$6") = 0xFF000000;
        register u32 *ot asm("$5") = D_8009D11C;
        drawMode->tag = (drawMode->tag & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)drawMode & mask24);
    }
}
