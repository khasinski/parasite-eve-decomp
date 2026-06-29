#include "pe1/geom_state.h"

int Sys_SetFlagSlot(int arg0, int arg1) {
    GeomCtrlEntry *slot;

    slot = (GeomCtrlEntry *)((char *)g_GeomState + g_GeomState->ctrl_offset) + arg0;
    if (arg1 != 0) {
        slot->head.b.flags |= 6;
    } else {
        slot->head.b.flags &= 0xF9;
    }

    return 0;
}
