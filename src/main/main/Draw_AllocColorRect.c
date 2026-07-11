/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

typedef struct PointYX {
    s16 y;
    s16 x;
} PointYX;

typedef struct PolyF3Pair {
    u32 tag0;
    u8 r0, g0, b0, code0;
    s16 x0, y0;
    s16 x1, y1;
    s16 x2, y2;
    u32 tag1;
    u8 r1, g1, b1, code1;
    s16 x3, y3;
    s16 x4, y4;
    s16 x5, y5;
} PolyF3Pair;

extern u8 *D_8009D100;
extern u8 *D_8009D104;
extern int D_8009D10C;
extern int D_8009D110;
extern int D_8009D114;
extern u32 *D_8009D11C;
extern u32 D_8009D14C[];
extern u8 D_800A22B0[];

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

void Draw_AllocColorRect(int startIndex, int endIndex, int width, int colorIndex) {
    PointYX start;
    PointYX end;
    PointYX startEdge;
    PointYX endEdge;
    PolyF3Pair *packet;
    int absWidth;
    u32 color;
    u8 *cursor;
    u8 *cursorEnd;
    u32 *ot;
    u32 mask24;
    u32 maskTop;
    u32 tag;

    absWidth = width;
    if (absWidth < 0) {
        absWidth = -absWidth;
    }

    ALLOC_PACKET(packet, PolyF3Pair, sizeof(PolyF3Pair));
    if (packet != 0) {
        cursor = (u8 *)packet + 7;
        cursorEnd = (u8 *)packet + 0x2F;
        while (cursor < cursorEnd) {
            *(u32 *)(cursor - 3) = D_8009D10C != 0 ? D_8009D114 : D_8009D110;
            cursor[-4] = 4;
            cursor[0] = 0x20;
            cursor += 0x14;
        }
    }

    start = *(PointYX *)(D_800A22B0 + ((startIndex & 0x7F) * sizeof(PointYX)));
    startEdge = start;
    end = *(PointYX *)(D_800A22B0 + ((endIndex & 0x7F) * sizeof(PointYX)));
    endEdge = end;

    packet->code0 |= 2;
    packet->code1 |= 2;

    color = D_8009D14C[colorIndex];
    if (start.y == end.y) {
        startEdge.x += absWidth;
        if (endIndex & 0x80) {
            endEdge.x += absWidth;
        } else {
            endEdge.x -= absWidth;
        }
        start.y += width;
        end.y += width;
        *(u32 *)&packet->r0 = (*(u32 *)&packet->r0 & 0xFF000000) | color;
        *(u32 *)&packet->r1 = (*(u32 *)&packet->r1 & 0xFF000000) | color;
    } else {
        startEdge.x += width;
        start.y += absWidth;
        endEdge.x += width;
        if (endIndex & 0x80) {
            end.y += absWidth;
        } else {
            end.y -= absWidth;
        }
        color = (color >> 1) & 0x7F7F7F;
        *(u32 *)&packet->r0 = (*(u32 *)&packet->r0 & 0xFF000000) | color;
        *(u32 *)&packet->r1 = (*(u32 *)&packet->r1 & 0xFF000000) | color;
    }

    packet->x0 = start.x;
    packet->y0 = start.y;
    packet->x1 = end.x;
    packet->y1 = end.y;
    packet->x2 = startEdge.x;
    packet->y2 = startEdge.y;

    packet->x3 = end.x;
    packet->y3 = end.y;
    packet->x4 = endEdge.x;
    packet->y4 = endEdge.y;
    packet->x5 = startEdge.x;
    packet->y5 = startEdge.y;

    ot = D_8009D11C;
    mask24 = 0x00FFFFFF;
    maskTop = 0xFF000000;

    tag = packet->tag0;
    packet->tag0 = (tag & maskTop) | (*ot & mask24);
    *ot = (*ot & maskTop) | ((u32)packet & mask24);

    tag = packet->tag1;
    packet->tag1 = (tag & maskTop) | (*ot & mask24);
    *ot = (*ot & maskTop) | (((u32)packet + 0x14) & mask24);
}
