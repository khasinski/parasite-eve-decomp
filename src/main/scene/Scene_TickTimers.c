#include "pe1/game_timers.h"

extern u32 g_GameStateFlags;
extern GameTimerEntry D_800A76A0[4];

int Gpu_CheckDrawStatus(void);

void Scene_TickTimers(void) {
    register GameTimerEntry *entry asm("$5");
    int *current_p;
    u32 *flags_p;
    int *limit_p;
    unsigned int i;
    u32 flags;
    int direct;
    int current;

    if ((g_GameStateFlags & 0x41) != 0) {
        return;
    }
    if ((Gpu_CheckDrawStatus() << 24) != 0) {
        return;
    }

    i = 0;
    flags_p = &D_800A76A0[0].flags;
    entry = (GameTimerEntry *)flags_p;
    limit_p = &entry->limit;
    current_p = &entry->current;

    do {
        flags = *flags_p;
        if ((flags & 1) != 0) {
            if ((flags & 4) == 0) {
                if ((flags & 2) != 0) {
                    if ((flags & 5) != 0) {
                        direct = *current_p;
                        entry->current = direct - 1;
                    } else {
                        current = *current_p;
                        if ((unsigned int)*limit_p < (unsigned int)current) {
                            direct = current - 1;
                            entry->current = direct;
                        } else {
                            direct = flags | 4;
                            *flags_p = direct;
                        }
                    }
                } else {
                    if ((flags & 5) != 0) {
                        direct = *current_p;
                        entry->current = direct + 1;
                    } else {
                        current = *current_p;
                        if ((unsigned int)current < (unsigned int)*limit_p) {
                            /* Flag tests are finished; reuse the temporary for the count. */
                            flags = current;
                            direct = flags + 1;
                            entry->current = direct;
                        } else {
                            direct = flags | 4;
                            *flags_p = direct;
                        }
                    }
                }
            }
        }
        flags_p += sizeof(GameTimerEntry) / sizeof(*flags_p);
        entry++;
        limit_p += sizeof(GameTimerEntry) / sizeof(*limit_p);
        i++;
        current_p += sizeof(GameTimerEntry) / sizeof(*current_p);
    } while (i < 4);
}
