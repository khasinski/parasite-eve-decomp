#include "pe1/inventory.h"

extern unsigned char g_EquipItemDataTable[];
extern unsigned char g_KeyItemDataTable[];

void *Inv_LookupData(unsigned int arg0) {
    if ((arg0 - 0x100U) < 0x80U) {
        return &g_EquipItemDataTable[arg0 << 5];
    }

    if ((arg0 - 1U) < 0xFFU) {
        return Item_LookupBaseData(arg0 - 1);
    }

    if ((arg0 - 0x200U) >= 9U) {
        return 0;
    }

    return &g_KeyItemDataTable[arg0 << 5];
}
