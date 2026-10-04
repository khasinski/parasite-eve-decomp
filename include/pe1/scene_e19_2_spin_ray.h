#ifndef PE1_SCENE_E19_2_SPIN_RAY_H
#define PE1_SCENE_E19_2_SPIN_RAY_H

#include "pe1/render_object.h"
#include "pe1/gte.h"

/* Scene e19_2 spin ray: a spinning ray anchored at the scene origin that
 * grows, sweeps out as a textured blade and closes as a glow sprite. The
 * spin angles double as the draw rotation. */
typedef struct SceneSpinRay {
    /* 0x00 */ union {
        GteRotation rotation;
        GteShortVector angles;
    } spin;
    /* 0x08 */ s16 state;
    /* 0x0A */ u16 timer;
    /* 0x0C */ s16 reach;
    /* 0x0E */ s16 spinY;
} SceneSpinRay;

/* The controller: its frame timer and the number of blades left to cast. */
typedef struct SceneSpinRayBurst {
    /* 0x00 */ u16 timer;
    /* 0x02 */ s16 blades;
} SceneSpinRayBurst;

/* Particle pools reached through the two effect channels. */
typedef struct SceneSpinRayChannel {
    s32 reserved[2];
    char *pool; /* 0x08 */
} SceneSpinRayChannel;

/* Frame counter read as a one-field record. */
typedef struct SceneSpinRayFrame {
    u16 count;
} SceneSpinRayFrame;

extern RenderColor D_8018F224;
extern RenderColor D_8018F228;
extern RenderColor D_8018F22C;
extern GteRotation D_8018F210;
extern RenderColor D_8018F230;
extern u8 D_8019B3E4[];
extern u8 D_8019B404[];
extern GteShortVector D_8019B670;
extern u16 D_800E11EA;
extern SceneSpinRayChannel *D_800F32D0, *D_800F33E0;
extern SceneSpinRayFrame D_800942EC;

int func_80077CF4(int angle);
int func_80077DC4(int angle);
u16 func_80077AA4(int x, int y);
int func_80071A54(void);
int func_800D3FD8(void);
int func_800D3F64(int sound, int handle);
int func_800CE560(void *pool, int size, int count, void *callback);
SceneSpinRay *func_800CE610(void *pool);
void func_800CE8F0(void *pool, int index, GteRotation *rotation, GteShortVector *position);
void func_800CFFAC(SceneSpinRay *ray);
int func_80196748(int mode, SceneSpinRay *ray);

#endif
