/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

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

void Draw_AllocColorTri(int width, int height, int pulse) {
    TilePrim *tile;
    DrawModePrim *drawMode;
    int shade;
    int sync;

    ALLOC_PACKET(tile, TilePrim, sizeof(TilePrim));
    if (tile != 0) {
        *(u32 *)&tile->r0 = D_8009D10C != 0 ? D_8009D114 : D_8009D110;
        ((u8 *)tile)[3] = 3;
        tile->code = 0x60;
    }

    if (pulse != 0) {
        sync = VSync(-1);
        if (sync & 0x20) {
            shade = (sync & 0x1F) << 1;
        } else {
            shade = 0x40 - ((sync & 0x1F) << 1);
        }
        tile->r0 = shade;
        tile->g0 = shade;
        tile->b0 = shade;
    } else {
        *(u32 *)&tile->r0 = (*(u32 *)&tile->r0 & 0xFF000000) | (D_8009D114 & 0x00FFFFFF);
    }

    tile->w = width - 4;
    tile->h = height - 4;
    tile->code |= 2;
    tile->x0 = D_8009D124 + 2;
    tile->y0 = D_8009D128 + 2;
    LINK_PACKET(tile);

    ALLOC_PACKET(drawMode, DrawModePrim, sizeof(DrawModePrim));
    if (drawMode != 0) {
        SetDrawMode(drawMode, 0, 0, 0x20);
    }
    LINK_PACKET(drawMode);
}
