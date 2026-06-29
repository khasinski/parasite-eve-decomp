
#include "pe1/geom_state.h"

int Obj_SetEntryFlags(int index, int bits) {
    GeomCtrlEntry *entry = (GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4));
    entry->head.b.flags |= bits & 0x30;
    return 0;
}
