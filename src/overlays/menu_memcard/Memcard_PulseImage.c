#include "menu_memcard_pulse.h"

s32 Memcard_PulseImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    RenderColor color;
    s32 angle;
    s32 scale;
    s32 palette;
    register s16 timer;
    register s32 twist;
    register void *track;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 16) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = state->timer << 6;
        scale = func_80077CF4(angle) + 2048;
        func_80077DC4(angle);
        rotation.x = 0; rotation.y = 0; twist = state->timer;
        __asm__("" : "=r"(twist) : "0"(twist) : "$4", "memory");
        track = D_801F1D5C;
        __asm__("" : "=r"(track) : "0"(track) : "memory");
        rotation.flags = 0;
        __asm__("" : "=r"(twist) : "0"(twist) : "memory");
        rotation.z = twist << 5;
        func_800CF3AC(track, &color, state->timer << 1);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        palette = D_800E1204[D_800F336C];
        if (D_800F336C == 4 && D_800F3428 != 0) palette += 4;
        func_800CEE20(&state->position, &rotation, scale, scale, 130,
                     func_80077AA4(48, palette), 1, 128, &color);
        break;
    }
    return 0;
}
