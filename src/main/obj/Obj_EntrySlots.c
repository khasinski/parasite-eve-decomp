#include "pe1/geom_state.h"

/* The control table lives at a byte offset from the geometry header; retail
 * reads the header pointer twice and adds the offsets to its address as
 * integers. */
typedef union GeomCtrlAddress {
    GeomState *state;
    GeomCtrlEntry *entry;
    u8 *slots;
    u32 word;
} GeomCtrlAddress;

#define GEOM_CTRL_ENTRY(address, base, index) \
    ((address).state = g_GeomState, \
     (base).state = g_GeomState, \
     (address).word = (base).word + (address).state->ctrl_offset + ((index) << 4), \
     (address).entry)

s16 Obj_GetEntryField6(int index) {
    GeomCtrlAddress address, base;

    return GEOM_CTRL_ENTRY(address, base, index)->field6;
}

int Obj_SetEntryField8(int index, unsigned int value) {
    GeomCtrlAddress address, base;

    GEOM_CTRL_ENTRY(address, base, index)->field8 = value >> 8;
    return 0;
}

int Obj_FillEntrySlotValues(int index, u8 value)
{
  GeomCtrlAddress address, base;
  GeomCtrlEntry *entry;
  u8 *ptr;
  int count;
  int i;
  int framePad[2];
  i = 0;
  entry = GEOM_CTRL_ENTRY(address, base, index);
  address.word += entry->slot_offset;
  count = entry->head.packed >> 8;
  if (count != 0)
  {
    ptr = address.slots;
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

int Obj_SetEntrySlotValue(int index, int slot, u8 value) {
    GeomCtrlAddress address, base;
    GeomCtrlEntry *entry;
    int ret;

    entry = GEOM_CTRL_ENTRY(address, base, index);
    address.word += entry->slot_offset;
    ret = 0;
    address.slots[slot * 2 + 1] = value;
    return ret;
}

int Obj_SetEntryFlags(int index, int bits) {
    GeomCtrlAddress address, base;

    GEOM_CTRL_ENTRY(address, base, index)->head.b.flags |= bits & 0x30;
    return 0;
}
