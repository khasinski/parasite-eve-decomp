#include "common.h"

typedef struct RoomTimedSpawnState {
    s16 x;
    s16 y;
    s16 z;
} RoomTimedSpawnState;

typedef struct RoomTimedSpawnSoundState {
    void *owner;
    u16 x;
    u16 y;
    u16 z;
} RoomTimedSpawnSoundState;

extern void *D_800F32D0;
extern void *D_800F33E0;
extern void *D_800E2368;
extern void *D_8009D254;
extern void *g_PlayerEntity;
extern void *RoomMain_ActorPtr2;
extern s16 D_800942EC;
extern u16 g_FrameCount16;
extern s32 D_800E27EC;
extern u16 D_800E11EA;
extern u16 D_800E2850[];
extern RoomTimedSpawnSoundState D_800B0E64;
extern s16 D_800F3368;
extern s16 D_800F336A;
extern u16 D_800F336C;
extern s16 D_800F336E;
extern u16 D_800F3370;
extern s16 D_800F3372;
extern s16 D_800F3374;
extern s16 D_800F3376;
extern s16 D_800F3378;

void func_800CE870(void *arg0, int arg1, void *arg2);
void *func_800CE610(void *arg0);
int func_800CE560(void *arg0, int arg1, int arg2, void *callback);
int func_800D3FD8(void);
void Akao_SendTableCommand(void *owner, int id, int value, int volume, int pan);

extern void ROOMLIB_TIMED_SPAWN_CALLBACK(void);

#ifndef ROOMLIB_TIMED_SPAWN_ACTOR_PTR
#define ROOMLIB_TIMED_SPAWN_ACTOR_PTR g_PlayerEntity
#endif

#ifndef ROOMLIB_TIMED_SPAWN_FRAME_COUNT
#define ROOMLIB_TIMED_SPAWN_FRAME_COUNT D_800942EC
#endif

int ROOMLIB_CONTROL_TIMED_SPAWN_NAME(int mode, RoomTimedSpawnState *state) {
    void *actor;
    int tick;
    int value;
    u16 ground;

    if (mode == 1) {
        goto mode1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto mode0;
        }
        goto ret0;
    }
    if (mode == 2) {
        goto mode2;
    }
    goto ret0;

mode0:
    func_800CE870(((void **)D_800F32D0)[2], 0, (char *)state + 4);
    state->x = 0;
    state->y = 0;

    if (*(u8 *)((char *)D_800E2368 + 0xD) != 0) {
        void *p = ((void **)D_800F32D0)[2];
        if (p != 0 && *(void **)p != 0) {
            u8 *status = *(u8 **)((char *)*(void **)p + 0x18);
            if (*status == 1) {
                *status = 2;
            }
        }
    }

    if (D_800B0E64.owner != 0) {
        value = func_800D3FD8();
        Akao_SendTableCommand(D_800B0E64.owner, 0x588, value, 0x80, 0x7F);
    }
    return func_800CE560(((void **)D_800F33E0)[2], 0xC, 8,
                         ROOMLIB_TIMED_SPAWN_CALLBACK);

mode1:
    tick = D_800E27EC;
    if (tick < 0x20 && tick % 6 == 0) {
        actor = func_800CE610(((void **)D_800F33E0)[2]);
        if (actor != 0) {
            *(u16 *)((char *)actor + 0) = *(u16 *)((char *)state + 4);
            *(u16 *)((char *)actor + 2) = *(u16 *)((char *)state + 6);
            *(u16 *)((char *)actor + 4) = *(u16 *)((char *)state + 8);
            ground = ROOMLIB_TIMED_SPAWN_FRAME_COUNT;
            *(s16 *)((char *)actor + 8) = 0;
            *(s16 *)((char *)actor + 0xA) = 0;
            *(u16 *)((char *)actor + 2) = ground;
        }
    }

    if (D_800E27EC == 7 && *(u8 *)((char *)D_800E2368 + 0xD) != 0) {
        register void *root asm("$5");
        root = D_800F32D0;
        if ((*(u32 *)*(void **)((void **)root)[2] & 0x3F000000) ==
            0x01000000) {
            void *object;
            void *engine = *(void **)ROOMLIB_TIMED_SPAWN_ACTOR_PTR;
            *(u32 *)((char *)engine + 0x4C) |= 0x4000;

            object = *(void **)((void **)root)[2];
            *(u32 *)object = (*(u32 *)object & 0xC0FFFFFF) | 0x21000000;

            object = *(void **)((void **)root)[2];
            *(u32 *)object |= 0x80000000;
        }
    }
    if (D_800E27EC < 8) {
        goto ret0;
    }
    return 2;

mode2:
    D_800F3368 = 0x20;
    D_800F336A = 2;
    D_800F3376 = 0x20;
    D_800F3378 = 0x20;
    D_800F336C = 3;
    D_800F336E = 0;
    D_800F3372 = 0;
    D_800F3374 = 0;
    D_800F3370 = D_800E2850[D_800E11EA];
ret0:
    return 0;
}
