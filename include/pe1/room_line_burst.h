#ifndef PE1_ROOM_LINE_BURST_H
#define PE1_ROOM_LINE_BURST_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/render_prim.h"
#include "pe1/room_spark.h"

/* Room burst that spits a ring of bounce particles, draws a line from its
 * anchor to a point swept along the actor, then shrinks away. */

typedef struct RoomLineBurstState {
    s16 x, y, z, reserved06;      /* 0x00: anchor */
    s16 ex, ey, ez, reserved0E;   /* 0x08: line end */
    s16 px, py, pz, reserved16;   /* 0x10: sprite position */
    s16 state;                    /* 0x18 */
    s16 timer;                    /* 0x1A */
    s16 color;                    /* 0x1C */
    s16 scale;                    /* 0x1E */
    s16 intensity;                /* 0x20 */
    s16 countdown;                /* 0x22 */
} RoomLineBurstState;

PE1_STATIC_ASSERT(sizeof(RoomLineBurstState) == 0x24, room_line_burst_state_size);

typedef struct RoomLineBurstParams {
    s16 x, y, z, reserved06;      /* 0x00 */
    s32 duration;                 /* 0x08 */
    s32 countdown;                /* 0x0C */
} RoomLineBurstParams;

typedef struct RoomLineBurstParticle {
    s16 x, y, z, reserved06;
    s16 vx;                       /* 0x08 */
    s16 vy;                       /* 0x0A */
    s16 vz;                       /* 0x0C */
    s16 ay;                       /* 0x0E */
    s16 state;                    /* 0x10 */
    s16 timer;                    /* 0x12 */
} RoomLineBurstParticle;

PE1_STATIC_ASSERT(sizeof(RoomLineBurstParticle) == 0x14, room_line_burst_particle_size);

/* LINE_F2 packet as the burst fills it: the tag's length byte, the colour
 * bytes with the primitive code, and the two screen points. */
typedef struct RoomLineBurstLinePacket {
    u8 tag[3];
    u8 length;                    /* 0x03 */
    u8 r, g, b, code;             /* 0x04 */
    s16 x0, y0;                   /* 0x08 */
    s16 x1, y1;                   /* 0x0C */
} RoomLineBurstLinePacket;

PE1_STATIC_ASSERT(sizeof(RoomLineBurstLinePacket) == 0x10, room_line_burst_line_packet_size);

/* Eight-byte constants copied to the stack unaligned, as retail does. */
typedef struct RoomLineBurstWords8 {
    s32 word[2];
} __attribute__((packed)) RoomLineBurstWords8;

/* Tile colour of a rising particle, copied to the stack unaligned. */
typedef struct RoomLineBurstColor {
    u8 r, g, b, code;
} RoomLineBurstColor;

extern char *D_800B0E58[];
extern u16 D_800E11EC;
extern u16 D_800E11E8;
extern void LoadAverageShort12(void *from, void *to, int weightFrom, int weightTo, void *out);

#endif
