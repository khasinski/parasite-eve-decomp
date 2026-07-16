typedef unsigned short u16;
typedef int s32;

extern s32 D_800E27EC;
extern u16 D_800E1204[];
extern u16 D_800F336C;
extern s32 D_800F3428;
extern void D_8019A630;

s32 func_80077AA4(s32 arg0, s32 arg1);
s32 func_800CEE20(void *arg0, void *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, void *arg8);

s32 func_8019A014(s32 mode, void *ctx) {
    s32 tex;

    if (mode == 1) {
        if (D_800E27EC < 5) {
            goto ret0;
        }
        goto ret1;
    }

    if (mode != 2) {
        return 0;
    }

    if (D_800E27EC < 2) {
        return 0;
    }

    tex = D_800E1204[D_800F336C];
    if (D_800F336C == 4 && D_800F3428 != 0) {
        tex += 4;
    }

    func_800CEE20(ctx, 0, 0x1000, 0x1000, 0xDC, (u16)func_80077AA4(0x30, tex), 1, (6 - D_800E27EC) << 5, &D_8019A630);
ret0:
    return 0;

ret1:
    return 1;
}
