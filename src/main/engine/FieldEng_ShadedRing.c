#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_prim.h"
#include "pe1/render_object.h"
#include "pe1/field_shaded_ring.h"
/* MASPSX_FLAGS: --expand-div */

void func_800C4FC4(FieldShadedRing *ring, GteMatrix *placement, u8 mode)
{
    FieldRingPacket *packet;
    char *drawMode;
    u8 outer[4];
    u8 inner[4];
    GteShortVector position;
    s32 flag;
    s32 p;
    GteShortVector *points;
    u32 i;
    u32 next;
    u16 a;
    u16 c;
    u16 d;
    FieldRingLink link;

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
    func_800C608C(ring->brightness, ring->outerRgb, outer);
    func_800C608C(ring->brightness, ring->innerRgb, inner);
    points = ring->points;
    for (i = 0; i < ring->count; i++) {
        packet = (FieldRingPacket *)(D_800B0E58[D_8009CDDC] + D_8009CDD8);
        D_8009CDD8 += sizeof(FieldRingPacket);
        drawMode = D_800B0E58[D_8009CDDC] + D_8009CDD8;
        D_8009CDD8 += 8;
        SetDrawMode(drawMode, 0, 0, (D_800E224C & 3) << 5);
        next = (i + 1) % ring->count;
        a = i;
        c = ring->count + i;
        d = ring->count + next;
        D_800F33B4->depth = RotTransPers4(&points[a], &points[next], &points[c],
                                          &points[d], &packet->x0, &packet->x1,
                                          &packet->x2, &packet->x3, &p, &flag);
        /* Word copies through plain pointers (not struct fields), so the
         * D_800F337A load below stays after them. */
        *(u32 *)&packet->c0 = *(u32 *)outer;
        *(u32 *)&packet->c1 = *(u32 *)outer;
        *(u32 *)&packet->c2 = *(u32 *)inner;
        *(u32 *)&packet->c3 = *(u32 *)inner;
        packet->tag.length = 8;
        packet->c0.rgb[3] = 0x38;
        /* PSY-Q setSemiTrans(packet, D_800F337A). */
        if (D_800F337A) {
            packet->c0.rgb[3] = packet->c0.rgb[3] | 2;
        } else {
            packet->c0.rgb[3] = packet->c0.rgb[3] & ~2;
        }
        packet->tag.address = RING_OT(D_800F33B4->depth + ring->depth)->address;
        link.tag = &packet->tag;
        RING_OT(D_800F33B4->depth + ring->depth)->address = link.word;
        link.bytes = drawMode;
        link.tag->address = RING_OT(D_800F33B4->depth + ring->depth)->address;
        RING_OT(D_800F33B4->depth + ring->depth)->address = link.word;
    }
}
