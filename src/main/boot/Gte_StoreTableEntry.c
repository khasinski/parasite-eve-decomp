typedef unsigned int u32;

#include "pe1/gte.h"

extern int D_800A76F0[];

void Gte_StoreTableEntry(u32 mask, int value) {
    int index;

    gte_ldlzcs(mask);
    index = 31;
    if (mask != 0x80000000) {
        gte_stlzcr(&mask);
        index -= mask;
    }
    D_800A76F0[index] = value;
}
