#ifndef PE1_SCENE_E19_2_SPIN_RAY_H
#define PE1_SCENE_E19_2_SPIN_RAY_H

#include "pe1/room_spark.h"
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

extern RenderColor D_8018F224;
extern RenderColor D_8018F228;
extern RenderColor D_8018F22C;
extern u8 D_8019B3E4[];
extern GteShortVector D_8019B670;

#endif
