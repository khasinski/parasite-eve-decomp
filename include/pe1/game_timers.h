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

/* Interior symbol naming timer zero's current word. Counter words follow
 * the three-word stride of GameTimerEntry, rather than a packed int array. */
extern int g_GameTimeTable;
extern int g_PlayTimeSeconds;
#define GAME_TIME_COUNTER(index) \
    (((GameTimerEntry *)((char *)&g_GameTimeTable - \
        PE1_OFFSETOF(GameTimerEntry, current)))[index].current)

#endif
