#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_shaded_quad.h"

void func_800C499C(FieldShadedQuadColors *colors, GteMatrix *placement,
                   u8 mode) {
    FieldShadedQuadPacket *packet;
    GteShortVector position;
    s32 flag;
    FieldShadedQuadLink link;

    packet = (FieldShadedQuadPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
    D_800F33B4 = FIELD_SHADED_QUAD_SCRATCH;
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
    func_800C608C(colors->brightness, colors->rgb[0], &packet->r0);
    func_800C608C(colors->brightness, colors->rgb[1], &packet->r1);
    func_800C608C(colors->brightness, colors->rgb[2], &packet->r2);
    func_800C608C(colors->brightness, colors->rgb[3], &packet->r3);
    gte_ldv3(&D_800F3310, &D_800F3318, &D_800F3320);
    gte_rtpt_padded();
    packet->tag.length = 8;
    packet->code = 0x38;
    gte_avsz3_padded();
    /* PSY-Q setSemiTrans(packet, D_800F337A). */
    if (D_800F337A) {
        packet->code = packet->code | 2;
    } else {
        packet->code = packet->code & ~2;
    }
    gte_stotz(&D_800F33B4->depth);
    D_800F33B4->depth += colors->depth;
    if (D_800F33B4->depth - 1 < 0xFFFU) {
        gte_stsxy3(&packet->x0, &packet->x1, &packet->x2);
        gte_ldv0(&D_800F3328);
        gte_rtps();
        gte_stsxy2(&packet->x3);
        packet->tag.address = SHADED_QUAD_OT(D_800F33B4->depth)->address;
        link.tag = &packet->tag;
        SHADED_QUAD_OT(D_800F33B4->depth)->address = link.word;
        D_8009CDD8 += sizeof(FieldShadedQuadPacket);
    }
}
