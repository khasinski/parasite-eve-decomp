/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Sfx_DrawSlotRow();
void *Item_LookupBaseData();
void *Str_LookupTable8(u32);
extern s32 g_InvCategoryBaseItemId;
extern s16 *g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern u8 g_KeyItemDataTable[] __asm__("g_KeyItemDataTable");
extern u8 g_EquipItemDataTable[] __asm__("g_EquipItemDataTable");

void Sfx_DrawActiveListSlot(s32 arg0) {
    s16 temp_v1;
    s32 temp_v1_3;
    register s32 temp_a1 asm("$5");
    u32 temp_a0;
    register u32 var_a0 asm("$4");
    void *temp_v1_2;
    register void *var_a1 asm("$5");
    void *var_v0;
    void *saved_a1;

    temp_v1 = *(arg0 + g_InvItemPtr);
    var_a1 = NULL;
    if ((u32) (temp_v1 - 0x100) < 0x80U) {
        temp_v1_2 = (temp_v1 << 5) + g_EquipItemDataTable;
        if (M2C_FIELD(temp_v1_2, u8 *, 5) & 0x10) {
            var_a1 = g_EquipItemDataTable + 0x31F8;
            if (M2C_FIELD(temp_v1_2, u8 *, 6) == 9) {
                var_a1 = g_EquipItemDataTable + 0x3208;
            }
        } else {
            var_a0 = M2C_FIELD(temp_v1_2, u8 *, 4) - 1;
            goto block_8;
        }
    } else {
        var_a0 = temp_v1 - 1;
        if (var_a0 >= 0xFFU) {
            if ((u32) (temp_v1 - 0x200) < 9U) {
                var_a0 = (g_InvCategoryBaseItemId + temp_v1) - 0x201;
                goto block_8;
            }
        } else {
block_8:
            var_a1 = Str_LookupTable8(var_a0);
        }
    }
    saved_a1 = var_a1;
    if ((arg0 >= 0) && (arg0 < g_InvSlotLimit)) {
        temp_v1_3 = *(arg0 + g_InvItemPtr);
        temp_a1 = temp_v1_3;
        if ((u32) (temp_v1_3 - 0x100) < 0x80U) {
            var_v0 = (temp_v1_3 << 5) + g_EquipItemDataTable;
        } else {
            temp_a0 = temp_v1_3 - 1;
            if (temp_a0 < 0xFFU) {
                var_v0 = Item_LookupBaseData(temp_a0, temp_a1);
            } else if ((u32) (temp_a1 - 0x200) < 9U) {
                temp_v1_3 = temp_a1 << 5;
                var_v0 = temp_v1_3 + g_KeyItemDataTable;
            } else {
                goto block_17;
            }
        }
    } else {
block_17:
        var_v0 = NULL;
    }
    if (var_v0 != NULL) {
        Sfx_DrawSlotRow(var_v0, saved_a1);
    }
}
