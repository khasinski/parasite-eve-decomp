#include "pe1/geom_state.h"

int Obj_SetEntrySlotValue(int index, int slot, u8 value) {
    GeomCtrlEntry *entry = (GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4));
    u8 *ptr;
    u8 *base;
    base = (u8 *)entry + entry->slot_offset;
    ptr = base + (slot << 1);
    ptr[1] = value;
    return 0;
}

int Obj_SetEntryFlags(int index, int bits) {
    GeomCtrlEntry *entry = (GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4));
    entry->head.b.flags |= bits & 0x30;
    return 0;
}
