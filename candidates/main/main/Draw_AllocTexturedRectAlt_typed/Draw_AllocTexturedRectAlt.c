/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/draw_level_bar.h"

static inline void SetCursor(u32 x, u32 y)
{
    D_8009D124 = x;
    D_8009D128 = y;
}

static inline void PushCursor(void)
{
    int *stack = g_TextCursorStack;
    if (stack < g_TextCursorStackTop) {
        int x = D_8009D124, y = D_8009D128;
        g_TextCursorStack = stack + 2;
        stack[0] = x;
        stack[1] = y;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

/* Moves the cursor and saves the new position. */
static inline void PushCursorAt(int x, int y)
{
    int *stack = g_TextCursorStack;

    SetCursor(x, y);
    if (stack < g_TextCursorStackTop) {
        stack[0] = x;
        stack[1] = y;
        g_TextCursorStack = stack + 2;
    } else {
        BoundsCheck_AssertStub(2);
    }
}

static inline void PopCursor(void)
{
    int *stack = g_TextCursorStack;
    if (g_TextCursorStackBottom < stack) {
        g_TextCursorStack = stack - 2;
        SetCursor(stack[-2], stack[-1]);
    } else {
        BoundsCheck_AssertStub(3);
    }
}

/* One textured quad of the bar: glyph texels starting `u` columns into the
 * glyph, from x to the right edge computed from the stored left edge. */
/* Allocates one bar quad from the packet arena and sets its colour and
 * command (0x2C: textured four-point polygon). */
static inline RenderTexturedQuad *AllocBarQuad(void)
{
    RenderTexturedQuad *prim = 0;
    u8 *old = D_8009D100;
    u8 *next = old + sizeof(RenderTexturedQuad);

    if (next < D_8009D104 + 0x4000) {
        D_8009D100 = next;
        prim = (RenderTexturedQuad *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (prim != 0) {
        if (D_8009D10C != 0)
            prim->color.word = D_8009D114;
        else
            prim->color.word = D_8009D110;
        prim->tag.bytes.length = 9;
        prim->color.bytes.code = 0x2C;
    }
    return prim;
}

/* One textured quad of the bar: glyph texels starting `u` columns into the
 * glyph, from `left` to the right edge computed from the stored left edge. */
#define DRAW_BAR_SEGMENT(left, right_from_x0, u_offset) \
    { \
        RenderTexturedQuad *prim = AllocBarQuad(); \
        u32 *ot; \
        prim->x0 = left; \
        prim->y0 = prim->y1 = D_8009D128 + 1; \
        prim->x2 = left; \
        prim->x1 = prim->x3 = right_from_x0; \
        prim->y2 = prim->y3 = prim->y0 + glyph->height; \
        prim->u0 = prim->u2 = glyph->u + (u_offset); \
        prim->v0 = prim->v1 = glyph->v; \
        prim->u1 = prim->u3 = prim->u0; \
        prim->v2 = prim->v3 = prim->v0 + glyph->height; \
        prim->clut = glyph->clut; \
        prim->tpage = 7; \
        ot = D_8009D11C; \
        prim->tag.word = (prim->tag.word & 0xFF000000) | (*ot & 0xFFFFFF); \
        *ot = (*ot & 0xFF000000) | ((u32)prim & 0xFFFFFF); \
    }

/* Draws a stat value followed by a 48-pixel level bar filled up to
 * 48 - width pixels, built from slices of glyph 0x48. */
void Draw_AllocTexturedRectAlt(int value, int width)
{
    DrawGlyphDescriptor *glyph = Draw_LookupGlyphDescriptor(0x48);
    RenderDrawModePacket *mode;
    u8 *old, *next;
    u32 *ot;

    PushCursor();
    PushCursorAt(D_8009D124, D_8009D128 + 2);
    SetCursor(D_8009D124 + 0x27, D_8009D128 - 2);
    Draw_PrintNumberWidth2Unk(value);
    D_8009D110 = 0x808080;
    D_8009D114 = 0x404040;
    PopCursor();
    Draw_AllocSprite(0x49);

    if (width < 0x2F)
        DRAW_BAR_SEGMENT(D_8009D124 + 1, prim->x0 + 1, 0);
    if (width < 0x2E)
        DRAW_BAR_SEGMENT(D_8009D124 + 2, prim->x0 + 0x2E - width, 1);
    if (width < 0x30)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x30 - width, prim->x0 + 1, 2);
    if (width > 0)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x31 - width, prim->x0 + 1, 3);
    if (width >= 3)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x32 - width, prim->x0 - 1 + width, 4);
    if (width >= 2)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x31, prim->x0 + 1, 5);
    PopCursor();

    mode = 0;
    old = D_8009D100;
    next = old + sizeof(RenderDrawModePacket);
    if (next < D_8009D104 + 0x4000) {
        D_8009D100 = next;
        mode = (RenderDrawModePacket *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (mode != 0)
        SetDrawMode((char *)mode, 0, 0, ((glyph->mode & 3) << 7) | 7);
    ot = D_8009D11C;
    mode->tag = (mode->tag & 0xFF000000) | (*ot & 0xFFFFFF);
    *ot = (*ot & 0xFF000000) | ((u32)mode & 0xFFFFFF);
}
