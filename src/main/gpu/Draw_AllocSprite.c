/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/render_prim.h"
#include "pe1/menu_inventory.h"
void Draw_AllocSprite(int index) {
    RenderSpritePacket *sprite = 0;
    register DrawGlyphDescriptor *glyph asm("$18");
    u8 *old, *next;

    u32 *page;
    int mode;
    glyph = Draw_LookupGlyphDescriptor(index);
    /* Finish saving the descriptor before starting the allocation. */
    asm("" : : "r"(glyph));
    old = g_DrawPacketCursor;
    next = old + sizeof(RenderSpritePacket);
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        sprite = (RenderSpritePacket *)old;
    } else BoundsCheck_AssertStub(1);
    if (sprite) {
        if (g_DrawColorSelect) sprite->color.word = g_DrawAlternateColor;
        else sprite->color.word = g_DrawPrimaryColor;
        sprite->tag.bytes.length = 4;
        sprite->color.bytes.code = 0x64;
    }
    {
        register u32 oldTag asm("$5");
        register u32 maskTop asm("$6");
        register u32 mask24;
        u32 *ot;
        oldTag = sprite->tag.word;
        sprite->x = g_DrawSpriteX;
        sprite->y = g_DrawSpriteY;
        sprite->u = glyph->u;
        sprite->v = glyph->v;
        sprite->clut = glyph->clut;
        sprite->width = glyph->width;
        sprite->height = glyph->height;
        maskTop = 0xff000000;
        ot = g_DrawOrderingTableEntry;
        mask24 = 0xffffff;
        sprite->tag.word = (oldTag & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)sprite & mask24);
    }
    mode = glyph->mode;
    page = 0;
    old = g_DrawPacketCursor;
    next = old + 8;
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        page = (u32 *)old;
    } else BoundsCheck_AssertStub(1);
    if (page) {
        register char *drawPage asm("$4") = (char *)page;
        register int pageBits = (mode & 3) << 7;
        /* Prepare page bits before the two zero-valued call arguments. */
        asm("" : : "r"(pageBits), "r"(drawPage));
        SetDrawMode(drawPage, 0, 0, pageBits | 7);
    }
    {
        u32 mask24, maskTop;
        u32 *ot;
        mask24 = 0xffffff;
        maskTop = 0xff000000;
        ot = g_DrawOrderingTableEntry;
        *page = (*page & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)page & mask24);
    }
}
