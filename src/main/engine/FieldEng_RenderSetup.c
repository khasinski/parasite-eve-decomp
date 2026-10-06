/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/field_oriented_sprite.h"

/* Field engine render setup: draw-area and texture-page state, the
 * colour ramp helpers and the oriented sprite draw. */

extern u8 D_800F33AC;
extern u8 D_800E224C;
extern u8 D_800F3422;
extern u16 D_800F3424;
extern u16 D_800F3426;
extern u16 D_800E27AC;
extern u8 D_800F345C;
extern u8 D_800F345D;
int printf(const char *fmt, ...);
extern char D_800C2110[];
extern u8 D_800F33B8;
extern u8 D_800F337A;

void func_800C2EAC(u8 mode) {
    if (mode == 0) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1D7;
        D_800F3422 = 0;
    }
    if (mode == 1) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x160;
        D_800F341E = 0x1DB;
        D_800F341C = 0;
        D_800F3422 = 0x60;
    }
    if (mode == 2) {
        D_800F3424 = 0x340;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1D6;
        D_800F3422 = 0;
    }
    if (mode == 3) {
        D_800F3424 = 0x380;
        D_800F3426 = 0x100;
        D_800F341C = 0;
        D_800F341E = 0x1C8;
        D_800F3422 = 0;
    }

    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}

void func_800C2FF0(int width, int height) {
    int widthMinus;
    int heightMinus;
    int x;
    int y;
    widthMinus = width - 1;
    heightMinus = height - 1;
    asm("" : : "r"(widthMinus), "r"(heightMinus));

    D_800F345C = width;
    x = (width & 0xFF) << 4;
    D_800F345D = height;
    y = (height & 0xFF) << 4;

    D_800F3310.x = -x;
    D_800F3310.y = -y;
    D_800F3310.z = 0;
    D_800F3318.x = x;
    D_800F3318.y = -y;
    D_800F3318.z = 0;
    D_800F3320.x = -x;
    D_800F3320.y = y;
    D_800F3320.z = 0;
    D_800F3328.x = x;
    D_800F3328.y = y;
    D_800F3328.z = 0;

    D_800F345C = widthMinus;
    D_800F345D = heightMinus;
}

void func_800C3098(int mode) {
    mode = (short)mode;

    if (mode == 0x10) {
        goto mode16;
    }
    if (mode == 0x100) {
        goto mode256;
    }
    goto badMode;

mode16:
    D_800F33AC = 0;
    goto done;

mode256:
    D_800F33AC = 1;
    goto done;

badMode:
    printf(D_800C2110);

done:
    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}

void func_800C3134(u8 *table, u32 step, u8 *out) {
    u32 sum = 0;
    u32 one = 0x1000;
    u32 last = 0xFF;
    u8 *entry = table;

    while (1) {
        u32 duration = entry[3];
        u8 *next = entry + 4;

        sum += duration;
        if (step < sum) {
            u32 weight = ((sum - step) << 12) / duration;
            u32 inv = one - weight;
            u32 out0;
            u32 out1;
            u32 out2;

            out0 = ((inv * entry[4]) + (weight * entry[0])) >> 12;
            out1 = ((inv * entry[5]) + (weight * entry[1])) >> 12;
            out2 = ((inv * entry[6]) + (weight * entry[2])) >> 12;
            out[0] = out0;
            out[1] = out1;
            out[2] = out2;
            return;
        }

        if (entry[7] == last) {
            out[0] = entry[4];
            out[1] = entry[5];
            out[2] = entry[6];
            return;
        }

        entry = next;
    }
}

void func_800C3238(u8 mode) {
    D_800F33B8 = mode;
    switch (mode) {
    case 0:
        D_800F337A = 0;
        D_800E224C = 0;
        break;
    case 1:
        D_800F337A = 1;
        D_800E224C = 0;
        break;
    case 2:
        D_800F337A = 1;
        D_800E224C = 1;
        break;
    case 3:
        D_800F337A = 1;
        D_800E224C = 2;
        break;
    case 4:
        D_800F337A = 1;
        D_800E224C = 3;
        break;
    }
    D_800E27AC = GetTPage(D_800F33AC, D_800E224C, D_800F3424, D_800F3426);
}

void func_800C3324(FieldOrientedSprite *sprite)
{
    FieldStripPacket *packet;
    FieldStripLink link;

    packet = (FieldStripPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    D_800F33B4 = FIELD_ENGINE_SCRATCH;
    D_800F33B4->v = sprite->cell & 0xF0;
    D_800F33B4->u = (sprite->cell - D_800F33B4->v) << 4;
    D_800F33B4->clutX = sprite->clut << 4;
    D_800F33B4->clutY = sprite->clut >> 4;
    if (sprite->brightness != 0x80) {
        D_800F33B4->channel = sprite->rgb[0];
        D_800F33B4->channel *= sprite->brightness;
        if (D_800F33B4->channel > 0x7FFF) {
            D_800F33B4->channel = 0x7FFF;
        }
        D_800F33B4->channel >>= 7;
        packet->r0 = D_800F33B4->channel;
        D_800F33B4->channel = sprite->rgb[1];
        D_800F33B4->channel *= sprite->brightness;
        if (D_800F33B4->channel > 0x7FFF) {
            D_800F33B4->channel = 0x7FFF;
        }
        D_800F33B4->channel >>= 7;
        packet->g0 = D_800F33B4->channel;
        D_800F33B4->channel = sprite->rgb[2];
        D_800F33B4->channel *= sprite->brightness;
        if (D_800F33B4->channel > 0x7FFF) {
            D_800F33B4->channel = 0x7FFF;
        }
        D_800F33B4->channel >>= 7;
        packet->b0 = D_800F33B4->channel;
    } else {
        packet->r0 = sprite->rgb[0];
        packet->g0 = sprite->rgb[1];
        packet->b0 = sprite->rgb[2];
    }
    RotMatrixYXZ(&sprite->rotation, &D_800F33B4->local);
    ScaleMatrix(&D_800F33B4->local, &sprite->scale);
    D_800F33B4->local.t[0] = 0;
    D_800F33B4->local.t[1] = 0;
    D_800F33B4->local.t[2] = 0;
    D_800F33B4->matrix = *D_800BCFA4.matrix;
    D_800F33B4->matrix.t[0] += (D_800F33B4->matrix.m[0][2] * sprite->position.z +
                                D_800F33B4->matrix.m[0][1] * sprite->position.y +
                                D_800F33B4->matrix.m[0][0] * sprite->position.x) / 4096;
    D_800F33B4->matrix.t[1] += (D_800F33B4->matrix.m[1][2] * sprite->position.z +
                                D_800F33B4->matrix.m[1][1] * sprite->position.y +
                                D_800F33B4->matrix.m[1][0] * sprite->position.x) / 4096;
    D_800F33B4->matrix.t[2] += (D_800F33B4->matrix.m[2][2] * sprite->position.z +
                                D_800F33B4->matrix.m[2][1] * sprite->position.y +
                                D_800F33B4->matrix.m[2][0] * sprite->position.x) / 4096;
    gte_CompMatrix(&D_800F33B4->matrix, &D_800F33B4->local, &D_800F33B4->matrix);
    gte_ldrotmatrix(&D_800F33B4->matrix);
    gte_ldtransmatrix(&D_800F33B4->matrix);
    gte_ldv3(&D_800F3310, &D_800F3318, &D_800F3320);
    gte_rtpt_padded();
    packet->tag.length = 9;
    packet->code = 0x2C;
    packet->u0 = D_800F33B4->u;
    packet->v0 = D_800F33B4->v;
    packet->u1 = D_800F33B4->u + D_800F345C;
    packet->v1 = D_800F33B4->v;
    packet->u2 = D_800F33B4->u;
    packet->v2 = D_800F33B4->v + D_800F345D;
    packet->u3 = D_800F33B4->u + D_800F345C;
    packet->v3 = D_800F33B4->v + D_800F345D;
    gte_avsz3_padded();
    /* PSY-Q setSemiTrans(packet, D_800F337A). */
    if (D_800F337A) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&D_800F33B4->depth);
    gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
    gte_ldv0(&D_800F3328);
    gte_rtps();
    packet->tpage = D_800E27AC;
    packet->clut = GetClut(D_800F341C + D_800F33B4->clutX,
                           D_800F341E + D_800F33B4->clutY);
    gte_stsxy2(&packet->x3);
    packet->tag.address = STRIP_OT(D_800F33B4->depth + sprite->depth)->address;
    link.tag = &packet->tag;
    STRIP_OT(D_800F33B4->depth + sprite->depth)->address = link.word;
    D_8009CDD8 += sizeof(FieldStripPacket);
}
