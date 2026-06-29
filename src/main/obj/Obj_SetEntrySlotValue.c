
#include "pe1/geom_state.h"

int Obj_SetEntrySlotValue(int index, int slot, u8 value) {
    GeomCtrlEntry *entry = (GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4));
    u8 *ptr;
    u8 *base;
    ptr = (u8 *)(slot << 1);
    base = (u8 *)entry + entry->slot_offset;
    asm volatile("addu\t$5,$5,$2" : : "r"(ptr), "r"(base));
    ptr[1] = value;
    return 0;
}
