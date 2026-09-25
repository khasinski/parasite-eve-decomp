#ifndef PE1_ROOM_M404_H
#define PE1_ROOM_M404_H

#include "common.h"

typedef struct { s32 word[2]; } __attribute__((packed)) RoomTemplate8;
typedef struct RoomM404Node {
    u8 reserved[0x18];
    u8 *state;
} RoomM404Node;
typedef struct RoomM404Channel {
    s32 reserved[2];
    char *pool;
} RoomM404Channel;
typedef struct RoomM404EventState {
    u8 reserved[0xD];
    u8 active;
} RoomM404EventState;
typedef struct RoomM404Particle {
    s16 x, y, z, unused06;
    s16 vx, vy, vz, unused0E;
    s16 state, timer;
} RoomM404Particle;
extern RoomTemplate8 D_8018F208;
extern s32 D_800E27EC;
extern volatile u16 D_800E11EA;
extern u16 D_800E2850[];
extern volatile s16 D_800F3368, D_800F336A, D_800F3376, D_800F3378;
extern volatile s16 D_800F336C, D_800F336E, D_800F3372, D_800F3374, D_800F3370;
extern RoomM404Channel *D_800F32D0, *D_800F33E0;
extern RoomM404EventState *D_800E2368;
extern int func_800D3FD8(void);
extern void func_800D3F64(int, int);
extern int func_800CE560(void *, int, int, void *);
extern void func_800CE8F0(void *, int, void *, void *);
extern void func_800CE9D4(void *, int, void *);
extern RoomM404Particle *func_800CE610(void *);
extern void func_800CFB7C(void *, int, void *);
extern void func_80192540(void);

#endif
