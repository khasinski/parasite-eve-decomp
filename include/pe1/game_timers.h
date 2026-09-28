#ifndef PE1_GAME_TIMERS_H
#define PE1_GAME_TIMERS_H

#include "common.h"

/* Three-word timer records beginning at D_800A76A0. */
typedef struct GameTimerEntry {
    /* 0x00 */ u32 flags;
    /* 0x04 */ int current;
    /* 0x08 */ int limit;
} GameTimerEntry;

PE1_STATIC_ASSERT(sizeof(GameTimerEntry) == 0x0C, game_timer_entry_size);

#endif
