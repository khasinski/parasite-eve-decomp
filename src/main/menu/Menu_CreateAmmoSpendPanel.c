/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void MenuWidget_SetCurrentNode(void *node);
void Inv_SelectActiveList(s32 useOverride);
void *Inv_LookupActiveListData(s32 index);
void Inv_ClearSelectionBitset(void);
void Inv_BuildDisplayFromList(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void *MenuWidget_CreateSimpleNode(s32 mode, s32 arg1, s32 arg2, s32 arg3);

extern s32 g_InvAmmoSpendActiveList;
extern s32 g_InvSelectedItemIndex;
extern s32 g_InvSwapSourceList;
extern s32 g_InvSwapTargetIndex;
extern s32 g_MenuSpendArrowDirection;

void Menu_AmmoSpendInputHandler(void);
void Menu_DrawAmmoSpendPanel(void);

asm(".globl func_800451D0");
asm("func_800451D0 = Menu_CreateAmmoSpendPanel");

void Menu_CreateAmmoSpendPanel(s32 parent) {
    void *node;
    void *item;

    node = MenuWidget_CreateSimpleNode(0x3E, parent, 0, 1);
    M2C_FIELD(node, void **, 0x2C) = Menu_AmmoSpendInputHandler;
    M2C_FIELD(node, void **, 0x30) = Menu_DrawAmmoSpendPanel;
    M2C_FIELD(node, s32 *, 0x28) = 1;
    MenuWidget_SetCurrentNode(node);
    Inv_SelectActiveList(g_InvAmmoSpendActiveList);
    item = Inv_LookupActiveListData(g_InvSelectedItemIndex);
    if ((item != NULL) && ((u32)(M2C_FIELD(item, u8 *, 6) - 0x13) >= 3U)) {
        asm(
            ".word 0x8f820000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvAmmoSpendActiveList\n"
            ".word 0x8f840000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvSwapSourceList\n"
            ".word 0x8f830000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvSelectedItemIndex\n"
            ".word 0x8f850000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvSwapTargetIndex\n"
            ".word 0x00441026\n"
            ".word 0x00822026\n"
            ".word 0xaf820000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvAmmoSpendActiveList\n"
            ".word 0x00441026\n"
            ".word 0x00651826\n"
            ".word 0x00a32826\n"
            ".word 0xaf830000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvSelectedItemIndex\n"
            ".word 0x00651826\n"
            ".word 0xaf840000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvSwapSourceList\n"
            ".word 0xaf820000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvAmmoSpendActiveList\n"
            ".word 0xaf850000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvSwapTargetIndex\n"
            ".word 0xaf830000\n"
            ".reloc .-4, R_MIPS_GPREL16, g_InvSelectedItemIndex\n"
        );
    }
    Inv_BuildDisplayFromList(g_InvAmmoSpendActiveList, g_InvSelectedItemIndex, g_InvSwapSourceList, g_InvSwapTargetIndex);
    Inv_ClearSelectionBitset();
    g_MenuSpendArrowDirection = 0;
}
