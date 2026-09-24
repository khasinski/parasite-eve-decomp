#include "pe1/room_m256.h"

int func_80195320(int mode, u16 *out) {
    char *p, *q, *r;
    /* Keep the return register available for the retail mode dispatch. */
    register int ret asm("$2");
    char *task;
    u16 value;

    if (mode == 1) goto mode1;
    if (mode < 2) {
        if (mode == 0) goto mode0;
        return 0;
    }
    if (mode == 2) goto mode2;
    return 0;

mode0:
    *out = (u16)D_800E2368->variables[0];
    D_80195EF8 = func_8006E498(D_800B0E64, 0xC5887704);
    func_800C6D5C((u8 *)D_80195EF8, 0, 0);
    if (D_800E2368->flags) {
        p = (char *)D_800F32D0->actor;
        if (p) {
            q = *(char **)p;
            if (q) {
                r = *(char **)(q + 0x18);
                if (*r == 1) *r = 2;
            }
        }
    }
    return func_800CE560(D_800F33E0->end, 0x18, 0x20,
                           func_801940B0);

mode1:
    if (D_800E27EC == mode) {
        ret = D_800E27EC < 8;
        task = func_800CE610(D_800F33E0->end);
        if (task) {
            value = *out;
            *(s16 *)(task + 0x14) = 0;
            *(s16 *)(task + 0x16) = 0;
            *(u16 *)(task + 6) = value;
        }
        func_800D3F64(0x5A6, func_800D3FD8());
        func_800D3F64(0x5BE, 0x80);
        ret = D_800E27EC < 8;
    } else {
        ret = D_800E27EC < 8;
    }
    if (ret) return 0;
    return 2;

mode2:
    {
        unsigned palette_index = D_800E11EA;
        u16 palette;
        palette = D_800E2850[palette_index];
        /* The palette lookup precedes every parameter write in retail. */
        asm volatile("" ::: "memory");
        D_800F336C = 3;
        D_800F336E = 0;
        D_800F3372 = 0;
        D_800F3374 = 8;
        D_800F3370 = palette;
        /* Emit the zero result after the final write, not before it. */
        asm volatile("" : "=r"(ret) : : "memory");
    }
    return 0;
}
