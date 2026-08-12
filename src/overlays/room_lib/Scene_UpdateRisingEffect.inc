#include "../room_lib/room_lib.h"
#include "pe1/overlay_math.h"

typedef struct SceneRisingScript {
    u8 state;
    u8 result;
    s16 frame;
} SceneRisingScript;

typedef struct SceneRisingEffect {
    unsigned char pad00[0x120];
    union {
        u16 unsignedValue;
        s16 signedValue;
    } horizontal;
    s16 vertical;
    union {
        u16 unsignedValue;
        s16 signedValue;
    } speed;
} SceneRisingEffect;

typedef struct SceneRisingActor {
    unsigned char pad00[0x18];
    s32 x;
    unsigned char pad1C[4];
    s32 z;
    unsigned char pad24[8];
    s16 collisionState;
    s16 collisionRadius;
} SceneRisingActor;

void SCENE_UPDATE_RISING_EFFECT_NAME(
    int unused, SceneRisingScript *script, SceneRisingEffect *effect) {
    s16 position[3];
    SceneRisingActor *actor;
    s16 value;

    actor = Overlay_GetScriptActor();
    if (script->frame >= 9) {
        value = effect->vertical;
        if (value >= 0x21) {
            effect->vertical = value - 0x20;
        }
    }

    effect->horizontal.signedValue = effect->horizontal.unsignedValue +
        (effect->speed.signedValue >> 4);
    effect->speed.signedValue = effect->speed.unsignedValue + 0x320;
    if (script->frame == 0x10) {
        script->result = 2;
    }

    position[0] = actor->x;
    position[1] = D_800942EC;
    position[2] = actor->z;
    if (func_800C6B90(position, actor->collisionRadius) != 0) {
        actor->collisionState = 1;
    }
}
