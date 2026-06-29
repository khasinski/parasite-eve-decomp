/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Battle_SyncEquipSlots();
s32 func_80052F70();
u16 *func_8005DBAC();
s32 func_8005DBF8();
extern void *g_InvItemPtr;
extern s32 g_InvSlotLimit;
extern struct { char _[16]; } D_800C0E06_o __asm__("g_AyaHpMax");
#define g_AyaHpMax (*(u16 *)&D_800C0E06_o)
extern struct { char _[16]; } D_800C0E08_o __asm__("g_AyaHpCurrent");
#define g_AyaHpCurrent (*(u16 *)&D_800C0E08_o)
extern struct { char _[16]; } g_AyaInventoryItems_o __asm__("g_AyaInventoryItems");
#define g_AyaInventoryItems (*(M2C_UNK *)&g_AyaInventoryItems_o)
extern struct { char _[16]; } g_AyaInventorySlotCount_o __asm__("g_AyaInventorySlotCount");
#define g_AyaInventorySlotCount (*(s8 *)&g_AyaInventorySlotCount_o)
extern struct { char _[16]; } g_AyaParasiteSpellFlags_o __asm__("g_AyaParasiteSpellFlags");
#define g_AyaParasiteSpellFlags (*(s32 *)&g_AyaParasiteSpellFlags_o)
extern struct { char _[16]; } g_AyaSaveLevel_o __asm__("g_AyaSaveLevel");
#define g_AyaSaveLevel (*(u8 *)&g_AyaSaveLevel_o)
extern struct { char _[16]; } g_AyaSaveTotalExp_o __asm__("g_AyaSaveTotalExp");
#define g_AyaSaveTotalExp (*(s32 *)&g_AyaSaveTotalExp_o)

void Inv_InitMaxLevelInventory(s32 arg0) {
    register s16 *temp_a1 asm("$5");
    s16 *var_v1;
    register s16 var_v0 asm("$2");
    s32 var_a0;
    u16 temp_v0;
    s32 saved_arg0;
    void *inv_base;
    s32 spell_flags;
    u16 *spell_ptr;

    saved_arg0 = arg0;
    inv_base = &g_AyaInventoryItems;
    g_AyaInventorySlotCount = 0x32;
    if (g_InvItemPtr == inv_base) {
        g_InvSlotLimit = func_80052F70();
    }
    __asm__ volatile(
        ".set push\n\t"
        ".set noat\n\t"
        ".set noreorder\n\t"
        "addiu $2, $0, 0x62\n\t"
        "lui $1, %%hi(g_AyaSaveLevel)\n\t"
        "sb $2, %%lo(g_AyaSaveLevel)($1)\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, func_8005DBF8\n\t"
        "nop\n\t"
        "lui $3, %%hi(g_AyaSaveLevel)\n\t"
        "lbu $3, %%lo(g_AyaSaveLevel)($3)\n\t"
        "nop\n\t"
        "sll $3, $3, 2\n\t"
        "addu $3, $3, $2\n\t"
        "lw $2, 0($3)\n\t"
        "lui $1, %%hi(g_AyaSaveTotalExp)\n\t"
        "sw $2, %%lo(g_AyaSaveTotalExp)($1)\n\t"
        ".set pop"
        ::: "$1", "$2", "$3", "$31", "memory");
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
            __asm__ volatile(
                "addiu %0, %1, -1\n"
                "sltiu %0, %0, 2"
                : "=r"(cmp)
                : "r"(var_a0));
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
