#ifndef PE1_ROOM_HOMING_MODEL_H
#define PE1_ROOM_HOMING_MODEL_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

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
    s32 reserved14;
    s32 reserved18;
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
} RoomHomingModel;

PE1_STATIC_ASSERT(sizeof(RoomHomingModel) == 0x3C, room_homing_model_size);

typedef struct RoomHomingModelEvent {
    u8 reserved[0x14];
    RoomHomingModel *model;       /* 0x14 */
} RoomHomingModelEvent;

typedef struct RoomHomingModelActorCore {
    s32 reserved[2];
    s32 sound;                    /* 0x08 */
} RoomHomingModelActorCore;

typedef struct RoomHomingModelActor {
    RoomHomingModelActorCore *core; /* 0x00 */
    u8 reserved04[0xA];
    u8 phase;                     /* 0x0E */
    u8 reserved0F[7];
    u16 frame;                    /* 0x16 */
    u8 reserved18[0x22];
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

/* Frame counter read unsigned as a record so the spark stores keep it in place. */
typedef struct RoomHomingModelFrameTick {
    u16 count;
} RoomHomingModelFrameTick;

extern RoomHomingModelEvent *D_800E2368;
extern RoomHomingModelActorChannel *D_800F32D0;
extern RoomHomingModelSparkChannel *D_800F33E0;
extern RoomHomingModelEntity *D_8009D254;
extern RoomHomingModelVertex *D_8009D248;
extern u16 D_8009D1CC;
extern RoomHomingModelFrameTick D_800942EC;
extern s16 D_800966EC[];
extern s16 D_800966EE[];
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

#endif
