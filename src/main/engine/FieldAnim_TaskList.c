#include "pe1/field_anim.h"

extern int D_800B0E18;
extern int D_800B0E1C;
extern int D_800B1630;

int func_800CE4F8(int index) {
    int ret;

    if (index == 1) {
        goto ret1;
    }
    if (index < 2) {
        if (index == 0) {
            goto ret0;
        }
        ret = 0;
        goto done;
    }
    if (index == 2) {
        goto ret2;
    }
    ret = 0;
    goto done;

ret0:
    ret = D_800B0E18;
    goto done;
ret1:
    ret = D_800B0E1C;
    goto done;
ret2:
    ret = D_800B1630;
done:
    return ret;
}
int func_800CE560(char *out, int stride, int count, FieldAnimTaskCallback callback)
{
  int i;
  register char *entry;
  FieldAnimCallbackList *list = (FieldAnimCallbackList *)out;
  stride += 4;
  entry = out;
  entry = entry + 0xC;
  list->callback = callback;
  list->stride = stride;
  list->count = count;
  for (i = 0; i < count; i++)
  {
    ((FieldAnimCallbackListEntry *)entry)->active = 0;
    entry += stride;
  }

  return (stride * count) + 0xC;
}




int func_800CE5AC(void *arg0, int arg1, int arg2, int arg3, void *arg4) {
    char *header;
    char *entry;
    FieldAnimCallbackList *list;
    int stride;
    int i;

    i = 0;
    stride = arg2 + 4;
    header = D_800F33E0->end + arg1;
    list = (FieldAnimCallbackList *)header;
    entry = header + 0xC;

    *(char **)arg0 = header;
    list->callback = (FieldAnimCallbackListCallback)arg4;
    list->stride = stride;
    list->count = arg3;

    while (i < arg3) {
        ((FieldAnimCallbackListEntry *)entry)->active = 0;
        i++;
        entry += stride;
    }

    return (stride * arg3) + 0xC;
}

void *func_800CE610(char *list) {
    char frame[8];
    FieldAnimCallbackList *base;
    char *entry;
    register int slot_or_count asm("$5");
    register int count asm("$6");
    register int stride asm("$7");
    int i;
    void *ret;

    (void)frame;
    ret = list;
    asm volatile("" : "=r"(ret) : "0"(ret));
    base = (FieldAnimCallbackList *)list;
    asm volatile("" : "=r"(base) : "0"(base));
    entry = list + 0xC;
        slot_or_count = ((FieldAnimCallbackList *)ret)->count;
    stride = ((FieldAnimCallbackList *)ret)->stride;
    i = 0;
    if (slot_or_count > 0) {
        count = slot_or_count;
        while (1) {
            slot_or_count = (int)entry;
            if (((FieldAnimCallbackListEntry *)entry)->active == 0) {
                break;
            }
            i++;
            entry += stride;
            if (i >= count) {
                break;
            }
        }

        if (i < base->count) {
            ((FieldAnimCallbackListEntry *)slot_or_count)->active = 1;
            ((FieldAnimCallbackListEntry *)slot_or_count)->age = 0;
            return entry + 0x4;
        }
    }

    return 0;
}

extern int D_800E27EC;
int func_800CE688(void *data)
{
  char *list = data;
  int frame_pad[2];
  int old_context = D_800E27EC;
  char *entry = list + 0xC;
  char *timer;
  int i = 0;
  register int active = 0;
  register FieldAnimTaskCallback callback = ((FieldAnimCallbackList *)list)->callback;
  int count = ((FieldAnimCallbackList *)list)->count;
  register int stride = ((FieldAnimCallbackList *)list)->stride;
  if (count > 0)
  {
    timer = list + 0xE;
    do
    {
      if (((FieldAnimCallbackListEntry *)entry)->active != 0)
      {
        active++;
        D_800E27EC = ((FieldAnimCallbackListEntry *)(timer - 2))->age;
        if (callback(1, entry + 0x4, D_800E2368->argument) != 0)
        {
          ((FieldAnimCallbackListEntry *)entry)->active = 0;
        }
        else
        {
          ((FieldAnimCallbackListEntry *)(timer - 2))->age = ((FieldAnimCallbackListEntry *)(timer - 2))->age + 1;
        }
      }
      i++;
      timer += stride;
      entry += stride;
    }
    while (i < (((FieldAnimCallbackList *)list)->count));
  }
  if (list)
  {
    D_800E27EC = old_context;
    return active;
  }
  else
  {
    D_800E27EC = old_context;
    return active;
  }
}

int func_800CE78C(char *list)
{

  int old_context = D_800E27EC;
  char *entry = list + 0xC;
  char *payload;
  int i = 0;
  register int active = 0;
  register FieldAnimTaskCallback callback = ((FieldAnimCallbackList *)list)->callback;
  int new_var;
  int count = ((FieldAnimCallbackList *)list)->count;
  register int stride = ((FieldAnimCallbackList *)list)->stride;
  new_var = 0;
  if (count > new_var)
  {
    payload = list + 0x10;
    do
    {
      if (((FieldAnimCallbackListEntry *)entry)->active != 0)
      {
        active++;
        D_800E27EC = *((s16 *) (payload - 0x2));
        callback(2, payload, D_800E2368->argument);
      }
      i++;
      payload += stride;
      entry += stride;
    }
    while (i < (((FieldAnimCallbackList *)list)->count));
  }
  D_800E27EC = old_context;
  return active;
}
