/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"

void *MenuWidget_FindByModeAndSelectedBase(s32 mode, s32 base);
void MenuWidget_SetCurrentNode(void *node);
void Menu_OpenItemList(void);
void Menu_StepInventoryRoot(s32 arg0, s32 arg1, s32 arg2);
void Menu_PlayCancelSound(void);
void Menu_PlayMoveSound(void);
s32 Inv_RestoreSelection(s32 index);
void Inv_BuildStorageDisplay(void);
void *MenuWidget_GetChild(s32 node, s32 index);
void MenuWidget_NavScrollTo(s32 selected_base);

extern s32 g_MenuSelectedItemList;

asm(".globl func_80047D74");
asm("func_80047D74 = Menu_EquipStatsInputHandler");

s32 Menu_EquipStatsInputHandler(s32 node, s32 flags) {
    void *selected;
    void *child;
    void *target;

    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x4000) {
        M2C_FIELD(child, s32 *, 0x44) = -1;
        child = MenuWidget_FindByModeAndSelectedBase(2, 0xB);
        selected = child;
        M2C_FIELD(selected, s32 *, 0x44) = 0;
        M2C_FIELD(selected, s32 *, 0x48) = 0;
        MenuWidget_SetCurrentNode(selected);
        Menu_PlayMoveSound();
        return 1;
    }
    if (flags & 0x10000) {
        Menu_OpenItemList();
        return 1;
    }
    if (flags & 0x40) {
        Inv_BuildStorageDisplay();
        MenuWidget_NavScrollTo(0x2C);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xA);
        MenuWidget_NavScrollTo(5);
        MenuWidget_NavScrollTo(6);
        MenuWidget_NavScrollTo(7);
        target = MenuWidget_FindByModeAndSelectedBase(2, 0);
        if (target == NULL) {
            target = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
        }
        MenuWidget_SetCurrentNode(target);
        Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
        Menu_PlayCancelSound();
    }
    return 1;
}
