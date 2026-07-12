#include "pe1/geom_state.h"

extern GeomState * volatile D_800B1624 __asm__("D_800B1624");

int Obj_SetEntrySlotValue(int index, int slot, u8 value) {
    GeomCtrlEntry *entry = (GeomCtrlEntry *)((u8 *)D_800B1624 + D_800B1624->ctrl_offset + (index << 4));
    register u8 *ptr __asm__("$5");
    u8 *base;
    int ret;

    base = (u8 *)entry + entry->slot_offset;
    ptr = (u8 *)((slot << 1) + (int)base);
    ret = 0;
    ptr[1] = value;
    return ret;
}

int Obj_SetEntryFlags(int index, int bits) {
    GeomCtrlEntry *entry = (GeomCtrlEntry *)((u8 *)D_800B1624 + D_800B1624->ctrl_offset + (index << 4));
    entry->head.b.flags |= bits & 0x30;
    return 0;
}
