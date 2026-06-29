/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK MenuWidget_ClearCursorY();
void *MenuWidget_FindByModeAndSelectedBase();
M2C_UNK MenuWidget_OffsetPosition();
void *func_80062A20();
extern s32 g_MenuItemUseMode;

void Menu_AlignEquipPanels(void) {
    s32 temp_s1;
    void *temp_s0;
    void *temp_v0;
    void *temp_v0_2;

    temp_s0 = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
    __asm__ volatile("" : "=r"(temp_s0) : "0"(temp_s0));
    func_80062A20(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
    __asm__ volatile(
        ".set push\n\t"
        ".set noreorder\n\t"
        "addu $3, $2, $0\n\t"
        ".word 0x8f820000\n\t"
        ".reloc .-4, R_MIPS_GPREL16, g_MenuItemUseMode\n\t"
        "lw $5, 0x18(%1)\n\t"
        "bnez $2, 1f\n\t"
        "addiu $2, $0, 0xB0\n\t"
        "lw $2, 0x80($3)\n\t"
        "nop\n\t"
        "bnez $2, 1f\n\t"
        "addiu $2, $0, 0xA2\n\t"
        "addiu $2, $0, 0x9C\n"
        "1:\n\t"
        "subu %0, $2, $5\n\t"
        "addu $4, %1, $0\n\t"
        "addu $5, %0, $0\n\t"
        ".word 0x0c000000\n\t"
        ".reloc .-4, R_MIPS_26, MenuWidget_OffsetPosition\n\t"
        "addu $6, $0, $0\n\t"
        ".set pop"
        : "=r"(temp_s1)
        : "r"(temp_s0)
        : "$2", "$3", "$4", "$5", "$6", "memory");
    MenuWidget_ClearCursorY(temp_s0);
    temp_v0 = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    MenuWidget_OffsetPosition(temp_v0, temp_s1, 0);
    MenuWidget_ClearCursorY(temp_v0);
    temp_v0_2 = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
    MenuWidget_OffsetPosition(temp_v0_2, temp_s1, 0);
    MenuWidget_ClearCursorY(temp_v0_2);
}
