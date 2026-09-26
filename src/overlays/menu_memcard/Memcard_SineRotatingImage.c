#include "menu_memcard_pulse.h"

s32 Memcard_SineRotatingImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    s32 angle, scale;
    register s32 palette;
    register s32 intensity;
    register s32 cosine asm("$5");
    register s32 index;
    register s32 four asm("$3");
    s16 timer;
    register s32 twist;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 12) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = (state->timer << 10) / 12;
        scale = func_80077CF4(angle) + 4096;
        cosine = func_80077DC4(angle);
        if (cosine < 0) cosine += 31;
        rotation.x = state->rotation.x;
        index = D_800F336C;
        rotation.y = state->rotation.y;
        rotation.z = state->rotation.z;
        intensity = cosine >> 5;
        __asm__("" : : "r"(intensity) : "memory");
        twist = state->timer;
        rotation.flags = 1;
        rotation.z = twist * 50;
        {
            four = 4;
            palette = D_800E1204[index];
        }
        if (index == four && D_800F3428 != 0) palette += 7;
        else palette += 3;
        func_800CEE20(&state->position, &rotation, scale, scale, 68,
                     func_80077AA4(0, palette), 1, intensity, &D_801F1F34);
        break;
    }
    return 0;
}
