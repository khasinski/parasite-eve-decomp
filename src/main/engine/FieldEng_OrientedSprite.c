#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/field_oriented_sprite.h"

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
    Gte_ScaleMatrix(&D_800F33B4->local, &sprite->scale);
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
