/* CC1_FLAGS: -g3 -G8 */
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

asm(".globl Inv_GetActiveListItemType");
asm("Inv_GetActiveListItemType = func_8005415C");

void *Item_LookupBaseData() __asm__("func_8005DB44");

extern s32 g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern struct { char _[16]; } D_8009DE64_o __asm__("g_KeyItemDataTable");
#define g_KeyItemDataTable (*(M2C_UNK *)&D_8009DE64_o)
extern struct { char _[16]; } D_800BEEAC_o __asm__("g_EquipItemDataTable");
#define g_EquipItemDataTable (*(M2C_UNK *)&D_800BEEAC_o)

u8 Inv_GetActiveListItemType(s32 index) __asm__("func_8005415C");

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
