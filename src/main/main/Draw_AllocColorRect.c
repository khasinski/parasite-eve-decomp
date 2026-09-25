#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

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
