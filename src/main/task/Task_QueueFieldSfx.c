/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/field_sfx.h"

FieldSfxU8 g_FieldSfxQueueCount;

void Task_QueueFieldSfx(int arg0, int arg1, int arg2, int arg3, int arg4) {
    FieldSfxQueueEntry *entry = &g_FieldSfxQueue[g_FieldSfxQueueCount];

    entry->field_3 = arg0;
    entry->field_2 = arg1;
    entry->field_8 = arg3;
    entry->field_0 = arg2;
    entry->field_4 = arg4;
    g_FieldSfxQueueCount++;
}
