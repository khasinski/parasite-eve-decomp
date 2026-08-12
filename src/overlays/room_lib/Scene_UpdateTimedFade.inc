#include "common.h"

typedef struct SceneTimedFadeScript {
    u8 state;
    u8 result;
    union {
        u16 unsignedValue;
        s16 signedValue;
    } frame;
} SceneTimedFadeScript;

typedef struct SceneTimedFade {
    unsigned char pad00[0x10];
    s16 horizontal;
    s16 vertical;
} SceneTimedFade;

void SCENE_UPDATE_TIMED_FADE_NAME(
    int unused, SceneTimedFadeScript *script, SceneTimedFade *fade) {
    u16 frame;
    SceneTimedFade *current = fade;

    frame = script->frame.unsignedValue;
    if ((u16)(frame - 1) < 7) {
        fade->vertical += 0x10;
    }
    frame = script->frame.unsignedValue;
    if ((u16)(frame - 0x15) < 0x1D) {
        fade->horizontal += 0x28;
    }
    if (script->frame.signedValue >= 0x33 && current->vertical >= 0x11) {
        current->vertical -= 0x10;
    }
    if (script->frame.signedValue == 0x3A) {
        script->result = 2;
    }
}
