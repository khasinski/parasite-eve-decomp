/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 --stack-return-delay */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

void *Str_LookupTable8();

extern s32 g_InvCategoryBaseItemId;
extern s32 g_InvItemPtr;
extern u8 g_EquipItemDataTable[];

void *Inv_LookupActiveListDisplayData(s32 index);

void *Inv_LookupActiveListDisplayData(s32 index) {
    s16 itemId;
    register u32 lookupIndex asm("$4");
    void *entry;
    void *displayData;

    itemId = ((s16 *)g_InvItemPtr)[index];
    displayData = NULL;
    if ((u32)(itemId - 0x100) < 0x80U) {
        entry = (itemId << 5) + g_EquipItemDataTable;
        if (M2C_FIELD(entry, u8 *, 5) & 0x10) {
            displayData = g_EquipItemDataTable + 0x31F8;
            if (M2C_FIELD(entry, u8 *, 6) == 9) {
                displayData = g_EquipItemDataTable + 0x3208;
            }
        } else {
            lookupIndex = M2C_FIELD(entry, u8 *, 4) - 1;
            goto lookup_base_data;
        }
    } else {
        lookupIndex = itemId - 1;
        if (lookupIndex >= 0xFFU) {
            if ((u32)(itemId - 0x200) < 9U) {
                lookupIndex = (g_InvCategoryBaseItemId + itemId) - 0x201;
                goto lookup_base_data;
            }
        } else {
lookup_base_data:
            displayData = Str_LookupTable8(lookupIndex);
        }
    }
    return displayData;
}
