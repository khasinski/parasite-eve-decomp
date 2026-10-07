#include "pe1/menu_inventory.h"
#include "pe1/menu_draw_list.h"

int Inv_GetPackedListItem(int arg0);
int BattleCmd_GetRemainingAmmo(int *out);
int Inv_GetSlotHighlightState(int arg0, int arg1);
void Draw_AllocColorQuad(int arg0, int arg1);
void Menu_DrawModNameItem(int index) __asm__("func_80050B48");

void Menu_DrawEquipModList(MenuWidgetNode *arg0) {
    int temp;
    MenuWidgetNode *saved_arg;
    int var_s0;
    int var_s1;
    int ret;

    saved_arg = arg0;
    var_s1 = Inv_GetPackedListItem(MenuWidget_GridCellIndex(arg0));
    var_s0 = BattleCmd_GetRemainingAmmo(&temp);
    asm volatile("" : "=r"(var_s0) : "0"(var_s0));
    ret = Inv_GetSlotHighlightState(var_s1, var_s0);
    Draw_AllocColorQuad(var_s0 - ret, temp);
    MenuWidget_DrawList(saved_arg, Menu_DrawModNameItem);
}
