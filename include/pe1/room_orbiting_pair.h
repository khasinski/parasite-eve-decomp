#ifndef PE1_ROOM_ORBITING_PAIR_H
#define PE1_ROOM_ORBITING_PAIR_H

#include "common.h"

typedef struct RoomOrbitPairTemplate8 {
    s32 word[2];
} __attribute__((packed)) RoomOrbitPairTemplate8;

typedef struct RoomOrbitPairChannel {
    s32 reserved[2];
    char *pool;
} RoomOrbitPairChannel;

typedef struct RoomOrbitPairNode {
    u8 reserved[0x18];
    u8 *state;
} RoomOrbitPairNode;

typedef struct RoomOrbitPairEventState {
    u8 reserved[0xD];
    u8 active;
} RoomOrbitPairEventState;

typedef struct RoomOrbitPairParticle {
    s16 x, y, z, pad6;
    s16 vx, vy, vz, padE;
    s16 state, timer;
} RoomOrbitPairParticle;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOrbitPairChannel, pool) == 8,
                  room_orbit_pair_pool_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOrbitPairNode, state) == 0x18,
                  room_orbit_pair_node_state_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomOrbitPairEventState, active) == 0xD,
                  room_orbit_pair_event_active_offset);
PE1_STATIC_ASSERT(sizeof(RoomOrbitPairParticle) == 0x14,
                  room_orbit_pair_particle_size);

extern RoomOrbitPairTemplate8 D_8018F03C;
extern RoomOrbitPairChannel *D_800F32D0, *D_800F33E0;
extern RoomOrbitPairEventState *D_800E2368;
extern s32 D_800E27EC;
extern volatile u16 D_800E11EA;
extern u16 D_800E2850[];
extern s16 D_80192BFC;
extern volatile s16 D_800F336C, D_800F336E, D_800F3372, D_800F3374, D_800F3370;
extern volatile s16 D_800F3368, D_800F336A, D_800F3376, D_800F3378;
extern int func_800CE560(void *, int, int, void *);
extern void func_800CE8F0(void *, int, void *, void *);
extern void func_800CE9D4(void *, int, void *);
extern RoomOrbitPairParticle *func_800CE610(void *);
extern int rcos(int);
extern int rsin(int);
extern void func_8018F058(void);
extern void func_8019050C(void);
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);
extern int func_80071A54(void);

int func_80190230(int mode, void *unused, s32 *state);
int func_801908B0(int mode, void *unused, void *state);

#endif
