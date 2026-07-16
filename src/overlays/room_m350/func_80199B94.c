typedef unsigned short u16;
typedef signed short s16;
typedef int s32;

extern s32 D_800E27EC;
extern u16 D_800E1204[];
extern s16 D_800F336A;
extern u16 D_800F336C;
extern s32 D_800F3428;
extern s32 D_800966EC[];
extern void D_8019A410;

s32 func_80077AA4(s32 arg0, s32 arg1);
s32 func_800CEE20(void *arg0, void *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, void *arg8);

s32 func_80199B94(s32 mode, void *ctx) {
    s32 tex;

    if (mode == 1) {
        if (D_800E27EC < 8) {
            goto ret0;
        }
        goto ret1;
    }

    if (mode != 2) {
        return 0;
    }

    tex = D_800E1204[D_800F336C];
    if (D_800F336C == 4 && D_800F3428 != 0) {
        tex += 4;
    }

    func_800CEE20(ctx, 0, 0x1800, 0x1800, D_800F336A + 0xDC, (u16)func_80077AA4(0x30, tex), 1,
                  (*(s32 *)((char *)D_800966EC + (((D_800E27EC - 1) << 10) & 0x3C00)) << 16) >> 21, &D_8019A410);
ret0:
    return 0;

ret1:
    return 1;
}
