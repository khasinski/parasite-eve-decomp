#include "pe1/gte.h"
#include "pe1/field_anim.h"
#include "pe1/render_object.h"
#include "pe1/pe_image.h"

int func_800D4620(FieldAnimTaskOwner *owner)
{
  register char *base;
  int i;
  register char *p;
  register char *q;
  int sentinel;
  char *obj = (char *)owner;
  base = (char *)&owner->tasks;
  i = 6;
  p = obj + 0x18;
  owner->prefix.reserved02[0] = 1;
  owner->prefix.reserved02[1] = 0;
  owner->tasks.delay = 0;
  owner->tasks.used = 0;
  owner->tasks.cursor = owner->tasks.arena;
  owner->tasks.count = 0;
  owner->tasks.flags = 0;
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


int func_800D4698(FieldAnimTaskOwner *owner, int skip, int arg2, int arg3, int arg4, int arg5) {
    FieldAnimTaskContext *ctx;
    int (*callback)(int, int, int, int);

    ctx = &owner->tasks;
    if (skip == 0) {
        callback = ((FieldAnimTaskProgram *)owner->tasks.table)->initialize;
        D_800F32D0 = &owner->prefix;
        D_800E2368 = ctx;
        if (callback != 0) {
            owner->tasks.argument = callback(arg2, arg3, arg4, arg5);
        }
    }
    return 0;
}

/* Matching debt: four register pins for the matrix pointer and GTE words.
 * Matrix loads are C; each GTE transfer uses its individual macro. */
int func_800D4704(FieldAnimTaskOwner *owner)
{
    FieldAnimTaskContext *context = &owner->tasks;
    FieldAnimTaskSlot *slot = context->slots;
    int i;
    GteMatrix **matrixSlot;
    D_800F32D0 = &owner->prefix;
    D_800E2368 = context;
    if (context->flags)
        D_800F3428 = Asset_SearchByKeyType(owner->prefix.asset_type);
    i = 0;
    matrixSlot = &D_800BCFA4.value;
    for (; i < 8; i++, slot++) {
        if (slot->id != 65535) {
            {
                register const GteMatrixWords *matrix asm("$7");
                register u32 a asm("$12");
                register u32 b asm("$13");
                register u32 c asm("$14");
                matrix = (const GteMatrixWords *)*matrixSlot;
                a = matrix->r11_r12;
                b = matrix->r13_r21;
                gte_ctc2_0(a);
                gte_ctc2_1(b);
                a = matrix->r22_r23;
                b = matrix->r31_r32;
                c = matrix->r33_pad;
                gte_ctc2_2(a);
                gte_ctc2_3(b);
                gte_ctc2_4(c);
                a = matrix->tx;
                b = matrix->ty;
                gte_ctc2_5(a);
                c = matrix->tz;
                gte_ctc2_6(b);
                gte_ctc2_7(c);
            }
            D_800F33E0 = slot;
            D_800E27EC = slot->age;
            context->table->callbacks[slot->id](2, slot->start, context->argument);
            if (slot->end)
                func_800CE78C(slot->end);
        }
    }
    return 0;
}
