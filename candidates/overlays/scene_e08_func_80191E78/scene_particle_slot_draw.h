#ifndef PE1_SCENE_PARTICLE_SLOT_DRAW_H
#define PE1_SCENE_PARTICLE_SLOT_DRAW_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"

/* Scene e08 particle slots draw: four textured sprites at the slot offsets
 * around the scene anchor, then their four shadows on the floor. */
extern int D_8019956C;
extern int D_8019957C;
extern GteShortVector D_8018F040;
extern RenderColor D_8018F048;
/* Texture page index slot at 0x800E11EA, read as a one-field record (an
 * in-struct read keeps it below the stack color copy). */
typedef struct SceneParticlePageIndex {
    u16 index;
} SceneParticlePageIndex;
extern SceneParticlePageIndex D_800E11EA;

void func_80071A44(void *data, int value, int size);
int func_80077AA4(int x, int y);

#endif
