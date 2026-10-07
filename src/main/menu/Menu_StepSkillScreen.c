#include "common.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
int MenuWidget_IsCursorYClear(MenuWidgetNode *ptr);
M2C_UNK func_80047678();
void Menu_OpenItemList(void);
int Battle_IsInputAllowedWrapped(void);
extern s32 g_MenuLayoutLocked;
void Menu_SetupSkillSubmenu(int arg0);

s32 Menu_StepSkillScreen(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *parg0;
    s32 parg1;
    s32 tv4k;
    MenuWidgetNode *temp_v0;
    register MenuWidgetNode *node asm("$16");
    register MenuWidgetNode *temp_v0_3 asm("$18");
    register s32 cm1 asm("$17");

    parg0 = arg0;
    parg1 = arg1;
    node = MenuWidget_GetChild(arg0, NULL);
    if (parg1 & 0x10000) {
        if (Inv_GetPackedListCount() != 0) {
            if (g_MenuLayoutLocked != 0) {
                Menu_OpenItemList();
                return 1;
            }
            if (Battle_IsInputAllowedWrapped() != 0) {
                temp_v0 = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                (MenuWidget_CreateNode(0x35, temp_v0, temp_v0))->draw = &Menu_SetupSkillSubmenu;
                node = MenuWidget_FindByModeAndSelectedBase(2, 7);
                node->cursor_x = 0;
                MenuWidget_SetCurrentNode(node);
                Menu_PlayConfirmSound();
                return 1;
            }
            goto block_6;
        }
block_6:
        Menu_PlayErrorSound();
        return 1;
    }
    if (parg1 & 0x40) {
        if (g_MenuLayoutLocked != 0) {
            func_80047678();
            return 1;
        }
        MenuWidget_NavScrollTo(7);
        MenuWidget_DestroyNode(parg0);
        temp_v0_3 = MenuWidget_FindByModeAndSelectedBase(1, 6);
        if (temp_v0_3 != NULL) {
            MenuWidget_DestroyNode(temp_v0_3);
            Menu_CreateBonusPointAllocationView();
            Menu_PlayCancelSound();
            return 1;
        }
        /* Duplicate return node #20. Try simplifying control flow for better match */
        return 1;
    }
    tv4k = parg1 & 0x4000;
    if (tv4k && (MenuWidget_IsCursorYClear(MenuWidget_FindByModeAndSelectedBase(1, 6)) != 0)) {
        cm1 = -1;
        node->cursor_x = cm1;
        node = MenuWidget_GetChild(parg0, 1);
        if (node != NULL) {
            node->cursor_x = cm1;
        }
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node != NULL) {
            node->cursor_y = 0;
            node->cursor_x = 0;
            MenuWidget_SetCurrentNode(node);
        }
        Menu_PlayMoveSound();
    }
    return 1;
}
