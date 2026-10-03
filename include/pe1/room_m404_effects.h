#ifndef PE1_ROOM_M404_EFFECTS_H
#define PE1_ROOM_M404_EFFECTS_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/room_spark.h"

/* Room m404 burst effects: a debris burst emitted from a model joint and
 * the bouncing spark particle it spawns. */

typedef struct RoomM404BurstOrigin {
    s16 x, y, z;
} RoomM404BurstOrigin;

typedef struct RoomM404JointTemplate {
    s32 word[2];
} __attribute__((packed)) RoomM404JointTemplate;

extern RoomM404JointTemplate D_8018F21C;
extern GteRotation D_8018F210;
extern RenderColor D_8018F218;
extern u8 D_80193F68[];

extern int func_801935E0(int mode, RoomDampedSpark *spark);

#endif
