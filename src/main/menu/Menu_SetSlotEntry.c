/* CC1_FLAGS: -G0 -g3 */
/* MASPSX_FLAGS: -G0 --use-comm-section */

#include "pe1/geom_state.h"

/* The do { } while (0) blocks are load-bearing: their loop notes fence the
 * cc1 scheduler so the flag load/stores keep the retail order (permuter zero). */
int Menu_SetSlotEntry(int index, unsigned int limit, unsigned int slot)
{
    GeomCtrlEntry *entry;
    u32 field4;
    register u32 flags;
    u32 hi;
    u8 *slotbase;
    s32 offset;
    int neg;
    s16 f8;

    entry = (GeomCtrlEntry *)(((u8 *)g_GeomState + g_GeomState->ctrl_offset) + (index << 4));
    field4 = *(u32 *)&entry->field4;
    field4 &= 0xFF;
    offset = entry->slot_offset;
    hi = limit << 16;
    do {
        entry->fieldA = 0;
        field4 |= hi;
        do {
            flags = entry->head.b.flags;
            slotbase = (u8 *)entry + offset;
            *(u32 *)&entry->field4 = field4;
        } while (0);
        neg = -1;
        flags |= 6;
        entry->head.b.flags = flags;
        slotbase[(slot << 1) + 1] = neg;
        f8 = entry->field8;
        if ((f8 > 0 && slot < limit) || (f8 < 0 && limit < slot)) {
            entry->field8 = -(u16)entry->field8;
        }
        return 0;
    } while (0);
}
