#ifndef PE1_ROOM_EMBER_BURST_H
#define PE1_ROOM_EMBER_BURST_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/field_actor.h"
#include "pe1/room_orbit_trail.h"

/* Ember burst controller (room_m318): attaches to the actor named by its
 * parameters, glows up, bursts orbiting sparks, embers and smoke, then
 * fades a halo and ring. */

typedef struct RoomEmberBurst {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s16 state;                    /* 0x08 */
    s16 timer;                    /* 0x0A */
    s16 glow;                     /* 0x0C */
} RoomEmberBurst;

typedef struct RoomEmberBurstParams {
    s32 subId;                    /* 0x00 */
    s32 typeId;                   /* 0x04 */
} RoomEmberBurstParams;

typedef struct RoomEmberBurstChannel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} RoomEmberBurstChannel;

extern FieldActor *D_8009D20C;
extern FieldActor *D_8009D254;
extern RoomEmberBurstChannel *D_800F33E0;
extern char D_8018F1CC[];
extern RenderColor D_8018F200;
extern RenderColor D_8018F204;
extern u16 D_800E11EA;

extern void func_80071A74();
extern int func_80071A54(void);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern int func_800CE560(void *pool, int size, int count, void *callback);
extern RoomOrbitTrailParticle *func_800CE610(void *pool);
extern int func_800D3FD8(void);
extern int func_800D3F64(int sound, int handle);
extern int func_801944E8(int mode, RoomOrbitTrailParticle *p);

/* Ember fountain controller (func_80197CBC): follows its actor for 141
 * frames while it sprays embers, smoke and orbiting sparks. */
typedef struct RoomEmberFountain {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s16 reserved08;
    s16 timer;                    /* 0x0A */
    s16 glow;                     /* 0x0C */
    s16 ringScale;                /* 0x0E */
} RoomEmberFountain;

typedef struct RoomEmberFountainChannel {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} RoomEmberFountainChannel;

extern RoomEmberFountainChannel *D_800F32D0;
extern GteShortVector D_8019993C;
extern int func_80197618(int mode, RoomOrbitTrailParticle *p);

/* Ember fountain particle (func_80197618). */
typedef struct RoomEmberFloor {
    s16 y;
} RoomEmberFloor;

extern RoomEmberFloor D_800942EC;
extern u16 D_800E11E8;
extern u16 D_800E11FA;
extern GteRotation D_8018F1F4;
extern RenderColor D_8018F240;
extern u8 D_80199868[];
extern u16 GetClut(int x, int y);

/* Twisting ember particle (func_80198268): its glow track and the bent
 * trail renderer it draws the orbiting sparks with. */
extern u8 D_80199890[];
extern void func_800CF844(void *, void *, int, void *, int, int);

/* Fan sweep controller (func_80192A7C): attaches to the actor named by its
 * parameters, fans out sparks on a turning angle into its own pool and
 * publishes that pool and its anchor for the pool's callbacks. */
typedef struct RoomFanSweep {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s32 angle;                    /* 0x08 */
    void *pool;                   /* 0x0C */
} RoomFanSweep;

/* Fan sweep spark: circles the published anchor on its angle at its
 * radius, sheds trail points, then swells, holds and fades. */
typedef struct RoomFanSweepSpark {
    s16 state;                    /* 0x00 */
    u16 angle;                    /* 0x02 */
    u16 timer;                    /* 0x04 */
    s16 x, y, z;                  /* 0x06 */
    s16 radius;                   /* 0x0C */
    u16 reserved0E;               /* 0x0E */
} RoomFanSweepSpark;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFanSweepSpark, x) == 0x06,
                  room_fan_sweep_spark_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomFanSweepSpark, radius) == 0x0C,
                  room_fan_sweep_spark_radius_offset);
PE1_STATIC_ASSERT(sizeof(RoomFanSweepSpark) == 0x10,
                  room_fan_sweep_spark_size);

extern void *D_80199900;
extern RenderColor D_8018F1F0;
extern GteShortVector D_801998F8;
extern int func_80192718(int mode, RoomFanSweepSpark *spark);
/* Main executable copy of the fan sweep spark (func_800D71B8). */
extern void *D_800E21E8;
extern RenderColor D_800C22D4;
extern GteShortVector D_800E21E0;
extern int func_800D71B8(int mode, RoomFanSweepSpark *spark);
extern int rand(void);
extern int func_80192620(int mode, s16 *position);
extern int func_800CE5AC(void **pool, int owner, int size, int count, void *callback);
extern int func_800CE688(void *pool);
extern int func_800CE78C(void *pool);

/* Spiral drop controller (func_80192ED4): attaches to the actor named by
 * its parameters and drops falling sparks from a point that circles it on
 * a turning angle at a pulsing radius. */
typedef struct RoomSpiralDrop {
    s16 x, y, z;                  /* 0x00 */
    s16 reserved06;
    s32 angle;                    /* 0x08 */
    s32 radius;                   /* 0x0C */
} RoomSpiralDrop;

typedef struct RoomSpiralDropSpark {
    s16 x, y, z;                  /* 0x00 */
    s16 speed;                    /* 0x06 */
} RoomSpiralDropSpark;

extern u8 D_801994BC[];
extern int func_80192DA0(int mode, RoomSpiralDropSpark *spark);

/* Attaches the effect to the first live actor with the given ids, or
 * reports the missing actor. */
static inline void RoomEffect_AttachToActor(int subId, int typeId, void *anchor) {
    FieldActor *actor;

    for (actor = D_8009D20C; actor != 0; actor = actor->next) {
        if (actor != D_8009D254 && actor->state != 0 &&
            actor->state->control10.command_value > 0 &&
            actor->sub_id == subId && actor->type_id == typeId) {
            func_800CE870((char *)actor, 0, (s16 *)anchor);
            return;
        }
    }
    func_80071A74(D_8018F1CC, subId, typeId);
}

#endif
