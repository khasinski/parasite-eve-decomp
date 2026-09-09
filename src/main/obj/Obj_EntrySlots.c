#include "pe1/geom_state.h"

s16 Obj_GetEntryField6(int index) {
    return ((GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4)))->field6;
}

int Obj_SetEntryField8(int index, unsigned int value) {
    ((GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4)))->field8 = value >> 8;
    return 0;
}
#include "pe1/geom_state.h"

int Obj_FillEntrySlotValues(int index, u8 value)
{
  u8 *new_var;
  GeomCtrlEntry *entry;
  u8 *ptr;
  register u8 *base;
  int count;
  int i;
  int framePad[2];
  i = 0;
  entry = (GeomCtrlEntry *) ((((u8 *) g_GeomState) + g_GeomState->ctrl_offset) + (index << 4));
  new_var = ((u8 *) entry) + entry->slot_offset;
  count = entry->head.packed >> 8;
  base = new_var;
  if (count != 0)
  {
    ptr = base;
    do
    {
      ptr[1] = value;
      i++;
      ptr += 2;
    }
    while (i < count);
  }
  return 0;
}
#include "pe1/geom_state.h"


int Obj_SetEntrySlotValue(int index, int slot, u8 value) {
    GeomCtrlEntry *entry = (GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4));
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
    GeomCtrlEntry *entry = (GeomCtrlEntry *)((u8 *)g_GeomState + g_GeomState->ctrl_offset + (index << 4));
    entry->head.b.flags |= bits & 0x30;
    return 0;
}
