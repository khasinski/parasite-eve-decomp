#ifndef PE1_INVENTORY_CATEGORY_H
#define PE1_INVENTORY_CATEGORY_H

#include "common.h"

typedef struct InventoryCategoryState {
    u16 count;
    /* Remaining words have not been identified. */
    u16 unknown[15];
} InventoryCategoryState;

extern InventoryCategoryState g_InvCategoryItemTable[];

#endif
