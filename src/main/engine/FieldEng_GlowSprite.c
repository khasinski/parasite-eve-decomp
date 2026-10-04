#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_glow_layers.h"
#include "pe1/field_shaded_quad.h"
#include "pe1/field_textured_strip.h"

void func_800C42A4(FieldGlowSprite *sprite, GteMatrix *placement, u8 mode)
{
    FieldStripPacket *packet;
    GteShortVector position;
    int swap[4];
    s32 flag;
    FieldStripLink link;

    packet = (FieldStripPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    D_800F33B4 = FIELD_ENGINE_SCRATCH;
    if (mode == 0) {
        gte_CompMatrix(D_800BCFA4.value, placement, &D_800F33B4->matrix);
        gte_ldrotmatrix(&D_800F33B4->matrix);
        gte_ldtransmatrix(&D_800F33B4->matrix);
    } else {
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        position.x = placement->t[0];
        position.y = placement->t[1];
        position.z = placement->t[2];
        RotTrans(&position, (GteVector *)placement->t, &flag);
        gte_ldrotmatrix(placement);
        gte_ldtransmatrix(placement);
    }
    func_800C608C((s16)sprite->depth, &sprite->r, &packet->r0);
    gte_ldv3(&D_800F3310, &D_800F3318, &D_800F3320);
    gte_rtpt_padded();
    D_800F33B4->v = D_800F3422 + (sprite->cell & 0xF0);
    D_800F33B4->u = (sprite->cell - D_800F33B4->v) << 4;
    D_800F33B4->clutX = sprite->clut << 4;
    D_800F33B4->clutY = sprite->clut >> 4;
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
    if (sprite->flip & 1) {
        swap[0] = packet->u0;
        swap[1] = packet->u1;
        swap[2] = packet->u2;
        swap[3] = packet->u3;
        packet->u0 = swap[1];
        packet->u1 = swap[0];
        packet->u2 = swap[3];
        packet->u3 = swap[2];
    }
    if (sprite->flip & 2) {
        swap[0] = packet->v0;
        swap[1] = packet->v1;
        swap[2] = packet->v2;
        swap[3] = packet->v3;
        packet->v0 = swap[2];
        packet->v1 = swap[3];
        packet->v2 = swap[0];
        packet->v3 = swap[1];
    }
    gte_avsz3_padded();
    /* PSY-Q setSemiTrans(packet, D_800F337A). */
    if (D_800F337A) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&D_800F33B4->depth);
    D_800F33B4->depth += sprite->offset;
    if (D_800F33B4->depth - 1 < 0xFFFU) {
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_ldv0(&D_800F3328);
        gte_rtps();
        packet->tpage = D_800E27AC;
        packet->clut = GetClut(D_800F341C + D_800F33B4->clutX,
                               D_800F341E + D_800F33B4->clutY);
        gte_stsxy2(&packet->x3);
        packet->tag.address = STRIP_OT(D_800F33B4->depth)->address;
        link.tag = &packet->tag;
        STRIP_OT(D_800F33B4->depth)->address = link.word;
        D_8009CDD8 += sizeof(FieldStripPacket);
    }
}
