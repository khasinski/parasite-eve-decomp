/* CC1_FLAGS: -g3 -G8 */
/* MASPSX_FLAGS: -G8 */
typedef signed char s8;typedef unsigned char u8;typedef short s16;typedef unsigned short u16;typedef int s32;typedef unsigned int u32;typedef long long s64;
#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
s32 MenuWidget_FindByModeAndSelectedBase(M2C_UNK, M2C_UNK); /* extern */
M2C_UNK MenuWidget_SetCurrentNode(void *);          /* extern */
M2C_UNK Menu_CreateBonusPointAllocationView();                            /* extern */
M2C_UNK Menu_CreateContextHelpPanel();                            /* extern */
s32 Menu_GetBattleEquipMode();                                /* extern */
void *MenuWidget_CreateSimpleNode(M2C_UNK, M2C_UNK, M2C_UNK, M2C_UNK); /* extern */
void *MenuWidget_CreateNode(M2C_UNK, void *, void *);       /* extern */
M2C_UNK Draw_SetPrimCallback(void *, s32);                 /* extern */
extern s32 g_MenuItemContextFlag;
extern s32 g_MenuBattleStatusOverlayActive;
extern s32 g_MenuSelectionLocked;
extern struct { char _[16]; } func_80043DA4_o __asm__("Menu_StepItemSelectScreen");
#define Menu_StepItemSelectScreen (*(M2C_UNK *)&func_80043DA4_o)
extern struct { char _[16]; } Menu_DrawItemList_o __asm__("Menu_DrawItemList");
#define Menu_DrawItemList (*(M2C_UNK *)&Menu_DrawItemList_o)

void Menu_CreateInventoryTabView(void) {
    s32 var_a0;
    s32 var_a1;
    s32 var_v1;
    void *temp_a0;
    void *temp_s1;
    void *temp_v0;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0) == 0) {
        temp_v0 = MenuWidget_CreateSimpleNode(0, 0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(0, temp_v0, temp_v0);
        temp_a0 = temp_s1;
        M2C_FIELD(temp_v0, M2C_UNK **, 0x2C) = &Menu_StepItemSelectScreen;
        M2C_FIELD(temp_s1, M2C_UNK **, 0x30) = &Menu_DrawItemList;
        MenuWidget_SetCurrentNode(temp_a0);
        if (Menu_GetBattleEquipMode() != 0) {
            var_v1 = g_MenuItemContextFlag & 0x1F;
        } else {
            var_v1 = g_MenuItemContextFlag & 0x1EF;
        }
        var_a1 = 0;
        var_a0 = 8;
        do {
            var_a1 += var_v1 & 1;
            var_a0 -= 1;
            var_v1 = var_v1 >> 1;
        } while (var_a0 >= 0);
        Draw_SetPrimCallback(temp_s1, var_a1);
        g_MenuSelectionLocked = 0;
        g_MenuBattleStatusOverlayActive = 1;
        Menu_CreateBonusPointAllocationView();
        Menu_CreateContextHelpPanel();
    }
}
