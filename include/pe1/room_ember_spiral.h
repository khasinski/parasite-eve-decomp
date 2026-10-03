#ifndef PE1_ROOM_EMBER_SPIRAL_H
#define PE1_ROOM_EMBER_SPIRAL_H

#include "common.h"
#include "pe1/room_shake_burst.h"

/* Ember spiral controller (room_m318 func_801988F8): rides the actor's
 * hand for 24 frames shedding orbiting sparks, embers and smoke while it
 * draws a glow and a turning, swelling model; hitting the player flags the
 * actor for the scene script. */
typedef struct RoomEmberSpiral {
    s16 x, y, z;                  /* 0x00: heading angles */
    s16 reserved06;
    s16 reserved08;
    s16 timer;                    /* 0x0A */
    s16 glow;                     /* 0x0C */
} RoomEmberSpiral;

extern void *D_80199944;
extern u8 D_80199890[];
extern int func_80198268(int mode, RoomOrbitTrailParticle *p);
extern int func_8006E498(void *channel, int id);
extern int func_800C6B90(void *position, int radius);
extern void func_800C6FA0(void *asset, int intensity);

#endif
