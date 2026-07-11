/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Rect {
    s16 x, y, w, h;
} Rect;

typedef struct TexWindowPrim {
    u32 tag;
    u32 code0;
    u32 code1;
} TexWindowPrim;

typedef struct SpritePrim {
    u32 tag;
    u8 r0, g0, b0, code;
    u16 x0, y0;
    u8 u0, v0;
    u16 clut;
    u16 w, h;
} SpritePrim;

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
extern u16 g_TextCursorStackTop[];
extern u16 *g_DrawVertexWritePtr;
extern u8 D_800930A8[];

void SetTexWindow(void *packet, Rect *rect);
void SetDrawMode(void *packet, int dfe, int dtd, int tpage);
void Draw_EmitWipeBar(u8 *arg0, int arg1);
void BoundsCheck_AssertStub(int arg0);

#define ALLOC_PACKET(packet, type, size)                       \
    do {                                                       \
        u8 *old = D_8009D100;                                  \
        u8 *next = old + (size);                               \
        packet = 0;                                            \
        if (next < D_8009D104 + 0x4000) {                      \
            D_8009D100 = next;                                 \
            packet = (type *)old;                              \
        } else {                                               \
            BoundsCheck_AssertStub(1);                         \
        }                                                      \
    } while (0)

#define LINK_PACKET(packet)                                                        \
    do {                                                                          \
        u32 *ot = D_8009D11C;                                                      \
        (packet)->tag = ((packet)->tag & 0xFF000000) | (*ot & 0x00FFFFFF);         \
        *ot = (*ot & 0xFF000000) | ((u32)(packet) & 0x00FFFFFF);                  \
    } while (0)

#define PUSH_WIPE_BAR_VERTEX(x, y)                         \
    do {                                                   \
        u16 *out = g_DrawVertexWritePtr;                   \
                                                           \
        if ((u32)out < (u32)(g_TextCursorStackTop + 0x18)) { \
            out[1] = (x);                                  \
            g_DrawVertexWritePtr = out + 2;                \
            out[0] = (y);                                  \
        } else {                                           \
            BoundsCheck_AssertStub(4);                     \
        }                                                  \
    } while (0)

void Draw_AllocColorGradient(int width, int height, u8 *points, int enableMask) {
    u8 *cursor;
    Rect rect;
    TexWindowPrim *texWindow;
    SpritePrim *sprite;
    DrawModePrim *drawMode;

    if (points != 0) {
        cursor = points;
        g_DrawVertexWritePtr = g_TextCursorStackTop;
        while (cursor[0] < 0xFF) {
            PUSH_WIPE_BAR_VERTEX(D_8009D124 + cursor[0], D_8009D128 + cursor[1]);
            cursor += 2;
        }
        Draw_EmitWipeBar(cursor + 1, 0);
    } else {
        g_DrawVertexWritePtr = g_TextCursorStackTop;
        PUSH_WIPE_BAR_VERTEX(D_8009D124, D_8009D128);
        PUSH_WIPE_BAR_VERTEX(D_8009D124 + width, D_8009D128);
        PUSH_WIPE_BAR_VERTEX(D_8009D124, D_8009D128 + height);
        PUSH_WIPE_BAR_VERTEX(D_8009D124 + width, D_8009D128 + height);
        Draw_EmitWipeBar(D_800930A8, 0);
    }

    if (enableMask != 0) {
        rect.x = 0;
        rect.y = 0;
        rect.w = 0;
        rect.h = 0;
        ALLOC_PACKET(texWindow, TexWindowPrim, sizeof(TexWindowPrim));
        if (texWindow != 0) {
            SetTexWindow(texWindow, &rect);
        }
        LINK_PACKET(texWindow);

        ALLOC_PACKET(sprite, SpritePrim, sizeof(SpritePrim));
        if (sprite != 0) {
            *(u32 *)&sprite->r0 = D_8009D10C != 0 ? D_8009D114 : D_8009D110;
            ((u8 *)sprite)[3] = 4;
            sprite->code = 0x64;
        }
        sprite->u0 = 0;
        sprite->v0 = 0;
        sprite->x0 = D_8009D124;
        sprite->y0 = D_8009D128;
        sprite->clut = 0x391C;
        sprite->w = width;
        sprite->h = height;
        LINK_PACKET(sprite);

        rect.x = 0;
        rect.y = 0;
        rect.w = 0x20;
        rect.h = 0x20;
        ALLOC_PACKET(texWindow, TexWindowPrim, sizeof(TexWindowPrim));
        if (texWindow != 0) {
            SetTexWindow(texWindow, &rect);
        }
        LINK_PACKET(texWindow);
    }

    ALLOC_PACKET(drawMode, DrawModePrim, sizeof(DrawModePrim));
    if (drawMode != 0) {
        SetDrawMode(drawMode, 0, 0, 7);
    }
    LINK_PACKET(drawMode);
}
