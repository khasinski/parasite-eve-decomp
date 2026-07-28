typedef unsigned int u32;

#include "pe1/gte.h"

extern u32 D_8009D26C;
extern u32 D_8009D1F4;
extern u32 D_8009D1E4;
extern u32 D_800A7770[];

int Task_DispatchCmd(int **args) {
    int type;
    u32 mask;
    int leading;

    type = *args[0];

    if (type == 0) {
        if ((D_8009D26C & *args[1]) == *args[1]) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
    } else if (type == 1) {
        if ((D_8009D1F4 & *args[1]) == *args[1]) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
    } else if (type == 2) {
        if ((D_8009D1E4 & *args[1]) == *args[1]) {
            *args[2] = 1;
        } else {
            *args[2] = 0;
        }
    } else if (type == 3) {
        mask = *args[1];
        if ((D_8009D26C & mask) != mask) {
            *args[2] = 0;
        } else {
            if (mask == 0x80000000) {
                leading = 0;
                *args[2] = D_800A7770[31];
            } else {
                gte_ldlzcs(mask);
                gte_stlzcr(args[1]);
                leading = *args[1];
                *args[2] = D_800A7770[31 - leading];
            }
        }
    }

    return 1;
}
