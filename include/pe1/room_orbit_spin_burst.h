#ifndef PE1_ROOM_ORBIT_SPIN_BURST_H
#define PE1_ROOM_ORBIT_SPIN_BURST_H

#include "pe1/room_spark.h"
#include "pe1/room_orbit_trail.h"

/* Rotation read back from the actor's transform and fed to the ring draw. */
typedef union RoomOrbitSpinBurstTilt {
    GteShortVector vector;
    GteRotation rotation;
} RoomOrbitSpinBurstTilt;

extern char *D_8009D254;

/* Glow sprite with an explicit sprite size; the eighth argument is zero at
 * every known call site. */
void func_800D3BC8(GteShortVector *position, int scale_x, int scale_y,
                   int texture, int clut, int page, int intensity, int arg7,
                   int size);

#endif
