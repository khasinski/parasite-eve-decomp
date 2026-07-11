/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;
typedef unsigned char u8;
typedef unsigned int u32;

typedef struct DrawModePrim {
    u32 tag;
    u32 code;
} DrawModePrim;

extern u8 *D_8009D100;
extern u8 *D_8009D104;
extern u32 *D_8009D11C;

void SetDrawMode(void *packet, int dfe, int dtd, int tpage);
void Draw_AllocColorRect(int startIndex, int endIndex, int width, int colorIndex);
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

void Draw_EmitWipeBar(u8 *points, int mode) {
    DrawModePrim *frontMode;
    DrawModePrim *backMode;
    int backColorIndex;

    ALLOC_PACKET(frontMode, DrawModePrim, sizeof(DrawModePrim));
    if (frontMode != 0) {
        SetDrawMode(frontMode, 0, 0, (((mode + 1) & 3) << 5));
    }

    backColorIndex = 2 - mode;

    ALLOC_PACKET(backMode, DrawModePrim, sizeof(DrawModePrim));
    if (backMode != 0) {
        SetDrawMode(backMode, 0, 0, ((backColorIndex & 3) << 5));
    }

    while ((s8)points[0] >= 0) {
        Draw_AllocColorRect((s8)points[0], (s8)points[1], 2, mode);
        points += 2;
    }

    points++;
    LINK_PACKET(frontMode);

    while ((s8)points[0] >= 0) {
        Draw_AllocColorRect((s8)points[0], (s8)points[1], -2, mode < 1);
        points += 2;
    }

    LINK_PACKET(backMode);
}
