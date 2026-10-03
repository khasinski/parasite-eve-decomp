#ifndef PE1_SCENE_GRAB_TRIGGER_H
#define PE1_SCENE_GRAB_TRIGGER_H

#include "common.h"
#include "pe1/field_actor.h"

/* Scene script object that pulls the player into a scripted grab animation
 * when the player walks up to the owning actor during its reach window. */
typedef struct SceneGrabControl {
    /* 0x0C */ void (*callback)();
    /* 0x10 */ int *signal;
    /* 0x14 */ u8 pad_14[8];
    /* 0x1C */ RenderMatrix player_matrix;
    /* 0x3C */ int saved_x;
    /* 0x40 */ int saved_z;
    /* 0x44 */ u8 grabbed;
} SceneGrabControl;

typedef struct SceneGrabPoint {
    s32 x;
    s32 y;
    s32 z;
} SceneGrabPoint;

typedef struct SceneGrabTrigger {
    /* 0x00 */ u8 pad_00[8];
    /* 0x08 */ FieldActor *actor;
    /* 0x0C */ SceneGrabControl control;
} SceneGrabTrigger;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneGrabTrigger, control.signal) == 0x10,
                  scene_grab_signal_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneGrabTrigger, control.player_matrix) == 0x1C,
                  scene_grab_matrix_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneGrabTrigger, control.grabbed) == 0x44,
                  scene_grab_flag_offset);

extern FieldActor *g_PlayerEntity;
int func_800DFE20(s32 *from, s32 *to);
void func_80020C74(void);
s32 func_80192D04(char *obj);
void func_80192878(SceneGrabTrigger *trigger);
void RoomLib_RunAfterFrame37_80192BC0();

#endif
