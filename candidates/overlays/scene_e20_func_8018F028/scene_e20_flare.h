#ifndef PE1_SCENE_E20_FLARE_H
#define PE1_SCENE_E20_FLARE_H

#include "pe1/render_object.h"
#include "pe1/gte.h"

/* Scene e20 flare particle: a position, a heading that doubles as the draw
 * rotation (a velocity while it falls), its state and its frame timer. */
typedef struct SceneE20Flare {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector heading;
    /* 0x10 */ s16 state;
    /* 0x12 */ s16 timer;
} SceneE20Flare;

/* Floor height read as a one-field record. */
typedef struct SceneE20Floor {
    s16 count;
} SceneE20Floor;

extern RenderColor D_8018EFF4;
extern RenderColor D_8018EFF8;
extern u8 D_80190804[];
/* Texture page indices: [0] is the shared effect page, [8] this scene's. */
extern u16 D_800E11EA[];
extern SceneE20Floor D_800942EC;

int func_80077CF4(int angle);
int func_80077DC4(int angle);
u16 func_80077AA4(int x, int y);
void func_800D2104(GteShortVector *position, RenderColor *color, int size, int alpha);

#endif
