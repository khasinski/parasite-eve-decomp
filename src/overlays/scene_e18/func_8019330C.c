#include "pe1/field_anim.h"
#include "pe1/field_actor.h"

typedef struct SceneE18PulseState {
    s16 stage;
    u16 ticks;
} SceneE18PulseState;

typedef struct SceneE18PulseParticle {
    s16 scale;
    s16 duration;
    u8 reserved04[2];
    u8 active;
    u8 final_stage;
} SceneE18PulseParticle;

extern int func_80193018(int mode, void *state);
extern int func_8006DCE4(int sound, int owner_value, int x, int y, int z);

int func_8019330C(int mode, SceneE18PulseState *state) {
    SceneE18PulseParticle *particle;
    register int stage asm("$5");

    if (mode == 1) goto update;
    if (mode < 2) {
        if (mode == 0) goto setup;
        goto done;
    }
    goto done;

setup:
    state->stage = 0;
    state->ticks = 0;
    return func_800CE560(D_800F33E0->end, 8, 4, func_80193018);

update:
    if (state->stage >= 4) return 2;
    state->ticks++;
    if ((s16)state->ticks < 2) return 0;

    particle = func_800CE610(D_800F33E0->end);
    if (!particle) return 0;

    if (state->stage == 0) {
        FieldActor *actor = D_800F32D0->actor;
        RenderMatrix *matrix = actor->render_object.matrices;
        FieldActorState *owner = actor->state;
        func_8006DCE4(0x5C3, owner->progress,
                       (s16)matrix->translation[0],
                       (s16)matrix->translation[1],
                       (s16)matrix->translation[2]);
    }

    *(volatile u16 *)&state->ticks = 0;
    *(volatile s16 *)&state->stage = state->stage + 1;
    stage = state->stage;

    if (stage == 4) {
        particle->scale = 0x100;
        particle->duration = 8;
        particle->active = 1;
        particle->final_stage = 1;
    } else {
        register int scaled asm("$2") = stage << 5;
        particle->scale = scaled + 64;
        particle->duration = 8;
        particle->active = 0;
        particle->final_stage = 0;
    }

done:
    return 0;
}
