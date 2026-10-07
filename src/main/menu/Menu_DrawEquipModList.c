#include "pe1/menu_inventory.h"
#include "pe1/menu_draw_list.h"

int Inv_GetPackedListItem(int arg0);
int BattleCmd_GetRemainingAmmo(int *out);
int Inv_GetSlotHighlightState(int arg0, int arg1);
void Draw_AllocColorQuad(int arg0, int arg1);
void Menu_DrawModNameItem(int index) __asm__("func_80050B48");

void Menu_DrawEquipModList(MenuWidgetNode *arg0) {
    int ammo;
    MenuWidgetNode *saved_arg;
    int remaining;
    int item;

    saved_arg = arg0;
    item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(arg0));
    remaining = BattleCmd_GetRemainingAmmo(&ammo);
    {
        int highlighted = Inv_GetSlotHighlightState(item, remaining);
        remaining -= highlighted;
    }
    Draw_AllocColorQuad(remaining, ammo);
    MenuWidget_DrawList(saved_arg, Menu_DrawModNameItem);
}
