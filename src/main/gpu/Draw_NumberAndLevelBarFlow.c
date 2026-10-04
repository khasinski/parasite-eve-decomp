/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --expand-div */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/render_prim.h"
#include "pe1/menu_inventory.h"
#include "pe1/text.h"
#include "pe1/draw_level_bar.h"

extern int D_8009D13C;
extern int D_8009D140;
extern int D_8009D144;

void Draw_AllocSprite(int arg0);

void Draw_EmitDigitSprite(int digit) {
    RenderTexturedQuad *quad = 0;
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + 40;

    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        quad = (RenderTexturedQuad *)old;
    } else {
        BoundsCheck_AssertStub(1);
    }
    if (quad) {
        if (g_DrawColorSelect) quad->color.word = g_DrawAlternateColor;
        else quad->color.word = g_DrawPrimaryColor;
        quad->tag.bytes.length = 9;
        quad->color.bytes.code = 0x2C;
    }
    {
        RenderTexturedQuad *p;
        u32 oldTag;
        u32 mask24;
        u16 x = *(u16 *)&g_DrawSpriteX, y = *(u16 *)&g_DrawSpriteY;
        p = quad;
        p->x0 = p->x2 = x;
        p->y0 = p->y1 = y;
        x = p->x0 + 5;
        /* Narrow volatile views preserve the retail packet-field reloads. */
        y = *(volatile u16 *)&p->y0 + 7;
        p->x1 = p->x3 = x;
        p->y2 = p->y3 = y;
        if (digit >= 0) p->u2 = g_DrawDigitFontBaseTexU + (digit % 10) * 5;
        else p->u2 = 88;
        {
            u8 value = p->u2;
            p->u2 = value;
            p->u0 = value;
        }
        if (digit >= 0) p->v1 = g_DrawDigitFontBaseTexV;
        else p->v1 = 164;
        mask24 = 0xFFFFFF;
        oldTag = *(volatile u32 *)&p->tag.word;
        {
            u8 value = *(volatile u8 *)&p->v1;
            int rightU = *(volatile u8 *)&p->u0;
            int bottomV;
            p->v0 = value;
            bottomV = *(volatile u8 *)&p->v0;
            rightU += 5;
            p->v1 = value;
            p->u1 = p->u3 = rightU;
            p->v2 = p->v3 = bottomV + 7;
        }
        p->clut = g_DrawDigitFontTpageClut;
        p->tpage = 7;
        {
            u32 maskTop = 0xFF000000;
            u32 *ot = g_DrawOrderingTableEntry;
            p->tag.word = (oldTag & maskTop) | (*ot & mask24);
            mask24 &= (u32)p;
            *ot = (*ot & maskTop) | mask24;
        }
    }
}

void Draw_PrintNumberWidth2Unk(int value) {
    int width = 2;
    int place = 1;
    int i;
    int x;
    int y;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 1;
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintNumberWidth3Unk(int value) {
    int width = 3;
    int place = 1;
    int i;
    int x;
    int y;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 2;
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintSignedNumberWidth3(int value) {
    int width = 3;
    int place = 1;
    int i;
    int x;
    int y;
    int sprite;

    if (value < 0) {
        value = -value;
        sprite = 0x52;
    } else {
        if (value <= 0) {
            goto digits;
        }
        sprite = 0x89;
    }
    Draw_AllocSprite(sprite);
    width = 2;
    x = g_DrawSpriteX;
    y = g_DrawSpriteY;
    g_DrawSpriteX = x + 5;
    g_DrawSpriteY = y;
digits:
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintNumberWidth4Unk(int value) {
    int width = 4;
    int place = 1;
    int i;
    int x;
    int y;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 3;
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintSignedNumberWidth4(int value) {
    int width = 4;
    int place = 1;
    int i;
    int x;
    int y;
    int sprite;

    if (value < 0) {
        value = -value;
        sprite = 0x52;
    } else {
        if (value <= 0) {
            goto digits;
        }
        sprite = 0x89;
    }
    Draw_AllocSprite(sprite);
    width = 3;
    x = g_DrawSpriteX;
    y = g_DrawSpriteY;
    g_DrawSpriteX = x + 5;
    g_DrawSpriteY = y;
digits:
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        x = g_DrawSpriteX;
        y = g_DrawSpriteY;
        g_DrawSpriteX = x + 5;
        g_DrawSpriteY = y;
        place /= 10;
    }
}

void Draw_PrintTimeValue(int value, int flag) {
    int x;
    int y;

    if (value > 360000) {
        value = 359999;
    }
    D_8009D13C = flag != 0 ? 0x3A1C : 0x395D;
    D_8009D140 = flag != 0 ? 0xCC : 0x84;
    D_8009D144 = 0xA4;

    Draw_EmitDigitSprite(value / 36000);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value / 3600);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    if (flag) { Draw_AllocSprite(0x9B); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 3; }
    else      { Draw_AllocSprite(0x4F); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 5; }
    g_DrawSpriteX = x; g_DrawSpriteY = y;
    PE1_COMPILER_MEMORY_BARRIER();

    Draw_EmitDigitSprite((value / 600) % 6);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value / 60);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    if (flag) { Draw_AllocSprite(0x9B); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 3; }
    else      { Draw_AllocSprite(0x4F); x = g_DrawSpriteX; y = g_DrawSpriteY; x = x + 5; }
    g_DrawSpriteX = x; g_DrawSpriteY = y;
    PE1_COMPILER_MEMORY_BARRIER();

    Draw_EmitDigitSprite((value / 10) % 6);
    x = g_DrawSpriteX; y = g_DrawSpriteY; g_DrawSpriteX = x + 5; g_DrawSpriteY = y;

    Draw_EmitDigitSprite(value);

    D_8009D13C = 0x395D;
    D_8009D140 = 0x84;
    D_8009D144 = 0xA4;
}

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

static inline void PrintSign(u8 *text)
{
    if (text) {
        PushCursor();
        while (*text != 255) {
            Draw_AllocTexturedQuad(*text++);
        }
        PopCursor();
    }
}

static inline int Digit(int leading, int value)
{
    if (!leading || value != 0) {
        return (u8)(value % 10);
    }
    return 15;
}

/* Print a signed decimal field; glyph 15 is padding, table entry 0x70 the
 * minus sign. The sign occupies one place and restores its own text cursor.
 * Retail callers use bounded widths; powers of ten and negation must fit int.
 */
void Draw_AllocTexturedRect(int value, int width)
{
    int divisor = 1;
    int i;

    for (i = 1; i < width; i++) {
        divisor *= 10;
    }
    if (value < 0) {
        value = -value;
        divisor /= 10;
        width--;
        while (value < divisor) {
            Draw_AllocTexturedQuad(15);
            divisor /= 10;
            width--;
        }
        PrintSign(Str_LookupTable4(0x70));
        SetCursor(D_8009D124 + 9u, D_8009D128);
    }
    for (i = 0; i < width; i++) {
        Draw_AllocTexturedQuad(Digit(i < width - 1, value / divisor));
        divisor /= 10;
    }
}

void Draw_PrintNumberWidth6(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 6);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 6;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth5(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 5);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 5;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth4(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 4);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 4;
    D_8009D128 = y;
}

void func_800605C4(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 3);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 3;
    D_8009D128 = y;
}

void Draw_PrintNumberWidth2(int value) {
    int x;
    int y;

    Draw_AllocTexturedRect(value, 2);
    x = D_8009D124;
    y = D_8009D128;
    D_8009D124 = x + 2;
    D_8009D128 = y;
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
        u32 *ot; \
        quad = AllocBarQuad(); \
        quad->x0 = quad->x2 = left; \
        quad->y0 = quad->y1 = D_8009D128 + 1; \
        quad->x1 = quad->x3 = right_from_x0; \
        quad->y2 = quad->y3 = quad->y0 + glyph->height; \
        quad->u0 = quad->u2 = glyph->u + (u_offset); \
        quad->v0 = quad->v1 = glyph->v; \
        quad->u1 = quad->u3 = quad->u0; \
        quad->v2 = quad->v3 = quad->v0 + glyph->height; \
        quad->clut = glyph->clut; \
        quad->tpage = 7; \
        quad->tag.link.address = *D_8009D11C; \
        ot = D_8009D11C; \
        link.quad = quad; \
        *ot = (*ot & 0xFF000000) | (link.word & 0xFFFFFF); \
    }

/* Draws a stat value followed by a 48-pixel level bar filled up to
 * 48 - width pixels, built from slices of glyph 0x48. */
void Draw_AllocTexturedRectAlt(int value, int width)
{
    DrawGlyphDescriptor *glyph = Draw_LookupGlyphDescriptor(0x48);
    RenderDrawModePacket *mode;
    RenderTexturedQuad *quad;
    int drawMode;
    DrawLevelBarLink link;
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
        DRAW_BAR_SEGMENT(D_8009D124 + 1, quad->x0 + 1, 0);
    if (width < 0x2E)
        DRAW_BAR_SEGMENT(D_8009D124 + 2, (s16)quad->x0 + 0x2E - width, 1);
    if (width < 0x30)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x30 - width, quad->x0 + 1, 2);
    if (width > 0)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x31 - width, quad->x0 + 1, 3);
    if (width >= 3)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x32 - width, (s16)quad->x0 - 1 + width, 4);
    if (width >= 2)
        DRAW_BAR_SEGMENT(D_8009D124 + 0x31, quad->x0 + 1, 5);
    PopCursor();

    drawMode = glyph->mode;
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
        SetDrawMode((char *)mode, 0, 0, ((drawMode & 3) << 7) | 7);
    ot = D_8009D11C;
    mode->tag = (mode->tag & 0xFF000000) | (*ot & 0xFFFFFF);
    link.mode = mode;
    *ot = (*ot & 0xFF000000) | (link.word & 0xFFFFFF);
}
