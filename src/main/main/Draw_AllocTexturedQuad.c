/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

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

void Draw_AllocTexturedQuad(int code) {
    int low;
    int glyph;
    int metrics;
    int width;
    int u;
    int v;
    int spacing;
    int column;
    int row;
    TexturedQuadPrim *prim;

    low = code & 0xFF;
    glyph = low;
    if (D_8009D0D8 != 0) {
        glyph += D_8009D0D8 << 8;
        D_8009D0D8 = 0;
    }

    if ((u32)low >= 0xFA) {
        D_8009D0D8 = low - 0xFA;
        glyph = -1;
    }

    if (glyph == 0x100) {
        Draw_AllocSprite(0x77);
        D_8009D124 += 0xC;
        D_8009D128 = D_8009D128;
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

    ALLOC_PACKET(prim, TexturedQuadPrim, sizeof(TexturedQuadPrim));
    if (prim != 0) {
        *(u32 *)&prim->r0 = D_8009D10C != 0 ? D_8009D114 : D_8009D110;
        ((u8 *)prim)[3] = 9;
        prim->code = 0x2C;
    }

    metrics = Draw_LookupGlyphMetrics(glyph);
    width = (metrics >> 4) & 0xF;
    spacing = 0;
    if (glyph < 10 || glyph == 15) {
        spacing = 1;
    }
    D_8009CDB0 = spacing + 1;

    u = column * 0xC + (metrics & 0xF);
    v = row * 0xC;

    prim->u2 = u;
    prim->u0 = u;
    prim->clut = 0x89C;
    prim->v1 = v;
    prim->v0 = v;
    prim->u3 = u + width;
    prim->u1 = u + width;
    prim->v3 = v + 0xC;
    prim->v2 = v + 0xC;

    prim->x0 = D_8009D124 + (D_8009CDB0 >> 1);
    prim->x2 = prim->x0;
    prim->y1 = D_8009D128 + 1;
    prim->y0 = prim->y1;
    prim->x3 = prim->x0 + width;
    prim->x1 = prim->x3;
    prim->y3 = prim->y0 + 0xC;
    prim->y2 = prim->y3;
    prim->tpage = GetTPage(0, 0, 0x140, 0);

    D_8009D128 = D_8009D128;
    LINK_PACKET(prim);
    D_8009D124 += width + D_8009CDB0;
}
