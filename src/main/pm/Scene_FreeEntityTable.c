#include "pe1/game_state.h"
#include "pe1/pm.h"

static inline void clearSlot(int slot)
{
    PmSlotHeader *entry;
    unsigned int i;
    if ((unsigned int)slot < 22) {
        if ((unsigned int)slot >= 11) {
            /* Signed byte offset preserves the retail loop strength reduction. */
            entry = (PmSlotHeader *)((u8 *)g_PmSlotTable2 +
                (slot - 11) * (int)sizeof(PmSecondarySlot));
        } else {
            entry = &g_PmSlotTable[slot].header;
        }
        if (entry->command == 0x72) {
            for (i = 0x6C; i < 0x73; ++i)
                g_PmSlotBuffer[i] = 0;
            g_GameState.flags &= ~0x10000;
        }
        entry->state = 0;
        entry->command = 0xFF;
        entry->field02 = 0xFF;
        entry->field03 = 0xFF;
        entry->ticks = 0;
        entry->owner = 0;
    }
}
int Scene_FreeEntityTable(void *owner)
{
    int result;
    int i;
    if (!owner)
        return -25;
    result = 0;
    for (i = 0; i < 11; ++i) {
        PmSlotHeader *entry = &g_PmSlotTable[i].header;
        if (entry->owner == owner) {
            result = Pm_Stop(i, owner, 1);
            if (result)
                return result;
            clearSlot(i);
        }
    }
    for (i = 0; i < 11; ++i) {
        PmSlotHeader *entry = &g_PmSlotTable2[i].header;
        if (entry->owner == owner) {
            result = Pm_Stop(i + 11, owner, 1);
            if (result)
                return result;
            clearSlot(i + 11);
        }
    }
    return result;
}
