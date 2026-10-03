#ifndef PE1_ROOM_M023_EFFECTS_H
#define PE1_ROOM_M023_EFFECTS_H

#include "common.h"
#include "pe1/render_object.h"

/* Room m023 controller that scatters particles around a model joint,
 * either at the joint itself or at a random point of a wide band. */

typedef struct RoomM023EventState {
    u8 reserved[0x12];
    s16 variant;                  /* 0x12 */
    s16 interval;                 /* 0x14 */
} RoomM023EventState;

typedef struct RoomM023Channel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} RoomM023Channel;

typedef struct RoomM023ScatterState {
    u8 reserved[8];
    s16 unused08;                 /* 0x08 */
    s16 unused0A;                 /* 0x0A */
    s16 timer;                    /* 0x0C */
    s16 joint;                    /* 0x0E */
} RoomM023ScatterState;

typedef struct RoomM023Particle {
    s16 x, y, z;
    s16 attached;                 /* 0x06 */
    s16 vx, vy, vz;               /* 0x08 */
} RoomM023Particle;

typedef struct RoomM023Template {
    s32 word[2];
} __attribute__((packed)) RoomM023Template;

extern RoomM023EventState *D_800E2368;
extern RoomM023Channel *D_800F32D0, *D_800F33E0;
extern RoomM023Template D_8018EFF4;
extern u8 D_80190758[];
extern u16 D_800E11E4[];

extern int func_800CE560(void *pool, int size, int count, void *callback);
extern void func_800CE8F0(void *pool, int index, void *template, void *position);
extern RoomM023Particle *func_800CE610(void *pool);
extern int func_80071A54(void);
extern int func_8018F004(int mode, RoomM023Particle *particle);

#endif
