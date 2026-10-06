#include "common.h"
#include "pe1/field_engine_state.h"

/* Field engine objects: clearing an object's script state and dispatch
 * slots, and the two per-frame passes over the 0x40 dispatch slots. */

int FieldEng_GetStatus(char *obj);
int func_800C2DA0(u16 slot);
int printf(char *fmt);

extern char *D_800E2248;
extern u8 *D_800F34F4;
extern char *D_800F32A8;
extern char *D_800F3330;
extern char *D_800F33B0;
extern char D_800C20C8[];
extern char D_800C20EC[];

void **FieldEng_GetSlot(char *obj) {
    char *p;
    unsigned int i;
    int offset;

    p = obj + 0xC;
    do {
        *p++ = 0;
    } while (p < obj + 0xA0C);

    obj[2] = 0;
    obj[3] = 0;

    D_800E2248 = obj + 0xC;
    D_800F34F4 = (u8 *)(obj + 0x80);
    D_800F32A8 = obj;
    D_800F3330 = obj + 0x200;

    *(s16 *)(obj + 0xC) = 0;
    *(s16 *)(obj + 0xE) = 0;
    *(s16 *)(obj + 0x10) = 0;

    i = 0;
    offset = 0;
    do {
        *(u8 *)(offset + (int)D_800F34F4) = 0;
        *(u8 *)(offset + (int)D_800F34F4 + 1) = 0;
        *(s16 *)(offset + (int)D_800F34F4 + 2) = 0;
        *(s16 *)(offset + (int)D_800F34F4 + 4) = 0;
        i++;
        offset += 6;
    } while (i < 0x40);

    if (FieldEng_GetStatus(obj) == 3) {
        char *inner = **(char ***)(obj + 8);

        *(int *)inner = (*(int *)inner & 0xC0FFFFFF) | 0x01000000;
    }

    return (void **)(D_800E2248 + 0x6C);
}

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

int func_800C251C(char *obj, int (**handlers)(char *obj, void *entry, void *data)) {
    int result;
    s16 i;
    char *data;
    char *table;

    result = 0;
    table = *(char **)(obj + 0x78);
    data = *(char **)(obj + 8);

    D_800E2248 = obj + 0xC;
    D_800F34F4 = (u8 *)(obj + 0x80);
    D_800F32A8 = obj;
    D_800F3330 = obj + 0x200;
    D_800F33B0 = table;

    obj[3] = obj[0x12];

    if (FieldEng_GetStatus(obj) == 3) {
        if ((u8)obj[1] != 0x24) {
            char *state = *(char **)(**(char ***)(obj + 8) + 0x18);

            if ((u8)state[0] == 1) {
                state[0] = 2;
            }
        }
    }

    for (i = 0; i < 0x40; i++) {
        FieldEngSlot *entry = (FieldEngSlot *)(i * 6 + (int)D_800F34F4);

        if (entry->flag == 1) {
            int (*handler)(char *obj, void *entry, void *data) = handlers[entry->handler_id];

            if (handler != (void *)-1) {
                handler(obj, entry, D_800F3330 + entry->data_offset);
            } else {
                printf(D_800C20EC);
            }
            {
                FieldEngSlot *counter_entry = (FieldEngSlot *)(i * 6 + (int)D_800F34F4);

                counter_entry->counter += 1;
            }
        }

        {
            int status_offset = i * 6;
            u8 *status_base = D_800F34F4;

            if (((FieldEngSlot *)(status_offset + (int)status_base))->flag == 2) {
                result |= func_800C2DA0((u16)i);
            }
        }
    }

    if (FieldEng_GetStatus(obj) == 3) {
        char *inner = *(char **)data;
        u32 flags = *(u32 *)inner;

        if (*(u8 *)(inner + ((flags >> 17) & 0x70) + 0x1C) == 0 && (flags & 0x180E) != 0) {
            result = -1;
        }
    }

    return result;
}
