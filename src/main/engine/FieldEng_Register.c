#include "common.h"
#include "pe1/field_engine_state.h"
int printf(char *fmt);
extern u8 *D_800F34F4;
extern char *D_800E2248;
extern char *D_800F3330;
extern char *D_800F33B0;
extern char D_800C20C8[];
int FieldEng_Register(char *obj, int (**handlers)(char *obj, void *entry, void *data))
{
  register char *obj_s0 = obj;
  register int (**handlers_s2)(char *obj, void *entry, void *data) = handlers;
  s16 i;
  char *table;
  i = 0;
  table = *((char **) (obj_s0 + 0x78));
  D_800E2248 = obj_s0 + 0xC;
  D_800F34F4 = (u8 *) (obj_s0 + 0x80);
  D_800F3330 = obj_s0 + 0x200;
  D_800F33B0 = table;
  for (; i < 0x40; i++)
  {
    FieldEngSlot *entry = (FieldEngSlot *) ((((s16) i) * 6) + ((int) D_800F34F4));
    if (entry->flag == 1)
    {
      int (*handler)(char *obj, void *entry, void *data) = handlers_s2[entry->handler_id];
      if (handler != ((void *) (-1)))
      {
        handler(obj, entry, D_800F3330 + entry->data_offset);
      }
      else
      {
        printf(D_800C20C8);
      }
    }
  }

  return 0;
}
