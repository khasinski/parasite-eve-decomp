/*
 * First step of the scripted grab trigger of room_m273 and scene_e19: waits
 * until the reaching actor enters its attack stance, then hands over to the
 * scene's own approach check (SceneEffect_GrabOnApproach). Both overlays
 * link it right after RoomLib_UpdateCallback; the grab class of room_m269
 * and scene_e01 has the same step as RoomFx_GrabAwaitTarget.
 */
#include "pe1/field_actor.h"
#include "pe1/scene_grab_head.h"

void SceneEffect_GrabAwaitStance(SceneGrabHead *head) {
    if (head->actor->mode == 0x10) {
        head->callback = SceneEffect_GrabOnApproach;
    }
}
