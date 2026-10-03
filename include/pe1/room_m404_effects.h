#ifndef PE1_ROOM_M404_EFFECTS_H
#define PE1_ROOM_M404_EFFECTS_H

#include "common.h"
#include "pe1/render_object.h"

/* Room m404 burst effects: a debris burst emitted from a model joint and
 * the falling spark particle it spawns. */

typedef struct RoomM404EffectChannel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} RoomM404EffectChannel;

typedef struct RoomM404BurstOrigin {
    s16 x, y, z;
} RoomM404BurstOrigin;

typedef struct RoomM404BurstSpark {
    s16 x, y, z, unused06;
    s16 vx, vy, vz, unused0E;
    s16 state, timer;             /* 0x10 */
} RoomM404BurstSpark;

typedef struct RoomM404JointTemplate {
    s32 word[2];
} __attribute__((packed)) RoomM404JointTemplate;

extern RoomM404EffectChannel *D_800F32D0, *D_800F33E0;
extern RoomM404JointTemplate D_8018F21C;
extern int D_800E27EC;
extern u16 D_800E11E4[];

extern int func_800CE560(void *pool, int size, int count, void *callback);
extern void func_800CE8F0(void *pool, int index, void *template, void *position);
extern RoomM404BurstSpark *func_800CE610(void *pool);
extern int func_800D3FD8(void);
extern int func_800D3F64(int sound, int handle);
extern int func_80071A54(void);
extern int func_801935E0(int mode, RoomM404BurstSpark *spark);

#endif
