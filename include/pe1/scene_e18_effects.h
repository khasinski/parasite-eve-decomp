#ifndef PE1_SCENE_E18_EFFECTS_H
#define PE1_SCENE_E18_EFFECTS_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Particle emitters of the scene_e18 script: one rises from random model
 * joints and fades out, the other drops wave rings from random joints. */

typedef struct SceneE18Owner {
    s32 reserved[2];
    s32 asset;                    /* 0x08 */
} SceneE18Owner;

typedef struct SceneE18Instance {
    SceneE18Owner *owner;         /* 0x00 */
    u8 reserved04[0xB];
    u8 frameLimit;                /* 0x0F */
    u8 reserved10[4];
    union {
        s32 word;                 /* 0x14 */
        struct {
            u16 step;
            u16 frame;            /* 0x16 */
        } part;
    } animation;
    u8 reserved18[0x1D0];
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
    s16 phase;                    /* 0x04 */
    s16 side;                     /* 0x06 */
    s16 scaleX;                   /* 0x08 */
    s16 scaleY;                   /* 0x0A */
    s16 spin;                     /* 0x0C */
    s16 tilt;                     /* 0x0E */
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

#endif
