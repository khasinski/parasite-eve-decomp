#ifndef PE1_SCENE_E18_EFFECTS_H
#define PE1_SCENE_E18_EFFECTS_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Particle emitters of the scene_e18 script: one rises from random model
 * joints and fades out, the other drops wave rings from random joints. */

typedef struct SceneE18Owner {
    u32 flags;                    /* 0x00 */
    s32 reserved04;
    s32 asset;                    /* 0x08 */
    u8 reserved0C[0xC];
    u8 *stage;                    /* 0x18 */
    u8 reserved1C[0x30];
    u32 status;                   /* 0x4C */
} SceneE18Owner;

typedef struct SceneE18Instance {
    SceneE18Owner *owner;         /* 0x00 */
    u8 reserved04[0xA];
    u8 animationId;               /* 0x0E */
    u8 frameLimit;                /* 0x0F */
    u8 reserved10[4];
    union {
        s32 word;                 /* 0x14 */
        struct {
            u16 step;
            u16 frame;            /* 0x16 */
        } part;
    } animation;
    u8 reserved18[0x10];
    s32 x, y, z;                  /* 0x28: 16.16 position */
    u8 reserved34[0x1B4];
    GteMatrix transform;          /* 0x1E8 */
    u8 reserved208[0x30];
    GteMatrix *transforms;        /* 0x238 */
} SceneE18Instance;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE18Instance, transforms) == 0x238,
                  scene_e18_instance_transforms);

typedef struct SceneE18Actor {
    s32 reserved[2];
    SceneE18Instance *instance;   /* 0x08 */
} SceneE18Actor;

typedef struct SceneE18Emitter {
    s32 reserved[2];
    void *pool;                   /* 0x08 */
} SceneE18Emitter;

typedef struct SceneE18EmitterState {
    s16 count;
    s16 delay;
} SceneE18EmitterState;

typedef struct SceneE18RisingParticle {
    GteShortVector position;
    s16 size;                     /* 0x08 */
} SceneE18RisingParticle;

typedef struct SceneE18WaveParticle {
    GteShortVector position;
    s16 controls[4];              /* 0x08 */
    s16 brightness;               /* 0x10 */
    s16 size;                     /* 0x12 */
    s16 amplitude;                /* 0x14 */
} SceneE18WaveParticle;

extern SceneE18Actor *D_800F32D0;
extern SceneE18Emitter *D_800F33E0;
extern u8 D_801941D0;
extern u16 D_800E11E4[];

extern int func_800CE560(void *pool, int size, int count, void *callback);
extern void *func_800CE610(void *pool);
extern int func_8006DCE4(int id, int channel, int x, int y, int z);
extern int func_80052B2C(void);
extern int func_80071A54(void);
extern int rcos(int angle);
extern int rsin(int angle);
extern int func_80193488(int mode, SceneE18RisingParticle *particle);
extern int func_80193A58(int mode, SceneE18WaveParticle *particle);

/* Flank model emitter (func_80192B8C): four model particles that follow
 * the room model's translation and alternate sides, the model asset it
 * looks up and the frame countdown its particles run down. */
typedef struct SceneE18FlankParticle {
    s32 *position;                /* 0x00: the room model's translation */
    s16 angle;                    /* 0x04 */
    s16 turn;                     /* 0x06 */
    s16 width;                    /* 0x08 */
    s16 height;                   /* 0x0A */
    s16 widthGrowth;              /* 0x0C */
    s16 heightGrowth;             /* 0x0E */
    s16 brightness;               /* 0x10 */
    s16 leader;                   /* 0x12 */
} SceneE18FlankParticle;

extern void *D_800B0E64;
extern void *D_801941C4;
extern s16 D_801941C8;
extern u16 D_800E11FA;
extern void *func_8006E498(void *base, u32 key);
extern void func_800C6D5C(void *asset, int x, int y);
extern int func_80077A64(int, int, int, int);
extern u16 func_80077AA4(int, int);
extern void func_800C6EC0(int tpage, int clut);
extern void func_800C6ED8(int mode);
extern int func_801928CC(int mode, SceneE18FlankParticle *particle);

/* Flank model particle (func_801928CC): the player instance whose distance
 * to the model releases the actor, and the model draw helpers. */
extern SceneE18Instance *RoomMain_ActorPtr;
extern int func_8005186C(int value);
extern int func_80077CF4(int angle);
extern int func_80077DC4(int angle);
extern void func_80071A44(void *data, int value, int size);
extern void func_80078CC4(GteMatrix *matrix, GteVector *scale);
extern void func_800C6EF8(void *asset);
extern void func_800C6FA0(void *asset, u16 intensity);
extern void func_800C71E4(void *asset, GteMatrix *matrix);
extern void func_800C6F4C(void *asset);

/* Pulse ring particle (func_80193018): shrinking light rings around the
 * room model; the last one releases the actor once the player is near. */
typedef struct SceneE18PulseRing {
    s16 size;                     /* 0x00 */
    s16 shrink;                   /* 0x02 */
    s16 radius;                   /* 0x04 */
    u8 active;                    /* 0x06 */
    u8 last;                      /* 0x07 */
} SceneE18PulseRing;

extern RenderColor D_8019411C[2];

/* Spinning model controller (func_801924E8): spins the scene model's
 * glow shell around the room model while it grows and flares, and sets
 * the model's stage byte once its seventh animation ends. */
typedef struct SceneE18SpinShell {
    s16 width;                    /* 0x00 */
    s16 height;                   /* 0x02 */
    s16 widthGrowth;              /* 0x04 */
    s16 heightGrowth;             /* 0x06 */
    s16 angle;                    /* 0x08 */
    s16 turn;                     /* 0x0A */
    s16 brightness;               /* 0x0C */
} SceneE18SpinShell;

extern void *D_801941C0;

#endif
