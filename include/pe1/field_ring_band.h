#ifndef PE1_FIELD_RING_BAND_H
#define PE1_FIELD_RING_BAND_H

#include "common.h"
#include "pe1/field_star_fan.h"

/* Field engine ring band (func_800D0728): an annulus of gouraud quads
 * between two radii, scaled, turned and placed at a projected anchor. */

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

PE1_STATIC_ASSERT(sizeof(FieldG4Packet) == 0x24, field_g4_packet_size);

void SetPolyG4(FieldG4Packet *packet);

/* TILE_OT_ADDPRIM through an ordering-table entry held as a plain tag
 * pointer: retail keeps that pointer in one register for every link. */
#define RING_OT_ADDPRIM(entry, packet, link) \
    ((packet)->tag.address = (entry)->address, \
     (link).tag = &(packet)->tag, \
     (entry)->address = (link).word)

#endif /* PE1_FIELD_RING_BAND_H */
