#ifndef SCENE_E19_SHARED_H
#define SCENE_E19_SHARED_H

#include "common.h"

typedef struct SceneE19ActorChild {
    u8 reserved_00[0x0C];
    s16 h0C;
} SceneE19ActorChild;

typedef struct SceneE19Actor {
    SceneE19ActorChild *child;
    u8 reserved_04[0x24];
    s32 pos[3];
    u8 reserved_34[0x0C];
    s32 base_pos[3];
    u8 reserved_4C[0x1C];
    s32 vel[3];
    u8 reserved_74[4];
    s32 move[3];
    u8 reserved_84[0x14];
    u32 flags;
    u8 reserved_9C[0x13C];
    s32 field_1D8;
    u16 field_1DC;
    u8 reserved_1DE[0x72];
    u16 h250;
} SceneE19Actor;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Actor, child) == 0, scene_e19_actor_child_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Actor, pos) == 0x28, scene_e19_actor_pos_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Actor, base_pos) == 0x40, scene_e19_actor_base_pos_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Actor, vel) == 0x68, scene_e19_actor_vel_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Actor, move) == 0x78, scene_e19_actor_move_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Actor, flags) == 0x98, scene_e19_actor_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Actor, field_1D8) == 0x1D8, scene_e19_actor_field_1d8_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19Actor, h250) == 0x250, scene_e19_actor_h250_offset);
PE1_STATIC_ASSERT(sizeof(SceneE19Actor) == 0x254, scene_e19_actor_size);

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19ActorChild, h0C) == 0x0C, scene_e19_actor_child_value_offset);
PE1_STATIC_ASSERT(sizeof(SceneE19ActorChild) == 0x0E, scene_e19_actor_child_size);

typedef struct SceneE19ResetTarget {
    u8 state;
    u8 reserved_01[0x0F];
    int *signal;
    u8 reserved_14[0x30];
    u8 enabled;
} SceneE19ResetTarget;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19ResetTarget, signal) == 0x10, scene_e19_reset_target_signal_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19ResetTarget, enabled) == 0x44, scene_e19_reset_target_enabled_offset);
PE1_STATIC_ASSERT(sizeof(SceneE19ResetTarget) == 0x48, scene_e19_reset_target_size);

#endif
