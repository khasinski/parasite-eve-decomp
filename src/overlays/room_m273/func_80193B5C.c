#include "common.h"
extern s32 D_800E27EC;
extern s16 D_8019AC18[];
extern s32 D_8019AC0C[];

extern s32 func_800D0728(void *obj, s16 a, s16 b, s32 c, s16 *params, s32 x0, s32 x1, s32 *p0, s32 *p1, s16 y, s32 one);

s32 func_80193B5C(s32 mode, void **arg1) {
    s16 params[4];
    s16 i;
    void **objp = arg1;
    s16 *x;
    s16 *x_next;
    s32 *z;
    s32 *z_next;

    if (mode == 1) {
        if (D_800E27EC < 0x20) {
            goto ret0;
        }
        goto ret1;
    }
    if (mode != 2) {
        goto ret0;
    }

    i = 0;
    x = D_8019AC18;
    x_next = x + 1;
    z = D_8019AC0C;
    z_next = z + 1;

    params[0] = 0x400;
    params[1] = D_800E27EC << 7;
    params[2] = 0;
    params[3] = 1;

    do {
        func_800D0728(*objp, x[i], x_next[i], 0x10, params,
                      *(s16 *)((char *)*objp + 8), *(s16 *)((char *)*objp + 8),
                      (s32 *)(((s32)i * 4) + (s32)z), (s32 *)(((s32)i * 4) + (s32)z_next),
                      *(s16 *)((char *)*objp + 0xC), 1);
        i++;
    } while ((s16)i < 2);

ret0:
    return 0;
ret1:
    return 1;
}
