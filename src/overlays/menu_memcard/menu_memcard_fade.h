#ifndef MENU_MEMCARD_FADE_H
#define MENU_MEMCARD_FADE_H

#include "common.h"

typedef struct MenuMemcardFadeState {
    u8 reserved00[0x06];
    s16 displayTop;
    u8 reserved08[0x02];
    s16 displayBottom;
    s32 phase;
    u8 reserved10[0x04];
    void (*onStep)(struct MenuMemcardFadeState *);
    u8 reserved18[0x04];
    s32 level;
    s32 velocity;
    s32 increment;
    u8 reserved28[0x08];
    s32 finished;
} MenuMemcardFadeState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardFadeState, level) == 0x1C,
                  menu_memcard_fade_level_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardFadeState, displayTop) == 0x06,
                  menu_memcard_fade_display_top_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardFadeState, displayBottom) == 0x0A,
                  menu_memcard_fade_display_bottom_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardFadeState, phase) == 0x0C,
                  menu_memcard_fade_phase_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardFadeState, onStep) == 0x14,
                  menu_memcard_fade_on_step_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardFadeState, velocity) == 0x20,
                  menu_memcard_fade_velocity_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardFadeState, increment) == 0x24,
                  menu_memcard_fade_increment_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(MenuMemcardFadeState, finished) == 0x30,
                  menu_memcard_fade_finished_offset);

#endif
