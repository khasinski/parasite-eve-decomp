#ifndef PE1_SCENE_GRAB_HEAD_H
#define PE1_SCENE_GRAB_HEAD_H

#include "common.h"

struct FieldActor;

/* Head of the scripted grab triggers of room_m273 and scene_e19: the actor
 * that reaches for the player and the trigger's step callback. Each scene's
 * trigger continues with its own grab state (RoomSelectionState in
 * room_m273, SceneGrabTrigger in scene_e19). */
typedef struct SceneGrabHead {
    /* 0x00 */ u8 pad_00[8];
    /* 0x08 */ struct FieldActor *actor;
    /* 0x0C */ void (*callback)();
} SceneGrabHead;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneGrabHead, callback) == 0x0C,
                  scene_grab_head_callback_offset);

/* Step that waits for the actor's attack stance
 * (src/overlays/room_lib/SceneEffect_GrabAwaitStance.c), then hands over to
 * the scene's own approach check. */
void SceneEffect_GrabAwaitStance(SceneGrabHead *head);
void SceneEffect_GrabOnApproach(SceneGrabHead *head);

#endif
