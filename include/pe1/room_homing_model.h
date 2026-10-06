#ifndef PE1_ROOM_HOMING_MODEL_H
#define PE1_ROOM_HOMING_MODEL_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/room_floor.h"

/* Room projectile that flies along its heading, bounces off the walkable
 * polygon's edges, drops onto the floor and finally hands its position to
 * the scene script. */

typedef union RoomHomingModelCoord {
    s32 word;                     /* 16.16 */
    struct {
        int fraction : 16;
        int integer : 16;
    } part;
} RoomHomingModelCoord;

typedef struct RoomHomingModel {
    RoomHomingModelCoord position[3]; /* 0x00 */
    s32 floor;                    /* 0x0C */
    s32 speed;                    /* 0x10: 16.16 */
    s32 scriptArg0;               /* 0x14: reported with the resting position */
    s32 scriptArg1;               /* 0x18 */
    s32 drag;                     /* 0x1C */
    s32 lift;                     /* 0x20 */
    s32 gravity;                  /* 0x24 */
    s32 soundHandle;              /* 0x28 */
    s16 angle;                    /* 0x2C */
    s16 state;                    /* 0x2E */
    s16 finished;                 /* 0x30 */
    s16 matrixIndex;              /* 0x32 */
    s16 escaped;                  /* 0x34 */
    s16 bounce;                   /* 0x36 */
    s16 damping;                  /* 0x38 */
    u8 released;                  /* 0x3A: the throw ended or was interrupted */
    u8 armed;                     /* 0x3B: the throw animation took the cue */
} RoomHomingModel;

PE1_STATIC_ASSERT(sizeof(RoomHomingModel) == 0x3C, room_homing_model_size);

typedef struct RoomHomingModelEvent {
    u8 reserved[0x14];
    RoomHomingModel *model;       /* 0x14 */
} RoomHomingModelEvent;

typedef struct RoomHomingModelActorCore {
    u32 flags;                    /* 0x00 */
    s32 reserved04;
    s32 sound;                    /* 0x08 */
    s32 reserved0C[3];
    u8 *cue;                      /* 0x18: throw cue, 1 armed, 2 taken, 4 done */
} RoomHomingModelActorCore;

typedef struct RoomHomingModelActor {
    RoomHomingModelActorCore *core; /* 0x00 */
    u8 reserved04[0xA];
    u8 phase;                     /* 0x0E */
    u8 frameCount;                /* 0x0F */
    u8 reserved10[6];
    u16 frame;                    /* 0x16 */
    u8 reserved18[4];
    s32 motion;                   /* 0x1C: playback step, negated to reverse */
    u8 reserved20[0x1A];
    u16 heading;                  /* 0x3A */
    u8 reserved3C[0x1FC];
    GteMatrix *matrices;          /* 0x238 */
} RoomHomingModelActor;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomHomingModelActor, matrices) == 0x238,
                  room_homing_model_actor_matrices);

typedef struct RoomHomingModelActorChannel {
    s32 reserved[2];
    RoomHomingModelActor *actor;  /* 0x08 */
} RoomHomingModelActorChannel;

typedef struct RoomHomingModelSparkChannel {
    s32 reserved[2];
    char *pool;                   /* 0x08 */
} RoomHomingModelSparkChannel;

typedef struct RoomHomingModelSpark {
    s16 x, y, z;
} RoomHomingModelSpark;

/* Walkable polygon vertex: x and the packed y/z word. */
typedef struct RoomHomingModelVertex {
    s16 reserved00;
    u16 x;                        /* 0x02 */
    u32 yz;                       /* 0x04 */
} RoomHomingModelVertex;

typedef struct RoomHomingModelEntity {
    u8 reserved[0x28];
    s32 position[3];              /* 0x28 */
} RoomHomingModelEntity;

extern RoomHomingModelEvent *D_800E2368;
extern RoomHomingModelActorChannel *D_800F32D0;
extern RoomHomingModelSparkChannel *D_800F33E0;
extern RoomHomingModelEntity *D_8009D254;
extern RoomHomingModelVertex *D_8009D248;
extern u16 D_8009D1CC;
/* One packed word of the PSY-Q sine table: sine low, cosine high. */
typedef union RoomHomingTrig {
    s32 word;
    struct {
        s16 sin;
        s16 cos;
    } part;
} RoomHomingTrig;

extern RoomHomingTrig D_800966EC[];
/* Texture page index slots and the trailing parameter pair, read and
 * written as records so they stay ordered with the parameter-block
 * pointer stores, as retail does. */
typedef struct RoomHomingModelTextureSlot {
    u16 index;
} RoomHomingModelTextureSlot;

typedef struct RoomHomingModelParamsTail {
    s16 parameter0A;
    s16 depth;
} RoomHomingModelParamsTail;

extern RoomHomingModelTextureSlot D_800E11EC;
extern RoomHomingModelTextureSlot D_800E11E8;
extern RoomHomingModelParamsTail D_800F3372;

extern int func_800CE560(void *pool, int size, int count, void *callback);
extern RoomHomingModelSpark *func_800CE610(void *pool);
extern int func_8006DCE4(int id, int channel, int x, int y, int z);
extern int FieldEng_VecToAngle(s32 *vec, s32 *ref);
extern int func_800DFE20(s32 *vec, s32 *ref);
extern int rcos(int angle);
extern int rsin(int angle);
extern void func_800866A4(int handle, int arg);
extern int func_8006F39C(int id, void *actor);
extern void func_8006F6D4(int handle, int, int index, int value, int, int);
extern u16 func_80077AA4(int, int);
/* Battle_GetEnemyContextField in main. */
extern int func_8003010C(void *actor, int field);


/* The projectile's constant seeds: the spark sprite's rotation and colour,
 * the model words reported to the scene script, and the floor ring's
 * rotation and colours. They are the shared unit's own data
 * (src/overlays/room_lib/RoomEffect_HomingProjectile.c); scene_e04 and
 * scene_e05 keep them past the end of their extracted image and name them
 * in their symbol files. */
extern GteRotation g_RoomHomingSparkRotation;
extern RenderColor g_RoomHomingSparkColor;
extern s16 g_RoomHomingReportFields[5];
extern GteRotation g_RoomHomingRingRotation;
extern RenderColor g_RoomHomingRingInner;
extern RenderColor g_RoomHomingRingOuter;

/* Launch parameters set by the scene script through the configure entry
 * and copied into the model when the throw starts. Each room keeps them in
 * its own data and names them in its symbol file. */
typedef struct RoomHomingLaunch {
    s32 speed;                    /* 0x00 */
    s32 drag;                     /* 0x04 */
    s32 lift;                     /* 0x08 */
    s32 gravity;                  /* 0x0C */
    s32 scriptArg0;               /* 0x10 */
    s32 scriptArg1;               /* 0x14 */
    s32 bounce;                   /* 0x18 */
    s32 damping;                  /* 0x1C */
} RoomHomingLaunch;

extern RoomHomingLaunch g_RoomHomingLaunch;

int RoomEffect_HomingProjectileSpark(int mode, GteShortVector *spark);
int RoomEffect_HomingProjectile(int mode);
int RoomEffect_HomingProjectileThrow(int mode, RoomHomingModel *model);
int RoomEffect_HomingProjectileConfigure(u32 mode, int a, int b, int c);

#endif
