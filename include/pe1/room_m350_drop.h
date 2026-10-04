#ifndef PE1_ROOM_M350_DROP_H
#define PE1_ROOM_M350_DROP_H

#include "pe1/render_object.h"

/* Falling drop moved by room_m350's splash callback. */
typedef struct RoomM350Drop {
    GteShortVector velocity;
    s16 x;
    s16 y;
    s16 z;
    u8 landed;
} RoomM350Drop;

/* Trail state at 0x8019A834: four queued positions, the splash flags,
 * the queued position count and the splash/contact positions. */
typedef struct RoomM350TrailQueue {
    GteShortVector points[4];
    u8 reserved20[2];
    u8 splashed;
    u8 touched;
    s32 reserved24;
    s16 count;
    s16 reserved2A;
    s16 splash_x;
    s16 splash_z;
    s16 hit_x;
    s16 hit_y;
    s16 hit_z;
} RoomM350TrailQueue;

typedef struct RoomM350DropFloor {
    s16 value;
} RoomM350DropFloor;

extern RoomM350TrailQueue g_RoomEffectTrailPositions;
extern GteRotation D_8019A3C0;
extern RenderColor D_8019A61C[3];
extern RoomM350DropFloor D_800942EC;

u16 GetClut(int x, int y);
int func_8019721C(short *point, int radius);

#endif
