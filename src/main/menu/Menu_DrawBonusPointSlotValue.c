/* CC1_FLAGS: -g3 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

int g_MenuItemUseMode;
int g_BonusPointDisplayValue;

void Draw_OffsetCursor(int x, int y);
void Draw_AllocSprite(int arg0);
void Draw_PrintNumberWidth5(int arg0);

void Menu_DrawBonusPointSlotValue(void) {
    int value;

    Draw_OffsetCursor(6, 4);
    Draw_AllocSprite(0x93);
    value = g_MenuItemUseMode;
    Draw_OffsetCursor((value * 5 * 4) + 0x48, 0);
    Draw_PrintNumberWidth5(g_BonusPointDisplayValue);
}
