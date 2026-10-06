#ifndef PE1_SCENE_E20_FLARE_DRAW_H
#define PE1_SCENE_E20_FLARE_DRAW_H

#include "pe1/scene_e20_flare.h"
#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/gte_window.h"

/* Declarations used only by the scene e20 flare particle callback. */
extern RenderColor D_8018EFF4;
extern RenderColor D_8018EFF8;
extern u8 D_80190804[];
/* Texture page indices: [0] is the shared effect page, [8] this scene's. */
extern u16 D_800E11EA[];

int rsin(int angle);
int rcos(int angle);
u16 func_80077AA4(int x, int y);
void func_800D2104(GteShortVector *position, RenderColor *color, int size, int alpha);

#endif
