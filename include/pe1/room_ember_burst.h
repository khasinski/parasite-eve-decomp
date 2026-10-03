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

#endif
