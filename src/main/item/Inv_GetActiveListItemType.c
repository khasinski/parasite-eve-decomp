/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;

#define NULL ((void *)0)

#include "../../../tools/m2c/m2c_macros.h"

void *Item_LookupBaseData();

extern s32 g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern M2C_UNK g_KeyItemDataTable[];
#define g_KeyItemDataTable (g_KeyItemDataTable[0])
extern M2C_UNK g_EquipItemDataTable[];
#define g_EquipItemDataTable (g_EquipItemDataTable[0])

u8 Inv_GetActiveListItemType(s32 index);

u8 Inv_GetActiveListItemType(s32 index)
{
    register s32 item_id asm("$3");
    register s32 saved_item_id asm("$5");
    u32 base_index;
    void *data;

    if ((index >= 0) && (index < g_InvSlotLimit)) {
        item_id = ((s16 *)g_InvItemPtr)[index];
        saved_item_id = item_id;
        if ((u32)(item_id - 0x100) < 0x80U) {
            data = (item_id << 5) + (u8 *)&g_EquipItemDataTable;
        } else {
            base_index = item_id - 1;
            if (base_index < 0xFFU) {
                data = Item_LookupBaseData(base_index, item_id);
            } else if ((u32)(saved_item_id - 0x200) < 9U) {
                item_id = saved_item_id << 5;
                data = item_id + (u8 *)&g_KeyItemDataTable;
            } else {
                goto invalid;
            }
        }
    } else {
invalid:
        data = NULL;
    }

    if (data == NULL) {
        return 0U;
    }
    return M2C_FIELD(data, u8 *, 6);
}
