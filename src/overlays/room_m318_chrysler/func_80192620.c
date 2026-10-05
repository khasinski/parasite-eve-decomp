#include "common.h"
extern s32 D_800E27EC;
extern char D_80199464[];

extern s32 func_80071A54(void);
extern void func_800CF3AC();
extern void func_800D1DEC();

s32 func_80192620(s32 mode, s16 *pos) {
    s16 in[4];
    s16 out[4];
    if (mode == 1) {
        goto mode1;
    }
    if (mode == 2) {
        goto mode2;
    }
    return 0;

mode1:
    pos[0] = (u16)(pos[0] - 3 + (func_80071A54() & 7));
    pos[1] = (u16)(pos[1] - 7 + (func_80071A54() & 3));
    pos[2] = (u16)(pos[2] - 3 + (func_80071A54() & 7));
    if (D_800E27EC >= 0x12) {
        return 1;
    }
    goto done;

mode2:
    in[0] = pos[0];
    in[1] = pos[1];
    in[2] = pos[2];
    func_800CF3AC(D_80199464, out, D_800E27EC);
    func_800D1DEC(in, out, 0x80, 1);

done:
    return 0;
}
