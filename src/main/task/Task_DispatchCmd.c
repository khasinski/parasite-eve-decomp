#include "common.h"
#include "pe1/gte.h"

extern u32 D_8009D26C[];
extern u32 D_8009D1F4[];
extern u32 D_8009D1E4[];
extern u32 D_800A7770[];

int Task_DispatchCmd(int **args) {
    int mode;

    mode = *args[0];
    switch (mode) {
    case 0: {
        u32 mask;
        u32 state;
        state = D_8009D26C[0];
        mask = *args[1];
        if ((state & mask) == mask) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
        break;
    }
    case 1: {
        u32 mask;
        u32 state;
        state = D_8009D1F4[0];
        mask = *args[1];
        if ((state & mask) == mask) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
        break;
    }
    case 2: {
        u32 mask;
        u32 state;
        state = D_8009D1E4[0];
        mask = *args[1];
        if ((state & mask) == mask) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
        break;
    }
    case 3: {
        u32 mask;
        u32 state;
        int index;

        index = (int)args[1];
        state = D_8009D26C[0];
        mask = *(u32 *)index;
        state &= mask;
        if (state == mask) {
            gte_ldlzcs(mask);
            index = 31;
            if (state != 0x80000000) {
                gte_stlzcr(args[1]);
                index -= *args[1];
            }
            *args[2] = D_800A7770[index];
        } else {
            *args[2] = 0;
        }
        break;
    }
    }
    return 1;
}
