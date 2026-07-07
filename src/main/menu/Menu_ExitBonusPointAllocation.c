/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "include_asm.h"
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void MenuWidget_ClearCursorY();
void *MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
void *MenuWidget_GetCurrentNode(void);
void MenuWidget_SetCurrentNode(void *node);
void Menu_CreateBonusPointAllocationView(void);
void Inv_RecalcSlotStats(s32 arg0, void *arg1);
void MenuWidget_NavScrollTo(s32 selected_base);

extern s32 g_MenuBattleStatusOverlayActive;
extern s32 g_MenuEquipMode;
extern s32 g_MenuItemUseMode;
extern s32 g_MenuItemRenameMode;
extern s32 g_BonusPointDisplayValue;
extern u16 g_BonusPointStatDeltas[];
#define g_BonusPointStatDeltas (g_BonusPointStatDeltas[0])
extern u16 g_AyaStatAgility[];
#define g_AyaStatAgility (g_AyaStatAgility[0])
extern s32 g_AyaBonusPoints[];
#define g_AyaBonusPoints (g_AyaBonusPoints[0])

asm(".globl func_800490B0");
asm("func_800490B0 = Menu_ExitBonusPointAllocation");

void Menu_ExitBonusPointAllocation(void) {
    s32 index;
    u8 *dst;
    u8 *src;
    u16 value;

    dst = (u8 *)&g_AyaStatAgility;
    index = 0;
    src = (u8 *)&g_BonusPointStatDeltas;
    do {
        value = *(u16 *)src;
        src += 4;
        index += 1;
        *(u16 *)dst = value;
        dst += 2;
    } while (index < 7);
    g_AyaBonusPoints = g_BonusPointDisplayValue;
    Inv_RecalcSlotStats(index, dst);
    MenuWidget_NavScrollTo(0xF);
    MenuWidget_NavScrollTo(0xB);
    MenuWidget_NavScrollTo(0xD);
    MenuWidget_NavScrollTo(0x18);
    MenuWidget_NavScrollTo(0x30);
    if (g_MenuEquipMode != 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
    } else {
        if (MenuWidget_GetCurrentNode() != NULL) {
            MenuWidget_ClearCursorY(M2C_FIELD(MenuWidget_GetCurrentNode(), s32 *, 4));
        }
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    }
    g_MenuItemRenameMode = 0;
    g_MenuItemUseMode = 0;
    if (g_MenuEquipMode == 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
        Menu_CreateBonusPointAllocationView();
        g_MenuBattleStatusOverlayActive = 1;
    }
}
