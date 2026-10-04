/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/render_prim.h"
#include "pe1/menu_inventory.h"


typedef struct ColorTriPrim {
    u32 tag;
    u8 r, g, b, code;
    u16 x0, y0, x1, y1, x2, y2;
} ColorTriPrim;

typedef struct {
    s16 y, x;
} __attribute__((packed)) DrawVertexPair;

typedef struct DrawVertexCoord {
    s16 y, x;
} DrawVertexCoord;

#define COORD(v) (*(DrawVertexCoord *)&(v))

typedef struct ColorTriPair {
    ColorTriPrim first, second;
} ColorTriPair;

extern u8 *D_8009D100;
extern u8 *D_8009D104;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern u32 *D_8009D11C;
extern DrawVertexPair D_800A22B0[];
extern u32 D_8009D14C[];
void BoundsCheck_AssertStub(int);

void Draw_AllocColorRect(int firstVertex, int secondVertex, int width, int mode) {
    ColorTriPair *pair = 0;
    ColorTriPair *p;
    u8 *end = D_8009D104;
    u8 *old = D_8009D100;
    u8 *next = old + sizeof(ColorTriPair);
    DrawVertexPair a, b, farA, farB;
    int magnitude;
    int color;
    u32 *ot;
    register u32 firstTag asm("$3");
    register u32 mask24 asm("$5");
    u32 maskTop;
    register u8 *cursor asm("$3");
    u8 *limit;
    int packetLen;
    int packetCode;
    int bx, fax, by, fay;

    magnitude = __builtin_abs(width);
    if (next < end + 0x4000) {
        D_8009D100 = next;
        pair = (ColorTriPair *)old;
    } else BoundsCheck_AssertStub(1);
    if (pair != 0 && (u32)pair < (u32)((u8 *)pair + sizeof(ColorTriPair))) {
        packetLen = 4;
        packetCode = 0x20;
        cursor = (u8 *)pair + 7;
        limit = (u8 *)pair + 0x2f;
        do {
            if (D_8009D10C) *(u32 *)(cursor - 3) = D_8009D114;
            else *(u32 *)(cursor - 3) = D_8009D110;
            cursor[-4] = packetLen;
            cursor[0] = packetCode;
            cursor += 0x14;
        } while (cursor < limit);
    }
    farA = D_800A22B0[firstVertex & 0x7f];
    a = farA;
    farB = D_800A22B0[secondVertex & 0x7f];
    b = farB;
    p = pair;
    p->first.code |= 2;
    p->second.code |= 2;
    color = D_8009D14C[mode];
    if (COORD(a).y == COORD(b).y) {
        COORD(farA).x += magnitude;
        COORD(farB).x += (secondVertex & 0x80) ? magnitude : -magnitude;
        COORD(farA).y += width;
        COORD(farB).y += width;
        *(u32 *)&p->first.r = (*(u32 *)&p->first.r & 0xff000000) | color;
        *(u32 *)&p->second.r = (*(u32 *)&p->second.r & 0xff000000) | color;
    } else {
        COORD(farA).x += width;
        COORD(farB).x += width;
        COORD(farA).y += magnitude;
        COORD(farB).y += (secondVertex & 0x80) ? magnitude : -magnitude;
        {
            register int halfMask asm("$2") = 0x7f7f7f;
            register u32 topMask asm("$5") = 0xff000000;
            register int faded asm("$4") = (color >> 1) & halfMask;
            *(u32 *)&p->first.r = (*(u32 *)&p->first.r & topMask) | faded;
            *(u32 *)&p->second.r = (*(u32 *)&p->second.r & topMask) | faded;
        }
    }
    p->first.x0 = COORD(a).x;
    bx = (u16)COORD(b).x;
    p->second.x2 = bx;
    p->first.x1 = bx;
    fax = (u16)COORD(farA).x;
    p->second.x1 = fax;
    p->first.x2 = fax;
    p->first.y0 = COORD(a).y;
    by = (u16)COORD(b).y;
    p->second.y2 = by;
    p->first.y1 = by;
    fay = (u16)COORD(farA).y;
    p->second.y1 = fay;
    p->first.y2 = fay;
    p->second.x0 = COORD(farB).x;
    p->second.y0 = COORD(farB).y;
    mask24 = 0xffffff;
    maskTop = 0xff000000;
    ot = D_8009D11C;
    firstTag = p->first.tag;
    p->first.tag = (firstTag & maskTop) | (*ot & mask24);
    *ot = (*ot & maskTop) | ((u32)&p->first & mask24);
    p->second.tag = (p->second.tag & maskTop) | (*ot & mask24);
    *ot = (*ot & maskTop) | ((u32)&p->second & mask24);
    PE1_COMPILER_USE(p);
}



extern int g_TextCursorX;
extern int g_TextCursorY;
extern u16 *g_DrawVertexWritePtr;
extern u8 D_800930A8[];

void BoundsCheck_AssertStub(int arg0);

static inline u32 *AllocateDrawMode(int mode) {
    u32 *packet = 0;
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + 8;
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        packet = (u32 *)old;
    } else BoundsCheck_AssertStub(1);
    if (packet) SetDrawMode((char *)packet, 0, 0, (mode & 3) << 5);
    return packet;
}

void Draw_EmitWipeBar(u8 *edges, int mode) {
    u32 *first = AllocateDrawMode(mode + 1);
    u32 *second = AllocateDrawMode(2 - mode);
    while ((s8)edges[0] >= 0) {
        Draw_AllocColorRect((s8)edges[0], (s8)edges[1], 2, mode);
        edges += 2;
    }
    ++edges;
    {
        u32 mask24 = 0xffffff, maskTop = 0xff000000;
        u32 *ot = g_DrawOrderingTableEntry;
        *first = (*first & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)first & mask24);
    }
    while ((s8)edges[0] >= 0) {
        Draw_AllocColorRect((s8)edges[0], (s8)edges[1], -2, !mode);
        edges += 2;
    }
    {
        u32 mask24 = 0xffffff, maskTop = 0xff000000;
        u32 *ot = g_DrawOrderingTableEntry;
        *second = (*second & maskTop) | (*ot & mask24);
        *ot = (*ot & maskTop) | ((u32)second & mask24);
    }
}

#define PUSH_WIPE_BAR_VERTEX(xValue, yValue)                     \
    {                                                           \
        int x = (xValue);                                       \
        int y = (yValue);                                       \
        u16 *out = g_DrawVertexWritePtr;                         \
                                                                \
        if ((unsigned int)out <                                  \
            (unsigned int)((u16 *)g_TextCursorStackTop + 0x18)) { \
            out[1] = x;                                          \
            g_DrawVertexWritePtr = out + 2;                      \
            out[0] = y;                                          \
        } else {                                                 \
            BoundsCheck_AssertStub(4);                           \
        }                                                       \
    }

void Draw_EmitWipeBarRect(int width, int height, int mode)
{
    g_DrawVertexWritePtr = (u16 *)g_TextCursorStackTop;

    PUSH_WIPE_BAR_VERTEX(g_TextCursorX, g_TextCursorY);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX + width, g_TextCursorY);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX, g_TextCursorY + height);
    PUSH_WIPE_BAR_VERTEX(g_TextCursorX + width, g_TextCursorY + height);

    Draw_EmitWipeBar(D_800930A8, mode);
}

void Draw_EmitWipeBarPoly(int arg0, int arg1, u8 *arg2) {
    u8 *cursor = arg2;
    u32 value;
    u16 *end;

    g_DrawVertexWritePtr = (u16 *)g_TextCursorStackTop;
    value = cursor[0];
    if (value < 0xFF) {
        end = (u16 *)g_TextCursorStackTop + 0x18;
        do {
            u16 *out = g_DrawVertexWritePtr;
            int x = value + g_TextCursorX;
            int y = g_TextCursorY + cursor[1];

            if ((unsigned int)out < (unsigned int)end) {
                out[1] = x;
                g_DrawVertexWritePtr = out + 2;
                out[0] = y;
            } else {
                BoundsCheck_AssertStub(4);
            }

            cursor += 2;
            value = cursor[0];
        } while (value < 0xFF);
    }

    Draw_EmitWipeBar(cursor + 1, 0);
}
#include "pe1/draw_wipe_rect.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

/* Wipe-bar panel over the polygon `points` ((x, y) byte pairs ending in
 * 0xFF, followed by its edge list) or, without points, over the
 * width x height rectangle at the text cursor. A textured panel also
 * gets a 4-bit sprite between two texture-window resets. */
/* Texture-window packet for `area`, linked into the ordering table. */
#define DRAW_ADD_TEXTURE_WINDOW(packet, area)               \
    {                                                       \
        RECT *rect = (area);                                \
                                                            \
        DRAW_ALLOC_PACKET(packet, sizeof(GpuCmdPacket));    \
        if ((packet).word)                                  \
            SetTexWindow((packet).window, rect);            \
        DRAW_LINK_PACKET(packet);                           \
    }

void Draw_AllocColorGradient(int width, int height, u8 *points, int textured)
{
    DrawPacketAddress windowPacket, sprite, mode;
    RECT window;

    if (points) {
        u8 *cursor = points;
        u32 value;

        g_DrawVertexWritePtr = (u16 *)g_TextCursorStackTop;
        value = cursor[0];
        if (value < 0xFF) {
            u16 *end = (u16 *)g_TextCursorStackTop + 0x18;

            do {
                DRAW_PUSH_WIPE_VERTEX(value + g_TextCursorX,
                                      g_TextCursorY + cursor[1], end);
                cursor += 2;
                value = cursor[0];
            } while (value < 0xFF);
        }
        Draw_EmitWipeBar(cursor + 1, 0);
    } else {
        g_DrawVertexWritePtr = (u16 *)g_TextCursorStackTop;
        DRAW_PUSH_WIPE_VERTEX(g_TextCursorX, g_TextCursorY,
                              (u16 *)g_TextCursorStackTop + 0x18);
        DRAW_PUSH_WIPE_VERTEX(g_TextCursorX + width, g_TextCursorY,
                              (u16 *)g_TextCursorStackTop + 0x18);
        DRAW_PUSH_WIPE_VERTEX(g_TextCursorX, g_TextCursorY + height,
                              (u16 *)g_TextCursorStackTop + 0x18);
        DRAW_PUSH_WIPE_VERTEX(g_TextCursorX + width, g_TextCursorY + height,
                              (u16 *)g_TextCursorStackTop + 0x18);
        Draw_EmitWipeBar(D_800930A8, 0);
    }

    if (textured) {
        window.x = 0;
        window.y = 0;
        window.w = 0;
        window.h = 0;
        DRAW_ADD_TEXTURE_WINDOW(windowPacket, &window);

        DRAW_ALLOC_PACKET(sprite, sizeof(RenderSpritePacket));
        if (sprite.word) {
            if (D_8009D10C)
                sprite.sprite->color.word = D_8009D114;
            else
                sprite.sprite->color.word = D_8009D110;
            sprite.sprite->tag.bytes.length = 4;
            sprite.sprite->color.bytes.code = 0x64;
        }
        sprite.sprite->x = g_TextCursorX;
        sprite.sprite->y = g_TextCursorY;
        sprite.sprite->u = 0;
        sprite.sprite->v = 0;
        sprite.sprite->clut = 0x391C;
        sprite.sprite->width = width;
        sprite.sprite->height = height;
        DRAW_LINK_PACKET(sprite);

        window.x = 0;
        window.y = 0;
        window.w = 0x20;
        window.h = 0x20;
        DRAW_ADD_TEXTURE_WINDOW(windowPacket, &window);
    }

    DRAW_ALLOC_PACKET(mode, sizeof(RenderDrawModePacket));
    if (mode.word)
        SetDrawMode(mode.bytes, 0, 0, 7);
    DRAW_LINK_PACKET(mode);
}
