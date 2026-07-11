/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

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

u8 *Draw_LookupGlyphDescriptor(int glyph);
void SetDrawMode(void *packet, int dfe, int dtd, int tpage);
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

void Draw_AllocSprite(int glyph) {
    u8 *desc;
    SpritePrim *prim;
    DrawModePrim *drawMode;
    int tpage;

    desc = Draw_LookupGlyphDescriptor(glyph);

    ALLOC_PACKET(prim, SpritePrim, sizeof(SpritePrim));
    if (prim != 0) {
        *(u32 *)&prim->r0 = D_8009D10C != 0 ? D_8009D114 : D_8009D110;
        ((u8 *)prim)[3] = 4;
        prim->code = 0x64;
    }

    prim->x0 = D_8009D124;
    prim->y0 = D_8009D128;
    prim->u0 = desc[0];
    prim->v0 = desc[1];
    prim->clut = *(u16 *)(desc + 2);
    prim->w = desc[4];
    prim->h = desc[5];
    LINK_PACKET(prim);

    tpage = desc[6];
    ALLOC_PACKET(drawMode, DrawModePrim, sizeof(DrawModePrim));
    if (drawMode != 0) {
        SetDrawMode(drawMode, 0, 0, ((tpage & 3) << 7) | 7);
    }
    LINK_PACKET(drawMode);
}
