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
    u8 reserved18[0x220];
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

#endif
