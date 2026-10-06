#include "pe1/geom_state.h"

int Obj_SetEntryLimit(int arg0, int arg1)
{
  GeomCtrlEntry *entry;
  u32 field4;
  register u32 flags;
  u32 hi;
  int ret;
  entry = (GeomCtrlEntry *) ((((u8 *) g_GeomState) + g_GeomState->ctrl_offset) + (arg0 << 4));
  field4 = *((u32 *) (&entry->field4));
  field4 &= 0xFF;
  ret = entry->head.b.flags;
  flags = ret;
  hi = arg1 << 16;
  entry->fieldA = 0;
  field4 |= hi;
  flags |= 2;
  entry->head.b.flags = flags;
  ret = 0;
  *((u32 *) (&entry->field4)) = field4;
  return ret;
}
