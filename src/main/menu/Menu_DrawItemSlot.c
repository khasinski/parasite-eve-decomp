/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuItemContextFlag;
int g_MenuActiveWidget;

int Menu_GetBattleEquipMode(void);
int MenuWidget_GridCellIndex(int arg0);
int Battle_IsActiveWrapped();
void Draw_SetTextDimmed(int value);
void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int arg0);

void Menu_DrawItemSlot(int arg0) {
    int var_s0;
    int var_s1;
    int mask;
    int index;

    var_s1 = arg0;

    if (Menu_GetBattleEquipMode() != 0) {
        mask = g_MenuItemContextFlag & 0x1F;
    } else {
        mask = g_MenuItemContextFlag & 0x1EF;
    }

    index = -1;
    while (index < 9 && var_s1 >= 0) {
        var_s1 -= mask & 1;
        mask >>= 1;
        index++;
    }

    var_s0 = index;
    if (arg0 == MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        if (var_s0 == 4) {
            if (Battle_IsActiveWrapped() == 0) {
                Draw_SetTextDimmed(1);
            }
        }
    } else {
        Draw_SetTextDimmed(1);
    }

    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(var_s0 + 0x56);
}
