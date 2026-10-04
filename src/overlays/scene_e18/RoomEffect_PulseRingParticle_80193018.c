#include "pe1/scene_e18_effects.h"

/* Pulse ring particle: a pair of light rings around the room model that
 * shrink away. While it lives it advances the model's stage byte; the last
 * ring releases the actor once the player is within 3/8 of its radius. */
int func_80193018(int mode, SceneE18PulseRing *ring) {
    GteShortVector position;
    GteRotation spin;
    SceneE18Instance *instance;
    int angle;
    int brightness;

    switch (mode) {
    case 1:
        instance = D_800F32D0->instance;
        if (*instance->owner->stage == 1)
            *instance->owner->stage = 2;
        ring->size -= ring->shrink;
        if (ring->size < 8) {
            if (ring->active)
                *instance->owner->stage = 4;
            return 1;
        }
        angle = (D_800E27EC + 1) * 24;
        if (angle > 0x400)
            angle = 0x400;
        ring->radius = func_80077CF4(angle);
        if (ring->last) {
            int dx = (RoomMain_ActorPtr->x - instance->x) >> 16;
            int dz = (RoomMain_ActorPtr->z - instance->z) >> 16;
            if (func_8005186C(dx * dx + dz * dz) < (ring->radius * 3) >> 3) {
                ring->last = 0;
                RoomMain_ActorPtr->owner->status |= 0x4000;
                instance->owner->flags |= 0x80000000;
            }
            if (ring->size < 0x40)
                ring->last = 0;
        }
        break;
    case 2:
        instance = D_800F32D0->instance;
        brightness = (s16)(ring->size > 0x80 ? 0x80 : ring->size);
        if (ring->size > 0) {
            position.x = instance->transform.t[0];
            position.y = instance->transform.t[1] - 0x140;
            position.z = instance->transform.t[2];
            spin.x = 0x400;
            spin.y = D_800E27EC << 7;
            spin.z = 0;
            spin.flags = 1;
            func_800D0728(&position, 0x500, 0x600, 0x10, &spin, ring->radius, ring->radius,
                          &D_8019411C[0], &D_8019411C[1], brightness, 1);
            func_800D0728(&position, 0x600, 0x680, 0x10, &spin, ring->radius, ring->radius,
                          &D_8019411C[1], &D_8019411C[0], brightness, 1);
        }
        break;
    }
    return 0;
}
