#include "common.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selectedBase);
void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);
void MenuWidget_SetCurrentNode(MenuWidgetNode *node);
void Menu_PlayConfirmSound(void);
void Menu_PlayErrorSound(void);
int Inv_GetActiveListItemType(int index);
int Inv_GetPackedListCount(void);
void Inv_BuildFilteredPackedList(int mask);
void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
int Inv_RestoreSelection(unsigned int index);
MenuWidgetNode *MenuWidget_CreateSimpleNode(int mode, MenuWidgetNode *parent, int arg2, int arg3);
MenuWidgetNode *MenuWidget_CreateNode(int mode, MenuWidgetNode *parent, MenuWidgetNode *sibling);
M2C_UNK func_80064B74();
void MenuWidget_ClearColumnLayout(void *node);
extern s32 g_MenuEquipMode;
extern s32 g_InvItemUsableFlag;
extern s32 g_MenuLayoutLocked;
int Menu_StepEquipConfirm(MenuWidgetNode *node, int input);
void Menu_DrawEquipSelectionList(MenuWidgetNode *node);

void Menu_StepSkillList(MenuWidgetNode *arg0, s32 arg1) {
    M2C_UNK var_a0_2;
    MenuWidgetNode *saved_arg0 = arg0;
    s32 saved_arg1 = arg1;
    s32 temp_v0;
    s32 temp_shift;
    MenuWidgetNode *child;
    MenuWidgetNode *parent;

    if (g_MenuLayoutLocked != 0) {
        temp_v0 = Inv_RestoreSelection(0);
        if ((Inv_GetActiveListItemType(temp_v0) != 0) && (Inv_GetActiveListItemType(temp_v0) < 6)) {
            Inv_BuildFilteredPackedListExcluding(0x3E, temp_v0);
        } else {
            s32 mask;
            temp_shift = Inv_GetActiveListItemType(temp_v0);
            {
                register s32 one asm("$3") = 1;
                mask = one << temp_shift;
            }
            Inv_BuildFilteredPackedListExcluding(mask, temp_v0);
        }
        parent = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
        MenuWidget_OffsetPosition(parent, 0xB4 - parent->x, 0xA4 - parent->y);
    } else {
        var_a0_2 = 0x200;
        if (g_InvItemUsableFlag != 0) {
            var_a0_2 = 0x1FE;
        }
        Inv_BuildFilteredPackedList(var_a0_2);
    }
    if ((g_MenuEquipMode != 0) || (Inv_GetPackedListCount() != 0)) {
        parent = MenuWidget_CreateSimpleNode(7, saved_arg0, 0, 0);
        child = MenuWidget_CreateNode(7, parent, parent);
        parent->update = (void (*)())Menu_StepEquipConfirm;
        {
            s32 temp_v1 = g_MenuLayoutLocked;
            child->draw = &Menu_DrawEquipSelectionList;
            if (temp_v1 != 0) {
                MenuWidget_ClearColumnLayout(child);
            }
        }
        if (g_InvItemUsableFlag == 0) {
            func_80064B74(child, 0x13);
        }
        if (saved_arg1 == 0) {
            child->cursor_x = -1;
        } else {
            child->cursor_x = 0;
        }
        if (child->cursor_y < 0) {
            child->cursor_y = 0;
        }
        if (child->cursor_x >= 0) {
            MenuWidget_SetCurrentNode(child);
        }
        Draw_SetPrimCallback(child, Inv_GetPackedListCount());
        if (g_MenuEquipMode != 0) {
            child->cursor_x = -1;
            MenuWidget_OffsetPosition(parent, 0, 0x14);
        }
        Menu_PlayConfirmSound();
        return;
    }
    Menu_PlayErrorSound();
}
