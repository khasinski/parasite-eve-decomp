#ifndef PE1_ROOM_M005_DRIFTING_SPRITE_H
#define PE1_ROOM_M005_DRIFTING_SPRITE_H

#include "common.h"

/* Drifting sprite drawn by the room_m005 effect callback; the joint beacon
 * spawns these into its particle pool. */
typedef struct RoomM005DriftingSpriteState {
    unsigned short x;             /* 0x00 */
    unsigned short y;             /* 0x02 */
    unsigned short z;             /* 0x04 */
    short pad06;                  /* 0x06 */
    unsigned short vx;            /* 0x08 */
    unsigned short vy;            /* 0x0A */
    unsigned short vz;            /* 0x0C */
} RoomM005DriftingSpriteState;

int func_8018FB84(int mode, RoomM005DriftingSpriteState *state);

#endif /* PE1_ROOM_M005_DRIFTING_SPRITE_H */
