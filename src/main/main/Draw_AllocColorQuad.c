/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct PolyG4Prim {
    u32 tag;
    u8 r0, g0, b0, code;
    u16 x0, y0;
    u8 r1, g1, b1, pad1;
    u16 x1, y1;
    u8 r2, g2, b2, pad2;
    u16 x2, y2;
    u8 r3, g3, b3, pad3;
    u16 x3, y3;
} PolyG4Prim;

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

int Battle_GetStateFlag1(void);
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

#define INIT_POLY_G4(packet)                      \
    do {                                          \
        if ((packet) != 0) {                       \
            *(u32 *)&(packet)->r0 = 0x808080;      \
            ((u8 *)(packet))[3] = 8;               \
            (packet)->code = 0x38;                 \
        }                                         \
    } while (0)

void Draw_AllocColorQuad(int value, int limit) {
    int battleOffset;
    int shadeWidth;
    PolyG4Prim *left;
    PolyG4Prim *right;
    TilePrim *tile;
    DrawModePrim *drawMode;

    battleOffset = Battle_GetStateFlag1() == 0 ? 7 : 0;

    ALLOC_PACKET(left, PolyG4Prim, sizeof(PolyG4Prim));
    INIT_POLY_G4(left);

    if (limit > 0 && (value < limit ? value : limit) >= 0) {
        shadeWidth = ((value < limit ? value : limit) * 0x38) / limit;
    } else {
        shadeWidth = 0;
    }

    left->r0 = 0;
    left->g0 = 0x82;
    left->b0 = 0x36;
    left->r1 = 0;
    left->g1 = 0x82;
    left->b1 = 0x36;
    left->r2 = 0x4A;
    left->g2 = 0xFF;
    left->b2 = 0x3B;
    left->r3 = 0x4A;
    left->g3 = 0xFF;
    left->b3 = 0x3B;
    left->x0 = 0xE5;
    left->y0 = battleOffset + 0xAA;
    left->x1 = shadeWidth + 0xE5;
    left->y1 = battleOffset + 0xAA;
    left->x2 = 0xE5;
    left->y2 = battleOffset + 0xAD;
    left->x3 = shadeWidth + 0xE5;
    left->y3 = battleOffset + 0xAD;
    LINK_PACKET(left);

    ALLOC_PACKET(right, PolyG4Prim, sizeof(PolyG4Prim));
    INIT_POLY_G4(right);

    right->r0 = 0xFF;
    right->g0 = 0x3D;
    right->b0 = 0x81;
    right->r1 = 0xFF;
    right->g1 = 0x3D;
    right->b1 = 0x81;
    right->r2 = 0x83;
    right->g2 = 0x13;
    right->b2 = 1;
    right->r3 = 0x83;
    right->g3 = 0x13;
    right->b3 = 1;
    right->x0 = shadeWidth + 0xE5;
    right->y0 = battleOffset + 0xAA;
    right->x1 = 0x11D;
    right->y1 = battleOffset + 0xAA;
    right->x2 = shadeWidth + 0xE5;
    right->y2 = battleOffset + 0xAD;
    right->x3 = 0x11D;
    right->y3 = battleOffset + 0xAD;
    D_8009D110 = 0x303030;
    D_8009D114 = 0x181818;
    D_8009D10C = 0;
    LINK_PACKET(right);

    ALLOC_PACKET(tile, TilePrim, sizeof(TilePrim));
    if (tile != 0) {
        *(u32 *)&tile->r0 = D_8009D10C != 0 ? D_8009D114 : D_8009D110;
        ((u8 *)tile)[3] = 3;
        tile->code = 0x60;
    }
    tile->x0 = 0xDD;
    tile->y0 = battleOffset + 0xA6;
    tile->w = 0x54;
    tile->h = 0xB;
    tile->code |= 2;
    LINK_PACKET(tile);

    ALLOC_PACKET(drawMode, DrawModePrim, sizeof(DrawModePrim));
    if (drawMode != 0) {
        SetDrawMode(drawMode, 0, 0, 0);
    }
    D_8009D110 = 0x808080;
    D_8009D114 = 0x404040;
    LINK_PACKET(drawMode);
}
