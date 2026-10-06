/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/text.h"

int g_InvItemUsableFlag;
int g_MenuLayoutLocked;
extern signed char g_AyaEquippedWeaponSlot[];
extern signed char g_AyaEquippedArmorSlot[];

int Battle_IsInputAllowedWrapped(void);
void Draw_SetTextDimmed(int arg0);

void Menu_SoundTestSelect(void)
{
    int selected;
    int value;
    void *node;

    if (g_MenuLayoutLocked != 0) {
        selected = Inv_RestoreSelection(0);
    } else {
        Draw_SetTextDimmed(Battle_IsInputAllowedWrapped() == 0);
        if (g_InvItemUsableFlag != 0) {
            value = g_AyaEquippedWeaponSlot[0];
        } else {
            value = g_AyaEquippedArmorSlot[0];
        }
        selected = value;
    }

    if (selected >= 0) {
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 6));
        Sfx_DrawActiveListSlot(selected);
        return;
    }

    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 6));
    node = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (*(int *)((char *)node + 0x44) >= 0) {
        *(int *)((char *)node + 0x44) = -1;
        node = MenuWidget_FindByModeAndSelectedBase(2, 5);
        *(int *)((char *)node + 0x44) = 0;
        MenuWidget_SetCurrentNode(node);
    }
    Draw_PrintTextById(0x39);
}
