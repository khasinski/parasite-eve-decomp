#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Battle_SyncEquipSlots();
s32 Inv_GetAyaSlotLimit();
u16 *func_8005DBAC();
s32 Aya_GetLevelExpTable();
extern void *g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern u16 g_AyaHpMax[];
#define g_AyaHpMax (g_AyaHpMax[0])
extern u16 g_AyaHpCurrent[];
#define g_AyaHpCurrent (g_AyaHpCurrent[0])
extern M2C_UNK g_AyaInventoryItems[];
#define g_AyaInventoryItems (g_AyaInventoryItems[0])
extern s8 g_AyaInventorySlotCount[];
#define g_AyaInventorySlotCount (g_AyaInventorySlotCount[0])
extern s32 g_AyaParasiteSpellFlags[];
#define g_AyaParasiteSpellFlags (g_AyaParasiteSpellFlags[0])
extern u8 g_AyaSaveLevel[];
extern u8 g_AyaSaveLevel_read[] asm("g_AyaSaveLevel");
#define g_AyaSaveLevel (g_AyaSaveLevel[0])
extern s32 g_AyaSaveTotalExp[];
#define g_AyaSaveTotalExp (g_AyaSaveTotalExp[0])

void Inv_InitMaxLevelInventory(s32 arg0) {
    s16 *temp_a1;
    s16 *var_v1;
    register s16 var_v0 asm("$2");
    s32 var_a0;
    u16 temp_v0;
    s32 saved_arg0;
    void *inv_base;
    s32 spell_flags;
    u16 *spell_ptr;
    s32 *exp_table;

    saved_arg0 = arg0;
    inv_base = &g_AyaInventoryItems;
    g_AyaInventorySlotCount = 0x32;
    if (g_InvItemPtr == inv_base) {
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    }
    g_AyaSaveLevel = 0x62;
    exp_table = (s32 *)Aya_GetLevelExpTable();
    g_AyaSaveTotalExp = exp_table[g_AyaSaveLevel_read[0]];
    spell_ptr = func_8005DBAC(0x62);
    spell_flags = 0xFFFFF;
    temp_a1 = (s16 *)((u8 *)inv_base - 0x20);
    temp_v0 = *spell_ptr;
    var_a0 = 0;
    g_AyaParasiteSpellFlags = spell_flags;
    g_AyaHpCurrent = temp_v0;
    g_AyaHpMax = temp_v0;
    __asm__ volatile("" ::: "memory");
    var_v1 = temp_a1;
    do {
        temp_a1 = (s16 *)((u8 *)var_v1 + 2);
        if (saved_arg0 == 0) {
            var_v0 = 0x3E8;
        } else {
            register s32 cmp asm("$2");
            cmp = (u32)(var_a0 - 1) < 2;
            if (cmp != 0) {
                var_v0 = 0;
            } else {
                var_v0 = 0x3E8;
            }
        }
        *var_v1 = var_v0;
        var_a0 += 1;
        var_v1 = temp_a1;
    } while (var_a0 < 7);
    Battle_SyncEquipSlots(var_a0, temp_a1);
}
