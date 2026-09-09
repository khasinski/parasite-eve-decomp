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


#include "common.h"
int func_800CE560(char *out, int stride, int count, int value)
{
  int i;
  register char *entry;
  stride += 4;
  entry = out;
  entry = entry + 0xC;
  *((int *) (out + 8)) = value;
  *((int *) (out + 0)) = stride;
  *((int *) (out + 4)) = count;
  for (i = 0; i < count; i++)
  {
    *((u16 *) entry) = 0;
    entry += stride;
  }

  return (stride * count) + 0xC;
}



extern void *D_800F33E0;

int func_800CE5AC(void *arg0, int arg1, int arg2, int arg3, void *arg4) {
    char *header;
    char *entry;
    int stride;
    int i;

    i = 0;
    stride = arg2 + 4;
    header = *(char **)((char *)D_800F33E0 + 8) + arg1;
    entry = header + 0xC;

    *(char **)arg0 = header;
    *(void **)(header + 8) = arg4;
    *(int *)header = stride;
    *(int *)(header + 4) = arg3;

    while (i < arg3) {
        *(s16 *)entry = 0;
        i++;
        entry += stride;
    }

    return (stride * arg3) + 0xC;
}
