/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_sfx.h"

FieldSfxU8 g_FieldSfxQueueCount;

void Task_ClearSfxTable(void) {
    unsigned int i;
    unsigned int j;

    for (i = 0; i < 28; i++) {
        for (j = 0; j < 3; j++) {
            ((int *)&g_FieldSfxQueue[i])[j] = 0;
        }
    }
    g_FieldSfxQueueCount = 0;
}

void Task_QueueFieldSfx(int arg0, int arg1, int arg2, int arg3, int arg4) {
    FieldSfxQueueEntry *entry = &g_FieldSfxQueue[g_FieldSfxQueueCount];

    entry->taskArgument = arg0;
    entry->subId = arg1;
    entry->taskValue = arg3;
    entry->typeId = arg2;
    entry->actorSelector.word = arg4;
    g_FieldSfxQueueCount++;
}
