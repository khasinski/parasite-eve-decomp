#include "common.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

#ifndef ROOMEFFECT_SEEKING_TRAIL_FUNC
#error "ROOMEFFECT_SEEKING_TRAIL_FUNC must name the room entry point"
#endif

/* Head of a seeking trail and the sparks and flashes it spawns; all three
 * share this record and are told apart by their state. */
typedef struct RoomSeekingTrail {
    s16 x, y, z;                  /* 0x00 */
    s16 slot;                     /* 0x06: trail history index */
    s16 ax, ay, az;               /* 0x08: heading angles */
    s16 fade;                     /* 0x0E */
    s16 speed;                    /* 0x10 */
    u16 turn;                     /* 0x12 */
    s16 state;                    /* 0x14 */
    s16 timer;                    /* 0x16 */
} RoomSeekingTrail;

typedef struct RoomSeekingTrailHistory {
    GteShortVector point[25];
} RoomSeekingTrailHistory;

typedef struct RoomSeekingTrailChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomSeekingTrailChannel;

typedef struct RoomSeekingTrailPacket {
    u32 code;
} RoomSeekingTrailPacket;

typedef struct RoomSeekingTrailActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomSeekingTrailActor;

typedef struct RoomSeekingTrailEntity {
    RoomSeekingTrailActor *actor;
} RoomSeekingTrailEntity;

typedef struct RoomSeekingTrailEventState {
    u8 reserved[0xD];
    u8 active;
} RoomSeekingTrailEventState;

extern RenderColor ROOMEFFECT_SEEKING_TRAIL_COLOR_A;
extern RenderColor ROOMEFFECT_SEEKING_TRAIL_COLOR_B;
extern GteRotation ROOMEFFECT_SEEKING_TRAIL_ROTATION;
extern RoomSeekingTrailHistory ROOMEFFECT_SEEKING_TRAIL_HISTORY[];

extern RoomSeekingTrailChannel *D_800F32D0, *D_800F33E0;
extern RoomSeekingTrailEntity *D_8009D254;
extern RoomSeekingTrailEventState *D_800E2368;
extern u16 D_800E11EA;
extern u16 D_800E120A;
extern u16 D_800F336E, D_800F3376, D_800F3378;
extern int func_80071A54(void);
extern int func_80077DC4(int angle);
extern u16 func_80077AA4(int, int);
extern RoomSeekingTrail *func_800CE610(void *pool);
extern void func_800CE8F0(void *pool, int index, void *rotation, void *position);
extern void func_800CFAA8(void *from, void *to, void *angles);
extern void func_800CFD50(void *angles, void *out, int speed);
extern void func_800D3AFC(void *history, int count, void *position, int reset);
extern int func_800C6B90(void *position, int radius);
extern void func_800D3114(void *trail, int count, int length, int arg3, int arg4,
                          int arg5, int arg6, int tpage, int clut, int alpha,
                          void *headColor, void *tailColor, int mode);

int ROOMEFFECT_SEEKING_TRAIL_FUNC(int mode, RoomSeekingTrail *trail) {
    RenderColor colorA;
    RenderColor colorB;
    GteRotation rotation;
    GteShortVector target;
    GteShortVector angles;
    GteShortVector unused;
    RoomSeekingTrail *child;
    GteShortVector *point;
    int range;
    int i;
    int scale;
    int kind;
    int palette;
    u16 clut;
    u16 *palettes;

    colorA = ROOMEFFECT_SEEKING_TRAIL_COLOR_A;
    colorB = ROOMEFFECT_SEEKING_TRAIL_COLOR_B;
    rotation = ROOMEFFECT_SEEKING_TRAIL_ROTATION;
    switch (mode) {
    case 1:
        switch (trail->state) {
        case 0:
            trail->timer = 0;
            trail->state = 1;
            trail->fade = 0;
            func_800CE8F0(D_800F32D0->pool, 7, &rotation, trail);
            func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->pool, 0, &angles);
            angles.x = 0;
            trail->ax = angles.x;
            trail->ay = angles.y;
            trail->az = angles.z;
            func_800D3AFC(&ROOMEFFECT_SEEKING_TRAIL_HISTORY[trail->slot], 0x18, trail, 1);
            trail->speed = 0x20;
            trail->turn = 0x200;
            child = func_800CE610(D_800F33E0->pool);
            if (!child) return 0;
            child->x = trail->x;
            child->y = trail->y;
            child->z = trail->z;
            child->state = 3;
            child->timer = 0;
            break;
        case 1:
            trail->timer++;
            point = ROOMEFFECT_SEEKING_TRAIL_HISTORY[trail->slot].point;
            func_800CE870((char *)D_8009D254, 1, &target.x);
            func_800CFAA8(trail, &target, &angles);
            angles.x = 0;
            if (trail->fade == 0) {
                func_800CFD50(&angles, &trail->ax, trail->turn);
                if (trail->timer >= 3) {
                    range = trail->timer * 16 + 0x100;
                    if (!(func_80071A54() & 1)) {
                        trail->ay += func_80071A54() % (range * 2) - range;
                    } else {
                        trail->ay += func_80071A54() % range - (range >> 1);
                    }
                }
                if (trail->speed < 0x3C) trail->speed += 2;
                func_800CFB7C(&trail->ax, (s16)(trail->speed * trail->speed / 16), &target);
                trail->x += target.x;
                trail->y += target.y;
                trail->z += target.z;
            } else if (++trail->fade >= 16) {
                return 1;
            }
            func_800D3AFC(point, 0x18, trail, 0);
            point = ROOMEFFECT_SEEKING_TRAIL_HISTORY[trail->slot].point;
            for (i = 0; i < 0x18; i++, point++) {
                if (func_800C6B90(point, 0x68) && D_800E2368->active) {
                    RoomSeekingTrailChannel *channel = D_800F32D0;
                    if (((*(RoomSeekingTrailPacket **)channel->pool)->code & 0x3F000000) ==
                        0x01000000) {
                        D_8009D254->actor->flags |= 0x4000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x3F000000;
                        (*(RoomSeekingTrailPacket **)channel->pool)->code |= 0x80000000;
                    }
                }
            }
            point = ROOMEFFECT_SEEKING_TRAIL_HISTORY[trail->slot].point;
            if (trail->fade == 0) {
                child = func_800CE610(D_800F33E0->pool);
                if (child) {
                    point += (func_80071A54() & 3) * 4;
                    child->x = point->x;
                    child->y = point->y;
                    child->z = point->z;
                    child->x += -0x20 + (func_80071A54() & 0x3F);
                    child->y += -0x20 + (func_80071A54() & 0x3F);
                    child->z += -0x20 + (func_80071A54() & 0x3F);
                    child->ax = trail->ax;
                    child->ay = trail->ay;
                    child->az = trail->az;
                    child->ax += -100 + func_80071A54() % 200;
                    child->ay += -100 + func_80071A54() % 200;
                    child->speed = (func_80071A54() & 0x3F) - 0x20;
                    child->turn = (func_80071A54() & 0x3F) - 0x20;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (D_800E27EC < 0x20) return 0;
            if (trail->fade != 0) return 0;
            child = func_800CE610(D_800F33E0->pool);
            if (child) {
                child->x = trail->x;
                child->y = trail->y;
                child->z = trail->z;
                child->state = 3;
                child->timer = 0;
            }
            trail->fade = 1;
            break;
        case 2:
            trail->timer++;
            trail->ax += trail->turn;
            trail->ay += trail->speed;
            if (trail->timer >= 20) return 1;
            break;
        case 3:
            if (++trail->timer < 24) break;
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        switch (trail->state) {
        case 1:
            scale = ((D_800E27EC / 3) & 1) * 32 + 32;
            i = 0;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            {
                u16 kind = D_800F336C;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
            }
            clut = func_80077AA4(0x10, palette);
            func_800D3114(&ROOMEFFECT_SEEKING_TRAIL_HISTORY[trail->slot], 0x18, 0x78, i, scale,
                          0xFD, 0x1F, D_800E2850[D_800E11EA], clut, 0x80,
                          &colorA, &colorB, 1);
            func_800D004C(&ROOMEFFECT_SEEKING_TRAIL_HISTORY[trail->slot].point[func_80071A54() & 0xF],
                          0x190, 0x190, 6, 0, 0x1000, 0x1000, &colorA, 0, 0x80, 3);
            return 0;
        case 2:
            scale = func_80077DC4((trail->timer << 10) / 20) / 32;
            func_80077DC4((trail->timer << 10) / 20);
            /* Shares the loop index's register with retail (s0). */
            i = (D_800E27EC & 1) << 7;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            /* tpage is the first parameter store, so its address stays in a
             * register and parameter00 is later written through it. */
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x10, palette);
            func_800D2370((GteShortVector *)trail, (GteRotation *)&trail->ax, 0x1FE, 0x78,
                          i, 0, 0x80, 0x20, clut, &colorB, &colorA, (s16)scale, 1);
            palettes = D_800E1204;
            if (trail->timer < 13) {
                /* Block-scope temporaries: the kind read stays below the
                 * parameter00 store, as in retail. */
                int kind;
                int palette;

                scale = func_80077DC4((trail->timer << 10) / 12) / 64;
                D_800F3368.parameter00 = 0x20;
                D_800F336A = 2;
                D_800F3376 = 0x20;
                D_800F3378 = 0x20;
                kind = D_800F3368.palette;
                palette = palettes[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = func_80077AA4(0x40, palette);
                func_800CEE20((GteShortVector *)trail, 0, 0x1000, 0x1000,
                              D_800F336A * (trail->timer * 2 / 3) + 0x90, clut, 1,
                              scale, 0);
            }
            break;
        case 3:
            scale = func_80077DC4((trail->timer << 10) / 24) / 32;
            func_80077DC4((trail->timer << 10) / 24);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            target.x = trail->x;
            target.y = trail->y;
            target.z = trail->z;
            target.x += -0x10 + (func_80071A54() & 0x1F);
            target.y += -0x10 + (func_80071A54() & 0x1F);
            target.z += -0x10 + (func_80071A54() & 0x1F);
            kind = D_800F336C;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = func_80077AA4(0x40, palette);
            func_800CEE20(&target, 0, 0x1000, 0x1000,
                          D_800F336A * (trail->timer & 7) + 0x90, clut, 1, scale, 0);
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}
