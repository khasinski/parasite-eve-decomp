#include "pe1/geom_state.h"

int Sys_SetFlagSlot(int arg0, int arg1) {
    GeomStateAddress table, base;
    GeomCtrlEntry *slot;

    GEOM_STATE_OFFSET(table, base, ctrl_offset, 0);
    slot = table.ctrl + arg0;
    if (arg1 != 0) {
        slot->head.b.flags |= 6;
    } else {
        slot->head.b.flags &= 0xF9;
    }

    return 0;
}
