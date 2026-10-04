#ifndef PE1_SCENE_E19_2_HOMING_BURST_H
#define PE1_SCENE_E19_2_HOMING_BURST_H

#include "pe1/render_object.h"
#include "pe1/gte.h"

/* Scene e19_2 homing burst controller: anchored on the room actor, it
 * releases twenty homing particles (func_8019549C) on odd frames, then
 * draws a fading flash, a screen tint and a shrinking set of glows. */
typedef struct SceneHomingBurst {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ s16 timer;
    /* 0x0A */ s16 released;
} SceneHomingBurst;

/* Child particle fields the controller seeds. */
typedef struct SceneHomingBurstChild {
    /* 0x00 */ s16 x, y, z;
    /* 0x06 */ s16 index;
    /* 0x08 */ u8 pad08[0x14];
    /* 0x1C */ s16 state;
    /* 0x1E */ s16 timer;
} SceneHomingBurstChild;

typedef struct SceneHomingBurstObject {
    /* 0x00 */ u8 pad00[0x18];
    /* 0x18 */ u8 *status;
} SceneHomingBurstObject;

typedef struct SceneHomingBurstPool {
    /* 0x00 */ SceneHomingBurstObject *object;
} SceneHomingBurstPool;

typedef struct SceneHomingBurstChannel {
    s32 reserved[2];
    SceneHomingBurstPool *pool; /* 0x08 */
} SceneHomingBurstChannel;

typedef struct SceneHomingBurstEvent {
    /* 0x00 */ u8 pad00[0xD];
    /* 0x0D */ u8 active;
} SceneHomingBurstEvent;

extern GteRotation D_8018F210;
extern GteRotation D_8018F1CC;
extern RenderColor D_8018F21C;
extern RenderColor D_8018F220;
/* The texture page index read as a one-field record so the read stays below
 * the parameter block store made through its base register. */
typedef struct SceneHomingBurstPageIndex {
    u16 value;
} SceneHomingBurstPageIndex;

extern SceneHomingBurstPageIndex D_800E11EA;
extern SceneHomingBurstChannel *D_800F32D0, *D_800F33E0;
extern SceneHomingBurstEvent *D_800E2368;

int func_80077CF4(int angle);
u16 func_80077AA4(int x, int y);
int func_800CE560(void *pool, int size, int count, void *callback);
SceneHomingBurstChild *func_800CE610(void *pool);
void func_800CE8F0(void *pool, int index, GteRotation *rotation, GteShortVector *position);
void func_800D1AE0(RenderColor *color, int value, int step, int count);
int func_8019549C(int mode, void *particle);

#endif
