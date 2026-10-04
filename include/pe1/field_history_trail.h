#ifndef PE1_FIELD_HISTORY_TRAIL_H
#define PE1_FIELD_HISTORY_TRAIL_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_prim.h"
#include "pe1/render_tint.h"
#include "pe1/render_object.h"
#include "pe1/field_tile.h"

/* Field engine history trail (func_800D1384): a ribbon of gouraud quads
 * through the last `count` head/tail point pairs, fading out towards the
 * oldest pair. The `pad` halves of both points flag a recorded pair. */

typedef struct FieldTrailPair {
    /* 0x00 */ RenderHistoryPoint head;
    /* 0x08 */ RenderHistoryPoint tail;
} FieldTrailPair;

/* PSY-Q POLY_G4. */
typedef struct FieldG4Packet {
    /* 0x00 */ RenderGpuTag tag;
    /* 0x04 */ u8 r0, g0, b0, code;
    /* 0x08 */ s16 x0, y0;
    /* 0x0C */ u8 r1, g1, b1, p1;
    /* 0x10 */ s16 x1, y1;
    /* 0x14 */ u8 r2, g2, b2, p2;
    /* 0x18 */ s16 x2, y2;
    /* 0x1C */ u8 r3, g3, b3, p3;
    /* 0x20 */ s16 x3, y3;
} FieldG4Packet;

PE1_STATIC_ASSERT(sizeof(FieldTrailPair) == 0x10, field_trail_pair_size);
PE1_STATIC_ASSERT(sizeof(FieldG4Packet) == 0x24, field_g4_packet_size);

void SetPolyG4(FieldG4Packet *packet);

#endif /* PE1_FIELD_HISTORY_TRAIL_H */
