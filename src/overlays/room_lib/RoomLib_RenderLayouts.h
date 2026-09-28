#ifndef ROOM_LIB_RENDER_LAYOUTS_H
#define ROOM_LIB_RENDER_LAYOUTS_H

#include "pe1/room_fx.h"

/* Shared field-engine view used by the room orbit particle initializers. */
typedef struct RoomLibOrbitView {
    int pad0;
    unsigned char transform[0x14];
    int baseX;
    int baseY;
    int baseZ;
} RoomLibOrbitView;

/* Scratch layout shared by the paired-sprite and sixteen-point transforms. */
typedef struct RoomLibSpriteTransformStack {
    RoomFxSeed8 seed;
    RoomSpriteMatrix scaleMatrix;
    RoomSpriteMatrix matrix;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
} RoomLibSpriteTransformStack;

PE1_STATIC_ASSERT(sizeof(RoomLibOrbitView) == 0x24,
                  room_lib_orbit_view_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomLibOrbitView, transform) == 0x04,
                  room_lib_orbit_view_transform_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomLibOrbitView, baseX) == 0x18,
                  room_lib_orbit_view_position_offset);
PE1_STATIC_ASSERT(sizeof(RoomLibSpriteTransformStack) == 0x68,
                  room_lib_sprite_transform_stack_size);

#endif
