#include "menu_memcard_fade.h"

void Memcard_UpdateFadeDisplay(MenuMemcardFadeState *state) {
    {
        s32 velocity = state->velocity;
        s32 increase = 0;
        s32 level;

        if (velocity != 0) {
            increase = velocity < 0x54;
        }
        level = state->level;
        velocity += increase;
        *(volatile s32 *)&state->velocity = velocity;
        if (level < 0x100) {
            level += state->increment << 3;
        }
        state->level = level;
        if (level == 0 || level == 0x100) {
            state->increment = 0;
        }
    }
    {
        s32 current = *(volatile s32 *)&state->velocity;
        s32 top = 0x3C - current;

        if (top >= 0) {
            top = current + 0x50;
        } else {
            top = 0x8C;
        }
        *(volatile s16 *)&state->displayTop = top;
    }
    {
        s32 current = *(volatile s32 *)&state->velocity;
        s32 delta = 0x54 - current;
        s32 bottom;

        /* Keep the floor constant in the branch delay slot. */
        asm volatile("");
        if (delta >= 0x18) {
            bottom = delta;
        } else {
            bottom = 0x18;
        }
        state->displayBottom = bottom;
    }
}
