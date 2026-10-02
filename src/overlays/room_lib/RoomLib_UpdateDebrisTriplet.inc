#include "common.h"
#include "pe1/field_script_context.h"
#ifndef ROOMLIB_UPDATE_DEBRIS_TRIPLET_FUNC
#error "ROOMLIB_UPDATE_DEBRIS_TRIPLET_FUNC must name the room entry point"
#endif

/* 16.16 fixed-point position; the integer halves are read as `>> 16`,
 * which the compiler narrows to halfword loads as retail does. */
typedef struct RoomDebrisPosition {
    s32 x, y, z;
    s32 reserved0C;
} RoomDebrisPosition;

typedef struct RoomDebrisVelocity {
    s16 x, y, z, reserved06;
} RoomDebrisVelocity;

/* Three pieces of debris that fall, bounce on the floor, and fade. */
typedef struct RoomDebrisTriplet {
    s16 remaining;                /* 0x00 */
    s16 speed;                    /* 0x02 */
    s16 acceleration;             /* 0x04 */
    s16 swingLimit;               /* 0x06 */
    s16 reserved08;
    u8 keepAlive;                 /* 0x0A */
    s8 jitter[3];                 /* 0x0B */
    u8 mode[4];                   /* 0x0E */
    u8 landed[4];                 /* 0x12 */
    s8 active[4];                 /* 0x16 */
    s8 countdown[4];              /* 0x1A */
    u8 size[4];                   /* 0x1E */
    u16 life[4];                  /* 0x22 */
    u16 swing[4];                 /* 0x2A */
    u16 phase[4];                 /* 0x32 */
    s16 reserved3A;
    RoomDebrisPosition position[4]; /* 0x3C */
    RoomDebrisVelocity reserved7C[4];
    RoomDebrisVelocity velocity[4]; /* 0x9C */
} RoomDebrisTriplet;

typedef struct RoomDebrisOwner {
    u8 reserved00;
    u8 phase;                     /* 0x01 */
    s16 angle;                    /* 0x02 */
} RoomDebrisOwner;

typedef struct RoomDebrisClock {
    u8 reserved[0x28];
    s16 landedFlag;               /* 0x28 */
} RoomDebrisClock;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDebrisTriplet, position) == 0x3C,
                  room_debris_triplet_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDebrisTriplet, velocity) == 0x9C,
                  room_debris_triplet_velocity_offset);

extern void *D_8009D248;
extern u16 D_8009D1CC;
extern int func_80071A54(void);
extern int rsin(int);
extern int func_8001CAB0(int, int, void *, int);
extern int func_800C6B90(void *position, int radius);

void ROOMLIB_UPDATE_DEBRIS_TRIPLET_FUNC(void *unused, RoomDebrisOwner *owner,
                                        RoomDebrisTriplet *state) {
    s16 point[4];
    RoomDebrisClock *clock;
    unsigned int i;
    int step;

    clock = func_800C2B50();
    for (i = 0; i < 3; i++) {
        if (state->countdown[i] != 0) {
            state->countdown[i]--;
            if (state->countdown[i] == 0) state->active[i] = 0;
        }
        if (state->active[i] == 0) {
            if (state->size[i] < 0x80) {
                state->size[i] += 4;
            } else {
                state->active[i] = 1;
            }
        }
        if (state->mode[i] == 1) {
            state->position[i].x += (state->velocity[i].x * state->speed) >> 4;
            state->position[i].y += (state->velocity[i].y * state->speed) >> 4;
            state->position[i].z += (state->velocity[i].z * state->speed) >> 4;
        }
        if (state->swing[i] < state->swingLimit) {
            state->swing[i] += func_80071A54() % 80 + 20;
            state->phase[i] += func_80071A54() % 40 + 40;
            state->position[i].y += -0xA0000;
        }
        state->swing[i] += rsin(owner->angle << 5) >> 7;
        if (state->life[i] < 0x14) {
            if (state->size[i] >= 7) state->size[i] -= 6;
        }
        if (state->landed[i] == 0) {
            if (!func_8001CAB0((state->position[i].x >> 16) << 16,
                               (state->position[i].z >> 16) << 16,
                               D_8009D248, D_8009D1CC)) {
                state->landed[i] = 1;
                state->mode[i] = 0;
                if (state->keepAlive == 0) state->life[i] = 0x14;
            }
        }
        state->life[i]--;
        if (state->life[i] == 0) {
            state->active[i] = -1;
            state->remaining--;
        }
        point[0] = (state->position[i].x >> 16);
        point[1] = (state->position[i].y >> 16);
        point[2] = (state->position[i].z >> 16);
        state->jitter[0] = func_80071A54() % 11 - 5;
        state->jitter[1] = func_80071A54() % 11 - 5;
        state->jitter[2] = func_80071A54() % 11 - 5;
        if (func_800C6B90(point, state->swing[i] >> 4)) {
            if (state->landed[i] == 0) {
                clock->landedFlag = 1;
                state->life[i] = 0x14;
                state->mode[i] = 0;
            }
            state->landed[i] = 1;
        }
    }
    state->speed += state->acceleration;
    if ((s16)state->speed < 0) state->speed = 0;
    if (state->remaining == 0) owner->phase = 2;
}

#undef ROOMLIB_UPDATE_DEBRIS_TRIPLET_FUNC
