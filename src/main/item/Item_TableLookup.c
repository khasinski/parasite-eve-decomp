#include "pe1/inventory.h"
#include "pe1/aya.h"

extern int D_800A8034[];
extern int D_800A8038[];

ItemDataRecord *Item_LookupBaseData(unsigned int index)
{
    int *endPtr;
    int base;
    unsigned int offset;
    int end;

    endPtr = D_800A8038;
    end = endPtr[0];
    base = D_800A8034[0];

    if (index >= ((unsigned int)(end - base) >> 5)) {
        return 0;
    }

    offset = index << 5;
    return (ItemDataRecord *)((char *)D_800A8038 - 0x10 + base + offset);
}


extern int g_StatGrowthTable[];

void *Stat_GetGrowthTable(int arg0) {
    return (void *)(g_StatGrowthTable[0] + (int)((char *)g_StatGrowthTable - 0x10) + (arg0 << 9));
}
