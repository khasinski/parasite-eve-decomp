
#include "pe1/geom_state.h"

int Obj_FillEntrySlotValues(int index, u8 value) {
    GeomCtrlEntry *entry;
    u8 *ptr;
    register u8 *base asm("$2");
    int count;
    int i;
    int framePad[2];

    i = 0;
    entry = (GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4));
    count = entry->head.packed >> 8;
    base = (u8 *)entry + entry->slot_offset;
    if (count != 0) {
        ptr = base;
        do {
            ptr[1] = value;
            i++;
            ptr += 2;
        } while (i < count);
    }

    return 0;
}
