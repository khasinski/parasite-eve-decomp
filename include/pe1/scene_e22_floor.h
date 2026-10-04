#ifndef PE1_SCENE_E22_FLOOR_H
#define PE1_SCENE_E22_FLOOR_H

#include "common.h"

/* The same halfword is called y by ember controllers and count by ring and
 * glow callbacks. */
typedef union SceneE22FloorHeight {
    s16 y;
    s16 count;
} SceneE22FloorHeight;

PE1_STATIC_ASSERT(sizeof(SceneE22FloorHeight) == 2,
                  scene_e22_floor_height_size);

#endif
