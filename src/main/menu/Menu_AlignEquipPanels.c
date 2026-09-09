#include "common.h"
#include "pe1/menu_inventory.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

void MenuWidget_ClearCursorY(MenuWidgetNode *node);
void MenuWidget_OffsetPosition(MenuWidgetNode *node, int dx, int dy);
extern s32 g_MenuItemUseMode;

void Menu_AlignEquipPanels(void) {
    register s32 temp_s1 asm("$17");
    MenuWidgetNode *temp_s0;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;
    MenuWidgetNode *child;
    register int x asm("$5");
    register int target asm("$2");

    temp_s0 = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
    __asm__ volatile("" : "=r"(temp_s0) : "0"(temp_s0));
    child = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
    target = g_MenuItemUseMode;
    x = temp_s0->x;
    if (target != 0) {
        target = 0xB0;
        goto aligned;
    }
    target = (s32)child->popup_node;
    if (target != 0) {
        target = 0xA2;
        goto aligned;
    }
    target = 0x9C;
aligned:
    temp_s1 = target - x;
    asm("" : : "r"(temp_s1));
    MenuWidget_OffsetPosition(temp_s0, temp_s1, 0);
    MenuWidget_ClearCursorY(temp_s0);
    temp_v0 = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    MenuWidget_OffsetPosition(temp_v0, temp_s1, 0);
    MenuWidget_ClearCursorY(temp_v0);
    temp_v0_2 = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
    MenuWidget_OffsetPosition(temp_v0_2, temp_s1, 0);
    MenuWidget_ClearCursorY(temp_v0_2);
}

void MenuWidget_SetCursorY(MenuWidgetNode *node);

void Menu_SetEquipPanelsCursorY(void) {
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xF));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xB));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x2F));
}
