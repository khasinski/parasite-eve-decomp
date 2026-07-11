
int MenuWidget_GridCellIndex(int arg0);
int Inv_GetPackedListItem(int arg0);
int BattleCmd_GetRemainingAmmo(int *out);
int Inv_GetSlotHighlightState(int arg0, int arg1);
void Draw_AllocColorQuad(int arg0, int arg1);
void Menu_DrawModNameItem(void);
void MenuWidget_DrawList(int arg0, void (*callback)(void));

void Menu_DrawEquipModList(int arg0) {
    int temp;
    int saved_arg;
    int var_s0;
    int var_s1;
    int ret;

    saved_arg = arg0;
    var_s1 = Inv_GetPackedListItem(MenuWidget_GridCellIndex(arg0));
    var_s0 = BattleCmd_GetRemainingAmmo(&temp);
    ret = Inv_GetSlotHighlightState(var_s1, var_s0);
    Draw_AllocColorQuad(var_s0 - ret, temp);
    MenuWidget_DrawList(saved_arg, Menu_DrawModNameItem);
}
