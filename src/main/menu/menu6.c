/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "include_asm.h"

typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;

s32 MenuWidget_GetChild(s32 node, s32 index);
s32 MenuWidget_GetCellIndex(s32 node);
s32 MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
void MenuWidget_DestroyNode(s32 node);
void Menu_CreateBonusPointAllocationView(void);
void Menu_StepItemDetailPanel2(void);
void Menu_PlayConfirmSound(void);
void Menu_PlayCancelSound(void);
void Menu_PlayErrorSound(void);

asm(".globl func_80046B58");
asm("func_80046B58 = Menu_SkillActionHandler");

int g_MenuBattleSelectedItem;

int Menu_GetBattleEquipMode(void);
void MenuWidget_NavScrollTo(int arg0);
void Battle_UseItem(int arg0);

s32 Menu_SkillActionHandler(s32 node, s32 flags) {
    s32 child;
    s32 handled;

    handled = 0;
    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x10000) {
        if (MenuWidget_GetCellIndex(child) >= 0) {
            handled = 1;
            Menu_StepItemDetailPanel2();
            Menu_PlayConfirmSound();
        } else {
            handled = 1;
            Menu_PlayErrorSound();
        }
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(node);
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1D));
        Menu_CreateBonusPointAllocationView();
        handled = 1;
        Menu_PlayCancelSound();
    }
    return handled;
}

INCLUDE_ASM("asm/USA/main/nonmatchings/menu/menu6", Menu_StepItemDetailPanel2);

void Menu_OnBattleCommandConfirm(int arg0, int arg1) {
    if (arg1 != 0) {
        if (Menu_GetBattleEquipMode() != 0) {
            MenuWidget_NavScrollTo(8);
        }
        Battle_UseItem(g_MenuBattleSelectedItem);
    }
}
