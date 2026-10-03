#include "pe1/gte.h"
#include "pe1/field_anim.h"
#include "pe1/render_object.h"
#include "pe1/pe_image.h"

int func_800D4620(char *obj)
{
  register char *base;
  int i;
  register char *p;
  register char *q;
  int sentinel;
  base = obj + 0xC;
  i = 6;
  p = obj + 0x18;
  obj[2] = 1;
  obj[3] = 0;
  *((short *) (obj + 0x1A)) = 0;
  *((short *) (obj + 0x1C)) = 0;
  *((char **) (obj + 0x10)) = obj + 0x90;
  obj[0x18] = 0;
  obj[0x19] = 0;
  for (; i >= 0; i--)
  {
    *((short *) (p + 0x12)) = 0;
    p -= 2;
  }

  p = base + 0x20;
  i = 0;
  sentinel = 0xFFFF;
  q = base + 0x24;
  for (; i < 8; p += 0xC)
  {
    i++;
    *((short *) p) = sentinel;
    *((short *) (q - 2)) = 0;
    *((int *) q) = 0;
    q += 0xC;
  }

  return 0;
}

extern void *g_FieldEngineObject __asm__("D_800F32D0");
extern void *g_FieldEngineContext __asm__("D_800E2368");

int func_800D4698(char *obj, int skip, int arg2, int arg3, int arg4, int arg5) {
    void *ctx;
    int (*callback)(int, int, int, int);

    ctx = obj + 0xC;
    if (skip == 0) {
        callback = *(int (**)(int, int, int, int))(*(char **)(obj + 0x8C) + 0x30);
        g_FieldEngineObject = obj;
        g_FieldEngineContext = ctx;
        if (callback != 0) {
            *(int *)(obj + 0x14) = callback(arg2, arg3, arg4, arg5);
        }
    }
    return 0;
}

int func_800D4704(FieldAnimTaskOwner *owner)
{
    FieldAnimTaskContext *context = &owner->tasks;
    FieldAnimTaskSlot *slot = context->slots;
    int i;
    D_800F32D0 = &owner->prefix;
    D_800E2368 = context;
    if (context->flags)
        D_800F3428 = Asset_SearchByKeyType(owner->prefix.asset_type);
    i = 0;
    for (; i < 8; i++, slot++) {
        if (slot->id != 65535) {
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            D_800F33E0 = slot;
            D_800E27EC = slot->age;
            context->table->callbacks[slot->id](2, slot->start, context->argument);
            if (slot->end)
                func_800CE78C(slot->end);
        }
    }
    return 0;
}
